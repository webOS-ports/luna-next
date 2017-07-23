/***************************************************************************
**
** Copyright (C) 2014 Jolla Ltd.
** Contact: Giulio Camuffo <giulio.camuffo@jollamobile.com>
**
** This file is part of lipstick.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <sys/time.h>
#if 0
#include <grp.h>
#endif

#include <QMutexLocker>

#include "recorder.h"
#include "compositor.h"

namespace luna
{

static uint32_t getTime()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

static const QEvent::Type FrameEventType = (QEvent::Type)QEvent::registerEventType();
static const QEvent::Type FailedEventType = (QEvent::Type)QEvent::registerEventType();

class FrameEvent : public QEvent
{
public:
    FrameEvent(uint32_t t)
        : QEvent(FrameEventType)
        , time(t)
    { }
    uint32_t time;
};

class FailedEvent : public QEvent
{
public:
    FailedEvent(int r)
        : QEvent(FailedEventType)
        , result(r)
    { }
    int result;
};


RecorderManager::RecorderManager(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate(compositor)
{
}

void RecorderManager::recordFrame(QWindow *window)
{
    QMutexLocker lock(&m_mutex);
    if (m_requests.isEmpty())
        return;

    uchar *pixels;
    uint32_t time = getTime();
    foreach (Recorder *recorder, m_requests.values(window)) {
        wl_shm_buffer *buffer = recorder->buffer();
        pixels = static_cast<uchar *>(wl_shm_buffer_get_data(buffer));
        int width = wl_shm_buffer_get_width(buffer);
        int height = wl_shm_buffer_get_height(buffer);
        int stride = wl_shm_buffer_get_stride(buffer);
        int bpp = 4;

        if (width < window->width() || height < window->height() || stride < window->width() * bpp) {
            qApp->postEvent(recorder, new FailedEvent(QtWaylandServer::luna_recorder::result_bad_buffer));
            continue;
        }

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        qApp->postEvent(recorder, new FrameEvent(time));

        m_requests.remove(window, recorder);
    }
}

void RecorderManager::requestFrame(QWindow *window, Recorder *recorder)
{
    QMutexLocker lock(&m_mutex);
    m_requests.insert(window, recorder);
}

void RecorderManager::remove(QWindow *window, Recorder *recorder)
{
    QMutexLocker lock(&m_mutex);
    m_requests.remove(window, recorder);
}

void RecorderManager::recorder_manager_create_recorder(Resource *resource, uint32_t id, ::wl_resource *output)
{
    QWaylandOutput *pOutput = QWaylandOutput::fromResource(output);

    new Recorder(this, resource->client(), id, static_cast<QQuickWindow*>(pOutput->window()));
    Compositor::instance()->setRecording(true);
}


Recorder::Recorder(RecorderManager *manager, wl_client *client, quint32 id, QQuickWindow *window)
                : QWaylandCompositorExtensionTemplate(manager)
                , QtWaylandServer::luna_recorder(client, id, 1)
                , m_manager(manager)
                , m_bufferResource(Q_NULLPTR)
                , m_client(client)
                , m_window(window)
{
    send_setup(window->width(), window->height(), window->width() * 4, WL_SHM_FORMAT_RGBA8888);
}

Recorder::~Recorder()
{
    m_manager->remove(m_window, this);
    Compositor::instance()->setRecording(false);
}

void Recorder::recorder_destroy_resource(Resource *resource)
{
    Q_UNUSED(resource)
    delete this;
}

void Recorder::recorder_destroy(Resource *resource)
{
    wl_resource_destroy(resource->handle);
}

void Recorder::recorder_record_frame(Resource *resource, ::wl_resource *buffer)
{
    Q_UNUSED(resource)
    if (m_bufferResource) {
        send_cancelled(buffer);
    }
    m_bufferResource = buffer;
    m_buffer = wl_shm_buffer_get(buffer);
    if (m_buffer) {
        m_manager->requestFrame(m_window, this);
    } else {
        m_bufferResource = Q_NULLPTR;
        send_failed(result_bad_buffer, buffer);
    }
}

void Recorder::recorder_repaint(Resource *resource)
{
    Q_UNUSED(resource)
    if (m_bufferResource) {
        m_window->update();
    }
}

bool Recorder::event(QEvent *e)
{
    if (e->type() == FrameEventType) {
        FrameEvent *fe = static_cast<FrameEvent *>(e);
        send_frame(m_bufferResource, fe->time, QtWaylandServer::luna_recorder::transform_y_inverted);
    } else if (e->type() == FailedEventType) {
        FailedEvent *fe = static_cast<FailedEvent *>(e);
        send_failed(fe->result, m_bufferResource);
    } else {
        return QObject::event(e);
    }

    m_bufferResource = Q_NULLPTR;
    wl_client_flush(client());
    return true;
}

} // namespace luna
