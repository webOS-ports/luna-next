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

#ifndef LIPSTICKRECORDER_RECORDER_H
#define LIPSTICKRECORDER_RECORDER_H

#include <QObject>
#include <QMutex>
#include <QIODevice>
#include <wayland-client.h>

class QScreen;

struct wl_display;
struct wl_registry;
struct luna_recorder_manager;
struct luna_recorder;

class Buffer;
class BuffersHandler;

class Recorder : public QObject
{
    Q_OBJECT
public:
    Recorder(QIODevice *output, unsigned int numberOfFrames = 0);
    ~Recorder();

private slots:
    void start();
    void recordFrame();

private:
    static void global(void *data, wl_registry *registry, uint32_t id, const char *interface, uint32_t version);
    static void globalRemove(void *data, wl_registry *registry, uint32_t id);
    static void setup(void *data, luna_recorder *recorder, int width, int height, int stride, int format);
    static void frame(void *data, luna_recorder *recorder, wl_buffer *buffer, uint32_t time, int transform);
    static void failed(void *data, luna_recorder *recorder, int result, wl_buffer *buffer);
    static void cancel(void *data, luna_recorder *recorder, wl_buffer *buffer);

    wl_display *m_display;
    wl_registry *m_registry;
    wl_shm *m_shm;
    luna_recorder_manager *m_manager;
    luna_recorder *m_recorder;
    QScreen *m_screen;
    QList<Buffer *> m_buffers;
    bool m_starving;
    QThread *m_buffersThread;
    BuffersHandler *m_buffersHandler;
    QMutex m_mutex;

    friend class BuffersHandler;
};

#endif
