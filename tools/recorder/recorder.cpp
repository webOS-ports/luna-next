/***************************************************************************
**
** Copyright (C) 2014 Jolla Ltd.
** Contact: Giulio Camuffo <giulio.camuffo@jollamobile.com>
**
** This file is part of lipstick-recorder.
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <QGuiApplication>
#include <QScreen>
#include <QImage>
#include <QtGui/qpa/qplatformnativeinterface.h>
#include <QDebug>
#include <QThread>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QFile>
#include <QDebug>

#include "wayland-luna-recorder-client-protocol.h"
#include "recorder.h"

class Buffer
{
public:
    static Buffer *create(wl_shm *shm, int width, int height, int stride, int format)
    {
        int size = stride * height;

        char filename[] = "/tmp/luna-recorder-shm-XXXXXX";
        int fd = mkstemp(filename);
        if (fd < 0) {
            qWarning("creating a buffer file for %d B failed: %m\n", size);
            return Q_NULLPTR;
        }
        int flags = fcntl(fd, F_GETFD);
        if (flags != -1)
            fcntl(fd, F_SETFD, flags | FD_CLOEXEC);

        if (ftruncate(fd, size) < 0) {
            qWarning("ftruncate failed: %s", strerror(errno));
            close(fd);
            return Q_NULLPTR;
        }

        uchar *data = (uchar *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        unlink(filename);
        if (data == (uchar *)MAP_FAILED) {
            qWarning("mmap failed: %m\n");
            close(fd);
            return Q_NULLPTR;
        }

        Buffer *buf = new Buffer;

        wl_shm_pool *pool = wl_shm_create_pool(shm, fd, size);
        buf->buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
        wl_buffer_set_user_data(buf->buffer, buf);
        wl_shm_pool_destroy(pool);
        buf->data = data;
        buf->image = QImage(data, width, height, stride, QImage::Format_RGBA8888);
        buf->busy = false;
        close(fd);
        return buf;
    }

    wl_buffer *buffer;
    uchar *data;
    QImage image;
    bool busy;
};

static const QEvent::Type FrameEventType = (QEvent::Type)QEvent::registerEventType();

class FrameEvent : public QEvent
{
public:
    FrameEvent(Buffer *b, uint32_t time, int tr)
        : QEvent(FrameEventType)
        , buffer(b)
        , timestamp(time)
        , transform(tr)
    {
    }

    Buffer *buffer;
    uint32_t timestamp;
    int transform;
};

class BuffersHandler : public QObject
{
public:
    BuffersHandler(QIODevice *output, unsigned int numberOfFrames) :
        output(output),
        countingFrames(numberOfFrames > 0),
        remainingFrames(numberOfFrames)
    {
    }

    bool event(QEvent *e) Q_DECL_OVERRIDE
    {
        if (e->type() == FrameEventType) {
            FrameEvent *fe = static_cast<FrameEvent *>(e);
            Buffer *buf = fe->buffer;
            static int id = 0;
            QImage img = fe->transform == LUNA_RECORDER_TRANSFORM_Y_INVERTED ? buf->image.mirrored(false, true) : buf->image;
            buf->busy = false;
            if (rec->m_starving)
                rec->recordFrame();

            output->write((const char*) img.bits(), img.byteCount());

            if (countingFrames && --remainingFrames == 0)
                QCoreApplication::exit(0);

            return true;
        }
        return QObject::event(e);
    }

    Recorder *rec;

private:
    QIODevice *output;
    bool countingFrames;
    unsigned int remainingFrames;
};

static void callback(void *data, wl_callback *cb, uint32_t time)
{
    Q_UNUSED(time)
    wl_callback_destroy(cb);
    QMetaObject::invokeMethod(static_cast<Recorder *>(data), "start");
}

Recorder::Recorder(QIODevice *output, unsigned int numberOfFrames)
        : QObject()
        , m_manager(Q_NULLPTR)
        , m_starving(false)
{
    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    m_display = static_cast<wl_display *>(native->nativeResourceForIntegration("display"));
    m_registry = wl_display_get_registry(m_display);

    static const wl_registry_listener registryListener = {
        global,
        globalRemove
    };
    wl_registry_add_listener(m_registry, &registryListener, this);

    wl_callback *cb = wl_display_sync(m_display);
    static const wl_callback_listener callbackListener = {
        callback
    };
    wl_callback_add_listener(cb, &callbackListener, this);

    m_buffersThread = new QThread;
    m_buffersHandler = new BuffersHandler(output, numberOfFrames);
    m_buffersHandler->rec = this;
    m_buffersHandler->moveToThread(m_buffersThread);
    m_buffersThread->start();
}

Recorder::~Recorder()
{
    m_buffersThread->quit();
    m_buffersThread->wait();
    luna_recorder_destroy(m_recorder);
    delete m_buffersHandler;
    delete m_buffersThread;
}

void Recorder::start()
{
    if (!m_manager)
        qFatal("The luna_recorder_manager global is not available.");

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    m_screen = QGuiApplication::screens().first();
    wl_output *output = static_cast<wl_output *>(native->nativeResourceForScreen("output", m_screen));

    m_recorder = luna_recorder_manager_create_recorder(m_manager, output);
    static const luna_recorder_listener recorderListener = {
        setup,
        frame,
        failed,
        cancel
    };
    luna_recorder_add_listener(m_recorder, &recorderListener, this);
}

void Recorder::recordFrame()
{
    Buffer *buf = Q_NULLPTR;
    foreach (Buffer *b, m_buffers) {
        if (!b->busy) {
            buf = b;
            break;
        }
    }
    if (buf) {
        luna_recorder_record_frame(m_recorder, buf->buffer);
        wl_display_flush(m_display);
        buf->busy = true;
        m_starving = false;
    } else {
        qWarning("No free buffers.");
        m_starving = true;
    }
}

QString formatToString(int format)
{
    switch (format) {
        case WL_SHM_FORMAT_RGBA8888:
            return QString("rgba8888");
        default:
            break;
    }

    return QString("unknown");
}

void Recorder::setup(void *data, luna_recorder *recorder, int width, int height, int stride, int format)
{
    Recorder *rec = static_cast<Recorder *>(data);
    QMutexLocker lock(&rec->m_mutex);

    qWarning("Recording with output size %dx%d format %s",
             width, height, formatToString(format).toUtf8().constData());

    for (int i = 0; i < 6; ++i) {
        Buffer *buffer = Buffer::create(rec->m_shm, width, height, stride, format);
        if (!buffer)
            qFatal("Failed to create a buffer.");
        rec->m_buffers << buffer;
    }
    rec->recordFrame();
}

void Recorder::frame(void *data, luna_recorder *recorder, wl_buffer *buffer, uint32_t timestamp, int transform)
{
    Q_UNUSED(recorder)

    Recorder *rec = static_cast<Recorder *>(data);
    static uint32_t time = 0;

    QMutexLocker lock(&rec->m_mutex);
    rec->recordFrame();
    Buffer *buf = static_cast<Buffer *>(wl_buffer_get_user_data(buffer));

    time = timestamp;

    qApp->postEvent(rec->m_buffersHandler, new FrameEvent(buf, timestamp, transform));
}

void Recorder::failed(void *data, luna_recorder *recorder, int result, wl_buffer *buffer)
{
    Q_UNUSED(data)
    Q_UNUSED(recorder)
    Q_UNUSED(buffer)

    qFatal("Failed to record a frame, result %d.", result);
}

void Recorder::cancel(void *data, luna_recorder *recorder, wl_buffer *buffer)
{
    Q_UNUSED(recorder)

    Recorder *rec = static_cast<Recorder *>(data);

    QMutexLocker lock(&rec->m_mutex);
    Buffer *buf = static_cast<Buffer *>(wl_buffer_get_user_data(buffer));
    buf->busy = false;
}

void Recorder::global(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version)
{
    Q_UNUSED(registry)

    Recorder *rec = static_cast<Recorder *>(data);
    if (strcmp(interface, "luna_recorder_manager") == 0) {
        rec->m_manager = static_cast<luna_recorder_manager *>(wl_registry_bind(registry, id, &luna_recorder_manager_interface, qMin(version, 1u)));
    } else if (strcmp(interface, "wl_shm") == 0) {
        rec->m_shm = static_cast<wl_shm *>(wl_registry_bind(registry, id, &wl_shm_interface, qMin(version, 1u)));
    }
}

void Recorder::globalRemove(void *data, wl_registry *registry, uint32_t id)
{
    Q_UNUSED(data)
    Q_UNUSED(registry)
    Q_UNUSED(id)
}
