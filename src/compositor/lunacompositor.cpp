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

#include "lunacompositor.h"

LunaCompositor::LunaCompositor()
    : QWaylandCompositor(this),
      m_fullscreenSurface(0)
{
    enableSubSurfaceExtension();
    setSource(QUrl("qrc:///qml/main.qml"));
    setResizeMode(QQuickView::SizeRootObjectToView);
    setColor(Qt::black);
    winId();

    connect(this, SIGNAL(frameSwapped()), this, SLOT(frameSwappedSlot()));
}

void LunaCompositor::destroyWindow(QVariant window)
{
    qvariant_cast<QObject *>(window)->deleteLater();
}

void LunaCompositor::destroyClientForWindow(QVariant window)
{
    QWaylandSurface *surface = 0;

    surface = qobject_cast<QWaylandSurfaceItem *>(qvariant_cast<QObject *>(window))->surface();
    destroyClientForSurface(surface);
}

void LunaCompositor::setFullscreenSurface(QWaylandSurface *surface)
{
    if (surface == m_fullscreenSurface)
        return;

    m_fullscreenSurface = surface;
    emit fullscreenSurfaceChanged();
}

void LunaCompositor::surfaceMapped()
{
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

void LunaCompositor::surfaceUnmapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    if (surface == m_fullscreenSurface)
        m_fullscreenSurface = 0;

    QQuickItem *item = surface->surfaceItem();
    emit windowDestroyed(QVariant::fromValue(item));
}

void LunaCompositor::surfaceDestroyed(QObject *object)
{
    QWaylandSurface *surface = static_cast<QWaylandSurface *>(object);
    if (surface == m_fullscreenSurface)
        m_fullscreenSurface = 0;

    QQuickItem *item = surface->surfaceItem();
    emit windowDestroyed(QVariant::fromValue(item));
}

void LunaCompositor::frameSwappedSlot()
{
    frameFinished(m_fullscreenSurface);
}

void LunaCompositor::resizeEvent(QResizeEvent *event)
{
    QQuickView::resizeEvent(event);
    QWaylandCompositor::setOutputGeometry(QRect(0, 0, width(), height()));
}

void LunaCompositor::surfaceCreated(QWaylandSurface *surface)
{
    connect(surface, SIGNAL(destroyed(QObject *)), this, SLOT(surfaceDestroyed(QObject *)));
    connect(surface, SIGNAL(mapped()), this, SLOT(surfaceMapped()));
    connect(surface, SIGNAL(unmapped()), this,SLOT(surfaceUnmapped()));
}
