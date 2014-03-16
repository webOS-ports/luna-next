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
#include "windowmodel.h"

#include <QWaylandInputDevice>

namespace luna
{

Compositor* Compositor::mInstance = 0;

Compositor::Compositor()
    : QWaylandCompositor(this),
      mFullscreenSurface(0),
      mNextWindowId(1)
{
    if (mInstance)
        qFatal("Compositor: Only one compositor instance per process is supported");

    mInstance = this;

    connect(this, SIGNAL(frameSwapped()), this, SLOT(frameSwappedSlot()));
}

Compositor* Compositor::instance()
{
    return mInstance;
}

void Compositor::classBegin()
{
}

void Compositor::componentComplete()
{
    QWaylandCompositor::setOutputGeometry(QRect(0, 0, width(), height()));
}

void Compositor::registerWindowModel(WindowModel *model)
{
    mWindowModels.append(model);
}

void Compositor::unregisterWindowModel(WindowModel *model)
{
    mWindowModels.removeAll(model);
}

CompositorWindow* Compositor::windowForId(int id)
{
    if (!mWindows.contains(id))
        return 0;

    return mWindows[id];
}

void Compositor::setFullscreenSurface(QWaylandSurface *surface)
{
    if (surface == mFullscreenSurface)
        return;

    // Prevent flicker when returning to composited mode
    if (!surface && mFullscreenSurface && mFullscreenSurface->surfaceItem())
        mFullscreenSurface->surfaceItem()->update();

    mFullscreenSurface = surface;

    emit fullscreenSurfaceChanged();
}

void Compositor::clearKeyboardFocus()
{
    defaultInputDevice()->setKeyboardFocus(0);
}

void Compositor::closeWindowWithId(int winId)
{
    CompositorWindow *window = mWindows.value(winId, 0);
    if (window) {
        if (window->surface() && window->checkIsAllowedToStay())
            window->surface()->destroySurface();
        else if (window->surface())
            destroyClientForSurface(window->surface());

        delete window;
    }
}

void Compositor::surfaceMapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());

    if (!surface->hasShellSurface())
        return;

    bool bIsSurfaceNew = false;

    CompositorWindow *window = qobject_cast<CompositorWindow*>(surface->surfaceItem());
    //Create a CompositorWindow if we have not yet
    if (!window) {
        unsigned int windowId = mNextWindowId++;
        window = new CompositorWindow(windowId, surface, contentItem());
        window->setSize(surface->size());
        window->setPosition(surface->pos());
        window->setFlag(QQuickItem::ItemIsFocusScope, true);
        window->setUseTextureAlpha(true);

        // QObject::connect(window, SIGNAL(destroyed(QObject*)), this, SLOT(windowDestroyed(QObject*)));
        mWindows.insert(windowId, window);

        bIsSurfaceNew = true;
    }

    window->setTouchEventsEnabled(true);

    qWarning() << Q_FUNC_INFO << "the window " << window << "is going to be added/shown";

    if( bIsSurfaceNew ) {
        emit windowAdded(QVariant::fromValue(static_cast<QQuickItem*>(window)));
    }
    else {
        emit windowShown(QVariant::fromValue(static_cast<QQuickItem*>(window)));
    }

    foreach (WindowModel *model, mWindowModels)
        model->addWindow(window);
}

void Compositor::surfaceUnmapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    if (surface == mFullscreenSurface)
        setFullscreenSurface(0);

    CompositorWindow *window = qobject_cast<CompositorWindow*>(surface->surfaceItem());
    qWarning() << Q_FUNC_INFO << "the window " << window << "is going to be hidden";

    emit windowHidden(QVariant::fromValue(static_cast<QQuickItem*>(window)));

    foreach (WindowModel *model, mWindowModels)
        model->removeWindow(window);
}

void Compositor::surfaceAboutToBeDestroyed(QWaylandSurface *surface)
{
    CompositorWindow *window = static_cast<CompositorWindow*>(surface->surfaceItem());

    surface->setSurfaceItem(0);

    if (surface == mFullscreenSurface)
        setFullscreenSurface(0);

    if (window) {
        mWindows.remove(window->winId());
        emit windowRemoved(QVariant::fromValue(static_cast<QQuickItem*>(window)));

        window->setClosed(true);
        window->tryRemove();
    }
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
    connect(surface, SIGNAL(mapped()), this, SLOT(surfaceMapped()));
    connect(surface, SIGNAL(unmapped()), this, SLOT(surfaceUnmapped()));
    connect(surface, SIGNAL(raiseRequested()), this, SLOT(surfaceRaised()));
    connect(surface, SIGNAL(lowerRequested()), this, SLOT(surfaceLowered()));
    connect(surface, SIGNAL(sizeChanged()), this, SLOT(surfaceSizeChanged()));
    connect(surface, SIGNAL(damaged(const QRect&)), this, SLOT(surfaceDamaged(const QRect&)));
}

void Compositor::surfaceRaised()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface*>(sender());
    CompositorWindow *window = static_cast<CompositorWindow*>(surface->surfaceItem());

    qWarning() << Q_FUNC_INFO << "the window " << window << "is going to be raised";

    if (window)
        emit windowRaised(QVariant::fromValue(static_cast<QQuickItem*>(window)));
}

void Compositor::surfaceLowered()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface*>(sender());
    CompositorWindow *window = static_cast<CompositorWindow*>(surface->surfaceItem());

    qWarning() << Q_FUNC_INFO << "the window " << window << "is going to be lowered";

    if (window)
        emit windowLowered(QVariant::fromValue(static_cast<QQuickItem*>(window)));
}

void Compositor::surfaceSizeChanged()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());

    CompositorWindow *window = static_cast<CompositorWindow*>(surface->surfaceItem());
    if (window)
        window->setSize(surface->size());
}

void Compositor::surfaceDamaged(const QRect&)
{
    if (!isVisible())
        frameFinished(0);
}


} // namespace luna
