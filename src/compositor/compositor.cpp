/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "compositor.h"

namespace luna
{

Compositor::Compositor(const QUrl& compositorPath)
    : QWaylandCompositor(this),
      mFullscreenSurface(0),
      mNextWindowId(1)
{
    enableSubSurfaceExtension();
    setSource(compositorPath);
    setResizeMode(QQuickView::SizeRootObjectToView);
    setColor(Qt::black);
    winId();

    connect(this, SIGNAL(frameSwapped()), this, SLOT(frameSwappedSlot()));

    rootContext()->setContextProperty("compositor", this);

    connect(this, SIGNAL(windowAdded(QVariant)), rootObject(), SLOT(windowAdded(QVariant)));
    connect(this, SIGNAL(windowDestroyed(QVariant)), rootObject(), SLOT(windowDestroyed(QVariant)));
    connect(this, SIGNAL(windowResized(QVariant)), rootObject(), SLOT(windowResized(QVariant)));
}

void Compositor::destroyWindow(QVariant window)
{
    qvariant_cast<QObject *>(window)->deleteLater();
}

void Compositor::destroyClientForWindow(QVariant window)
{
    QWaylandSurface *surface = 0;

    surface = qobject_cast<QWaylandSurfaceItem *>(qvariant_cast<QObject *>(window))->surface();
    destroyClientForSurface(surface);
}

void Compositor::setFullscreenSurface(QWaylandSurface *surface)
{
    if (surface == mFullscreenSurface)
        return;

    mFullscreenSurface = surface;
    emit fullscreenSurfaceChanged();
}

void Compositor::surfaceMapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());

    if (!surface->hasShellSurface())
        return;

    unsigned int windowId = mNextWindowId++;
    CompositorWindow *window = new CompositorWindow(windowId, surface, contentItem());
    window->setSize(surface->size());
    // FIXME handle disconnecting clients
    // QObject::connect(window, SIGNAL(destroyed(QObject*)), this, SLOT(windowDestroyed()));
    mWindows.insert(windowId, window);

    window->setTouchEventsEnabled(true);

    emit windowAdded(QVariant::fromValue(static_cast<QQuickItem *>(window)));
}

void Compositor::surfaceUnmapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    if (surface == mFullscreenSurface)
        mFullscreenSurface = 0;

    CompositorWindow *window = qobject_cast<CompositorWindow*>(surface->surfaceItem());

    mWindows.remove(window->windowId());

    emit windowDestroyed(QVariant::fromValue(static_cast<QQuickItem*>(window)));

    delete window;
}

void Compositor::surfaceDestroyed(QObject *object)
{
    QWaylandSurface *surface = static_cast<QWaylandSurface *>(object);
    if (surface == mFullscreenSurface)
        mFullscreenSurface = 0;

    QQuickItem *item = surface->surfaceItem();
    emit windowDestroyed(QVariant::fromValue(item));
}

void Compositor::frameSwappedSlot()
{
    frameFinished(mFullscreenSurface);
}

void Compositor::resizeEvent(QResizeEvent *event)
{
    QQuickView::resizeEvent(event);
    QWaylandCompositor::setOutputGeometry(QRect(0, 0, width(), height()));
}

void Compositor::surfaceCreated(QWaylandSurface *surface)
{
    connect(surface, SIGNAL(destroyed(QObject *)), this, SLOT(surfaceDestroyed(QObject *)));
    connect(surface, SIGNAL(mapped()), this, SLOT(surfaceMapped()));
    connect(surface, SIGNAL(unmapped()), this,SLOT(surfaceUnmapped()));
}

} // namespace luna
