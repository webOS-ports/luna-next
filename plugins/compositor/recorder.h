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

#ifndef LIPSTICKCOMPOSITORRECORDER_H
#define LIPSTICKCOMPOSITORRECORDER_H

#include <QObject>
#include <QMultiHash>
#include <QMutex>
#include <QWaylandCompositorExtension>

#include "qwayland-server-luna-recorder.h"

struct wl_shm_buffer;
struct wl_client;

class QWindow;
class QQuickWindow;
class QEvent;

namespace luna
{

class Recorder;
class Compositor;

class RecorderManager : public QWaylandCompositorExtensionTemplate<RecorderManager>, public QtWaylandServer::luna_recorder_manager
{
    Q_OBJECT
public:
    RecorderManager(QWaylandCompositor *compositor);

    void recordFrame(QWindow *window);
    void requestFrame(QWindow *window, Recorder *recorder);
    void remove(QWindow *window, Recorder *recorder);

private:
    void recorder_manager_create_recorder(Resource *resource, uint32_t id, ::wl_resource *output) Q_DECL_OVERRIDE;

    QMultiHash<QWindow *, Recorder *> m_requests;
    QMutex m_mutex;
};

class Recorder : public QWaylandCompositorExtensionTemplate<Recorder>, public QtWaylandServer::luna_recorder
{
    Q_OBJECT
public:
    Recorder(RecorderManager *manager, wl_client *client, quint32 id, QQuickWindow *window);
    ~Recorder();

    wl_shm_buffer *buffer() const { return m_buffer; }
    wl_client *client() const { return m_client; }

protected:
    bool event(QEvent *e) Q_DECL_OVERRIDE;
    void recorder_destroy_resource(Resource *resource) Q_DECL_OVERRIDE;
    void recorder_destroy(Resource *resource) Q_DECL_OVERRIDE;
    void recorder_record_frame(Resource *resource, ::wl_resource *buffer) Q_DECL_OVERRIDE;
    void recorder_repaint(Resource *resource) Q_DECL_OVERRIDE;

private:
    RecorderManager *m_manager;
    wl_resource *m_bufferResource;
    wl_shm_buffer *m_buffer;
    wl_client *m_client;
    QQuickWindow *m_window;
};

} // namespace luna

#endif
