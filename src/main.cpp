/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwaylandcompositor.h"
#include "qwaylandsurface.h"
#include "qwaylandsurfaceitem.h"

#include <QGuiApplication>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>

#include <QQmlContext>

#include <QQuickItem>
#include <QQuickView>

#include <QFileInfo>

#include <unistd.h>
#include <stdio.h>

class QmlCompositor : public QQuickView, public QWaylandCompositor
{
    Q_OBJECT
    Q_PROPERTY(QWaylandSurface* fullscreenSurface READ fullscreenSurface WRITE setFullscreenSurface NOTIFY fullscreenSurfaceChanged)

public:
    QmlCompositor()
        : QWaylandCompositor(this)
        , m_fullscreenSurface(0)
    {
        enableSubSurfaceExtension();
        setSource(QUrl("qrc:///qml/main.qml"));
        setResizeMode(QQuickView::SizeRootObjectToView);
        setColor(Qt::black);
        winId();

	connect(this, SIGNAL(frameSwapped()), this, SLOT(frameSwappedSlot()));
    }

    QWaylandSurface *fullscreenSurface() const
    {
        return m_fullscreenSurface;
    }

signals:
    void windowAdded(QVariant window);
    void windowDestroyed(QVariant window);
    void windowResized(QVariant window);
    void fullscreenSurfaceChanged();

public slots:
    void destroyWindow(QVariant window) {
        qvariant_cast<QObject *>(window)->deleteLater();
    }

    void destroyClientForWindow(QVariant window) {
        QWaylandSurface *surface = qobject_cast<QWaylandSurfaceItem *>(qvariant_cast<QObject *>(window))->surface();
        destroyClientForSurface(surface);
    }

    void setFullscreenSurface(QWaylandSurface *surface) {
        if (surface == m_fullscreenSurface)
            return;
        m_fullscreenSurface = surface;
        emit fullscreenSurfaceChanged();
    }

private slots:
    void surfaceMapped() {
        QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
        //Ignore surface if it's not a window surface
        if (!surface->hasShellSurface())
            return;

        QWaylandSurfaceItem *item = surface->surfaceItem();
        //Create a WaylandSurfaceItem if we have not yet
        if (!item)
            item = new QWaylandSurfaceItem(surface, rootObject());

        item->setTouchEventsEnabled(true);
        //item->takeFocus();
        emit windowAdded(QVariant::fromValue(static_cast<QQuickItem *>(item)));
    }
    void surfaceUnmapped() {
        QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
        if (surface == m_fullscreenSurface)
            m_fullscreenSurface = 0;
        QQuickItem *item = surface->surfaceItem();
        emit windowDestroyed(QVariant::fromValue(item));
    }

    void surfaceDestroyed(QObject *object) {
        QWaylandSurface *surface = static_cast<QWaylandSurface *>(object);
        if (surface == m_fullscreenSurface)
            m_fullscreenSurface = 0;
        QQuickItem *item = surface->surfaceItem();
        emit windowDestroyed(QVariant::fromValue(item));
    }

    void frameSwappedSlot() {
        frameFinished(m_fullscreenSurface);
    }

protected:
    void resizeEvent(QResizeEvent *event)
    {
        QQuickView::resizeEvent(event);
        QWaylandCompositor::setOutputGeometry(QRect(0, 0, width(), height()));
    }

    void surfaceCreated(QWaylandSurface *surface) {
        connect(surface, SIGNAL(destroyed(QObject *)), this, SLOT(surfaceDestroyed(QObject *)));
        connect(surface, SIGNAL(mapped()), this, SLOT(surfaceMapped()));
        connect(surface,SIGNAL(unmapped()), this,SLOT(surfaceUnmapped()));
    }

private:
    QWaylandSurface *m_fullscreenSurface;
};

static int convertPermission(const QFileInfo &fileInfo)
{
    int p = 0;

    if (fileInfo.permission(QFile::ReadUser))
        p += 400;
    if (fileInfo.permission(QFile::WriteUser))
        p += 200;
    if (fileInfo.permission(QFile::ExeUser))
        p += 100;
    if (fileInfo.permission(QFile::ReadGroup))
        p += 40;
    if (fileInfo.permission(QFile::WriteGroup))
        p += 20;
    if (fileInfo.permission(QFile::ExeGroup))
        p += 10;
    if (fileInfo.permission(QFile::ReadOther))
        p += 4;
    if (fileInfo.permission(QFile::WriteOther))
        p += 2;
    if (fileInfo.permission(QFile::ExeOther))
        p += 1;

    return p;
}

static void verifyXdgRuntimeDir()
{
    QByteArray dirName = qgetenv("XDG_RUNTIME_DIR");

    if (dirName.isEmpty()) {
        QString msg = QObject::tr(
                    "The XDG_RUNTIME_DIR environment variable is not set.\n"
                    "Refer to your distribution on how to get it, or read\n"
                    "http://www.freedesktop.org/wiki/Specifications/basedir-spec\n"
                    "on how to implement it.\n");
        qFatal(msg.toUtf8());
    }

    QFileInfo fileInfo(dirName);

    if (!fileInfo.exists()) {
        QString msg = QObject::tr(
                    "The XDG_RUNTIME_DIR environment variable is set to "
                    "\"%1\", which doesn't exist.\n").arg(dirName.constData());
        qFatal(msg.toUtf8());
    }

    if (convertPermission(fileInfo) != 700 || fileInfo.ownerId() != getuid()) {
        QString msg = QObject::tr(
                    "XDG_RUNTIME_DIR is set to \"%1\" and is not configured correctly.\n"
                    "Unix access mode must be 0700, but is 0%2.\n"
                    "It must also be owned by the current user (UID %3), "
                    "but is owned by UID %4 (\"%5\").\n")
                .arg(dirName.constData())
                .arg(convertPermission(fileInfo))
                .arg(getuid())
                .arg(fileInfo.ownerId())
                .arg(fileInfo.owner());
        qFatal(msg.toUtf8());
    }
}

int main(int argc, char *argv[])
{
    if (!qgetenv("DISPLAY").isEmpty())
        setenv("QT_QPA_PLATFORM", "xcb", 0);
    else {
        setenv("QT_QPA_PLATFORM", "eglfs", 0);
        setenv("QT_QPA_GENERIC_PLUGINS", "evdevtouch", 0);
    }

    QGuiApplication app(argc, argv);

    verifyXdgRuntimeDir();

    QmlCompositor compositor;
    compositor.setTitle(QLatin1String("QML Compositor"));
    compositor.setGeometry(0, 0, 1024, 768);
    compositor.show();

    compositor.rootContext()->setContextProperty("compositor", &compositor);

    QObject::connect(&compositor, SIGNAL(windowAdded(QVariant)), compositor.rootObject(), SLOT(windowAdded(QVariant)));
    QObject::connect(&compositor, SIGNAL(windowDestroyed(QVariant)), compositor.rootObject(), SLOT(windowDestroyed(QVariant)));
    QObject::connect(&compositor, SIGNAL(windowResized(QVariant)), compositor.rootObject(), SLOT(windowResized(QVariant)));

    return app.exec();
}

#include "main.moc"
