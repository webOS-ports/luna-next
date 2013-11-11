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

#include <QWaylandInputDevice>

namespace luna
{

Compositor::Compositor()
    : QWaylandCompositor(this),
      mFullscreenSurface(0),
      mNextWindowId(1)
{
    connect(this, SIGNAL(frameSwapped()), this, SLOT(frameSwappedSlot()));
}

void Compositor::classBegin()
{
}

void Compositor::componentComplete()
{
    QWaylandCompositor::setOutputGeometry(QRect(0, 0, width(), height()));
}

void Compositor::setFullscreenSurface(QWaylandSurface *surface)
{
    if (surface == mFullscreenSurface)
        return;

    // Prevent flicker when returning to composited mode
    if (!surface && mFullscreenSurface && mFullscreenSurface->surfaceItem())
        mFullscreenSurface->surfaceItem()->update();

    mFullscreenSurface = surface;

    const bool directRenderingSucceeded = setDirectRenderSurface(mFullscreenSurface, openglContext());
    if (surface && !directRenderingSucceeded)
        qWarning() << Q_FUNC_INFO << "failed to set direct rendering surface";

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
        // Determine the parent the future CompositorWindow.
        // If there is a transient parent on the surface, try to put
        // the corresponding parent for the CompositorWindow.
        QQuickItem *surfaceItemParent = contentItem();
        QWaylandSurface *surfaceTransientParent = surface->transientParent();
        if( surfaceTransientParent ) {
            CompositorWindow *windowParent = qobject_cast<CompositorWindow*>(surfaceTransientParent->surfaceItem());
            if( windowParent )
                surfaceItemParent = windowParent;
        }

        unsigned int windowId = mNextWindowId++;
        window = new CompositorWindow(windowId, surface, surfaceItemParent);
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
}

void Compositor::surfaceUnmapped()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    if (surface == mFullscreenSurface)
        setFullscreenSurface(0);

    CompositorWindow *window = qobject_cast<CompositorWindow*>(surface->surfaceItem());
    qWarning() << Q_FUNC_INFO << "the window " << window << "is going to be hidden";

    emit windowHidden(QVariant::fromValue(static_cast<QQuickItem*>(window)));
}

void Compositor::surfaceAboutToBeDestroyed(QWaylandSurface *surface)
{
    CompositorWindow *window = static_cast<CompositorWindow*>(surface->surfaceItem());

    // First, tell all the transient children windows that they are going to be destroyed too
    QHash<unsigned int, CompositorWindow*>::iterator i = mWindows.begin();
    while (i != mWindows.end()) {
        CompositorWindow *registeredWindow = i.value();
        ++i;
        if( registeredWindow ) {
            QWaylandSurface *registeredSurface = registeredWindow->surface();
            if( registeredSurface && registeredSurface->transientParent() == surface ) {
                qWarning() << Q_FUNC_INFO << "the surface " << registeredSurface << " is a child of " << surface << "and will be destroyed too";

                surfaceAboutToBeDestroyed(registeredSurface);

                // The mWindows hash table has been modified, so restart from the beginning.
                // This should be improved !
                i = mWindows.begin();
            }
        }
    }

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

    if (window)
        emit windowRaised(QVariant::fromValue(static_cast<QQuickItem*>(window)));
}

void Compositor::surfaceLowered()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface*>(sender());
    CompositorWindow *window = static_cast<CompositorWindow*>(surface->surfaceItem());

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
