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
#include "windowtype.h"
#include "recorder.h"

#include <QtCore/QtGlobal>

#include <QWaylandSeat>
#include <QWaylandOutput>
#include <QWaylandXdgShell>
#include <QWaylandXdgSurface>
#include <QWaylandXdgToplevel>

#include "QtWaylandCompositor/private/qwlqtkey_p.h"
#include <QtWaylandCompositor/QWaylandXdgDecorationManagerV1>

namespace luna
{

static CompositorWindow *surfaceWindow(QWaylandSurface *surface)
{
    return surface->primaryView() ? static_cast<CompositorWindow*>(surface->primaryView()->renderObject()) : 0;
}

Compositor* Compositor::mInstance = 0;

Compositor::Compositor()
    : mFullscreenWindow(0),
      mNextWindowId(1),
      mRecorderCounter(0)
{
    if (mInstance)
        qFatal("Compositor: Only one compositor instance per process is supported");

    qDebug() << __PRETTY_FUNCTION__;

    mInstance = this;
}

Compositor::~Compositor()
{
    delete mSurfaceExtension; mSurfaceExtension = nullptr;
    delete mRecorder; mRecorder = nullptr;
}

Compositor* Compositor::instance()
{
    return mInstance;
}

void Compositor::create()
{
    setRetainedSelectionEnabled(true);

    QWaylandCompositor::create();

    // Add a valid mode for the output
    QWaylandOutput *output = defaultOutput();
    QWaylandOutputMode defaultMode(QSize(output->window()->size()), 25000);
    output->addMode(defaultMode, true);
    output->setCurrentMode(defaultMode);

    QWaylandXdgShell *xdgShell = new QWaylandXdgShell(this);
    //xdgShell->setFocusPolicy(QWaylandShell::ManualFocus);
//    connect(xdgShell, &QWaylandXdgShell::xdgSurfaceCreated, this, &Compositor::onXdgSurfaceCreated);
    connect(xdgShell, &QWaylandXdgShell::toplevelCreated, this, &Compositor::onXdgToplevelCreated);

    connect(this, &QWaylandCompositor::surfaceCreated, this, &Compositor::onSurfaceCreated);

    QQuickWindow *defaultOutputWindow = static_cast<QQuickWindow*>(defaultOutput()->window());
    connect(defaultOutputWindow, &QQuickWindow::afterRendering, this, &Compositor::readContent, Qt::DirectConnection);

    mSurfaceExtension = new QtWayland::SurfaceExtensionGlobal(this);
    connect(mSurfaceExtension, &QtWayland::SurfaceExtensionGlobal::extendedSurfaceReady, this, &Compositor::onExtendedSurfaceReady);

    QtWayland::QtKeyExtensionGlobal *pKeyExtension = new QtWayland::QtKeyExtensionGlobal(this);

    QWaylandXdgDecorationManagerV1 *mXdgDecorationManagerV1 = new QWaylandXdgDecorationManagerV1();
    mXdgDecorationManagerV1->setExtensionContainer(this);
    mXdgDecorationManagerV1->initialize();
    mXdgDecorationManagerV1->setPreferredMode(QWaylandXdgToplevel::ServerSideDecoration);

    mRecorder = new RecorderManager(this);
}

void Compositor::readContent()
{
    mRecorder->recordFrame(defaultOutput()->window());
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

void Compositor::setFullscreenWindow(CompositorWindow *window)
{
    if (window == mFullscreenWindow)
        return;

    qDebug() << Q_FUNC_INFO << window;

    // Prevent flicker when returning to composited mode
    if (!window && mFullscreenWindow) {
        mFullscreenWindow->update();
    }

    mFullscreenWindow = window;

    emit fullscreenWindowChanged();
}

void Compositor::clearKeyboardFocus()
{
    defaultSeat()->setKeyboardFocus(0);
}

bool Compositor::hasProcessMultipleWindows(QWaylandClient* client)
{
    unsigned int count = 0;

    QList<QWaylandSurface *> surfaces = surfacesForClient(client);
    for (QWaylandSurface *surface : surfaces) {
        if (surfaceWindow(surface))
            count++;
    }

    return count > 1;
}

void Compositor::closeWindowWithId(int winId)
{
    qDebug() << Q_FUNC_INFO << "winId" << winId;

    CompositorWindow *window = mWindows.value(winId, 0);
    if (window && window->surface()) {
        QWaylandSurface *surface = window->surface();

        if (window->checkIsAllowedToStay() ||
            hasProcessMultipleWindows(surface->client()) ||
            window->keepAlive()) {
            qDebug() << Q_FUNC_INFO << "Destroying surface and keeping client alive";
            window->sendClose();
        }
        else {
            qDebug() << Q_FUNC_INFO << "Closing client and destroying surface";
            destroyClientForSurface(surface);
        }
    }
}

void Compositor::windowIsReady()
{
    CompositorWindow *window = static_cast<CompositorWindow*>(sender());
    QWaylandXdgSurface *pShellSurfaceExt = static_cast<QWaylandXdgSurface*>(window->shellSurface());

    QWaylandXdgToplevel *pXdgTopLevel = NULL;
    if(pShellSurfaceExt) pXdgTopLevel = pShellSurfaceExt->toplevel();

    // Windows created by QtWebProcess are not meant to be shown to the user
    // They are mainly temporary windows used for offscreen drawing.
    // Therefore, as long as they are hidden, don't create a card for them.
    if (pXdgTopLevel->appId() != "QtWebProcess" && pXdgTopLevel->appId() != "QtWebEngineProcess") {
        if (!WindowModel::isWindowAlreadyAdded(mWindowModels, window)) {
            qDebug() << Q_FUNC_INFO << "Adding window" << window << "to our models";
            emit windowAdded(QVariant::fromValue(static_cast<QQuickItem*>(window)));
            WindowModel::addWindowForEachModel(mWindowModels, window);
        }
    }
}

void Compositor::onSurfaceCreated(QWaylandSurface *surface)
{
    qDebug() << __PRETTY_FUNCTION__ << "surface" << surface;

//    CompositorWindow *window = createWindowForSurface(surface);

    connect(surface, &QWaylandSurface::hasContentChanged, this, &Compositor::onSurfaceMappedChanged);
    //connect(surface, &QWaylandSurface::sizeChanged, this, &Compositor::onSurfaceSizeChanged);
    //connect(surface, &QWaylandSurface::damaged, this, &Compositor::onSurfaceDamaged);
}

void Compositor::onSurfaceMappedChanged()
{
    QWaylandSurface *surface = qobject_cast<QWaylandSurface *>(sender());
    CompositorWindow *window = surfaceWindow(surface);
    if(window && surface) {
        if(surface->hasContent()) {
            qDebug() << __PRETTY_FUNCTION__ << " MAPPED " << window << "appId" << window->appId() << "windowType" << window->windowType();

            // If it was a window created by QtWebProcess, it may be not already in our WindowModel list
            if (!WindowModel::isWindowAlreadyAdded(mWindowModels, window)) {
                qDebug() << Q_FUNC_INFO << "Adding window" << window << "to our models";
                emit windowAdded(QVariant::fromValue(static_cast<QQuickItem*>(window)));
                WindowModel::addWindowForEachModel(mWindowModels, window);
            }
            emit windowShown(QVariant::fromValue(static_cast<QQuickItem*>(window)));
        }
        else {
            qDebug() << __PRETTY_FUNCTION__ << " UNMAPPED " << window << "appId" << window->appId() << "windowType" << window->windowType();

            if (window == mFullscreenWindow)
                setFullscreenWindow(0);

            emit windowHidden(QVariant::fromValue(static_cast<QQuickItem*>(window)));
        }
    }
}

void Compositor::onSurfaceDestroyed()
{
    CompositorWindow *window = static_cast<CompositorWindow*>(sender());

    qDebug() << Q_FUNC_INFO << window;

    if (window == mFullscreenWindow)
        setFullscreenWindow(0);

    if (window) {
        if (WindowModel::isWindowAlreadyAdded(mWindowModels, window)) {
            WindowModel::removeWindowForEachModel(mWindowModels, window);
            emit windowRemoved(QVariant::fromValue(static_cast<QQuickItem*>(window)));
        }

        mWindows.remove(window->winId());

        // window->deleteLater(); // not needed: the deletion of the surface will trigger the deletion of its extensions, among which is our CompositorWindow
    }
}

void Compositor::onSurfaceRaised()
{
    QtWayland::ExtendedSurface *extSurface = qobject_cast<QtWayland::ExtendedSurface*>(sender());
    QWaylandSurface *surface = qobject_cast<QWaylandSurface*>(extSurface->extensionContainer());
    CompositorWindow *window = surfaceWindow(surface);

    qWarning() << Q_FUNC_INFO << "the window " << window << "is going to be raised";

    if (window)
        emit windowRaised(QVariant::fromValue(static_cast<QQuickItem*>(window)));
}

void Compositor::onSurfaceLowered()
{
    QtWayland::ExtendedSurface *extSurface = qobject_cast<QtWayland::ExtendedSurface*>(sender());
    QWaylandSurface *surface = qobject_cast<QWaylandSurface*>(extSurface->extensionContainer());
    CompositorWindow *window = surfaceWindow(surface);

    qWarning() << Q_FUNC_INFO << "the window " << window << "is going to be lowered";

    if (window)
        emit windowLowered(QVariant::fromValue(static_cast<QQuickItem*>(window)));
}

void Compositor::onSurfaceDamaged(const QRegion &)
{
    /*
    if (!isVisible())
        sendFrameCallbacks(surfaces());
    */
}

void Compositor::onXdgToplevelCreated(QWaylandXdgToplevel *toplevel, QWaylandXdgSurface *shellSurface)
//void Compositor::onXdgSurfaceCreated(QWaylandXdgSurface *shellSurface)
{
    unsigned int windowId = mNextWindowId++;
    QWaylandSurface *surface = shellSurface->surface();

    qDebug() << Q_FUNC_INFO << "windowId" << windowId << surface;

    QQuickWindow *defaultOutputWindow = static_cast<QQuickWindow*>(defaultOutput()->window());
    CompositorWindow *window = new CompositorWindow(windowId, defaultOutputWindow->contentItem());

    window->setOutput(defaultOutput()); //useful ?
    window->setFlag(QQuickItem::ItemIsFocusScope, true);
    // window->setUseTextureAlpha(true);

    window->initialize(shellSurface);
    window->setSize(surface->size());
    window->setTouchEventsEnabled(true);

    mWindows.insert(windowId, window);

    connect(window, &CompositorWindow::readyChanged, this, &Compositor::windowIsReady);
    connect(window, &QWaylandQuickItem::surfaceDestroyed, this, &Compositor::onSurfaceDestroyed);

    window->sendWindowIdToClient();
}

void Compositor::onExtendedSurfaceReady(QtWayland::ExtendedSurface *extSurface, QWaylandSurface *surface)
{
    extSurface->initialize();
}

void Compositor::setRecording(bool value)
{
    unsigned int before = recording();

    if (!value && mRecorderCounter > 0)
        mRecorderCounter--;
    else
        mRecorderCounter++;

    if (before != recording())
        recordingChanged();
}

} // namespace luna
