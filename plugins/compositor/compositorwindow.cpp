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

#include <QCoreApplication>
#include <QTimer>

#include <QWaylandCompositor>
#include <QWaylandSeat>
#include <QWaylandXdgSurface>
#include <QWaylandXdgToplevel>

#include <QtWaylandCompositor/private/qwlextendedsurface_p.h>

#include "compositorwindow.h"
#include "windowtype.h"

namespace luna
{

CompositorWindow::CompositorWindow(unsigned int winId, QQuickItem *parent)
    : QWaylandQuickShellSurfaceItem(parent),
      mId(winId),
      mParentWinId(0),
      mParentWinIdSet(false),
      mWindowType(WindowType::Card),
      mWindowTypeSet(false),
      mReady(false),
      mKeepAlive(false),
      mLoadingAnimationDisabled(false)
{
    QTimer::singleShot(2000, this, SLOT(onReadyTimeout()));

    qDebug() << Q_FUNC_INFO << "id" << mId << "type" << mWindowType << "appId" << mAppId;
}

CompositorWindow::~CompositorWindow()
{
    qDebug() << Q_FUNC_INFO << "id" << mId << "type" << mWindowType << "appId" << mAppId;
}

void CompositorWindow::initialize(QWaylandXdgSurface *shellSurface)
{
    setShellSurface(shellSurface);

    QWaylandSurface *surface = shellSurface->surface();
    if(surface) {
        setSurface(surface);

        QtWayland::ExtendedSurface *extSurface = static_cast<QtWayland::ExtendedSurface*>(surface->extension(QtWayland::ExtendedSurface::interfaceName()));
        if (extSurface)
        {
            QVariantMap properties = extSurface->windowProperties();
            QMapIterator<QString,QVariant> iter(properties);
            while (iter.hasNext()) {
                iter.next();
                onWindowPropertyChanged(iter.key(), iter.value());
            }

            connect(extSurface, &QtWayland::ExtendedSurface::windowPropertyChanged, this, &CompositorWindow::onWindowPropertyChanged);
        }
    }

    connect(this, &QWaylandQuickItem::surfaceDestroyed, this, &QObject::deleteLater);
    connect(surface, &QWaylandSurface::hasContentChanged, this, &CompositorWindow::onSurfaceMappedChanged);
    connect(shellSurface, &QWaylandShellSurface::windowTypeChanged, this, &CompositorWindow::onWindowTypeChanged);
}

void CompositorWindow::forceVisible()
{
    QtWayland::ExtendedSurface *pExtendedSurfaceExt = static_cast<QtWayland::ExtendedSurface*>(surface()->extension(QtWayland::ExtendedSurface::interfaceName()));
    if(pExtendedSurfaceExt) {
        pExtendedSurfaceExt->sendOnScreenVisibilityChange(true);
    }
}

void CompositorWindow::sendWindowIdToClient()
{
    QtWayland::ExtendedSurface *pExtendedSurfaceExt = static_cast<QtWayland::ExtendedSurface*>(surface()->extension(QtWayland::ExtendedSurface::interfaceName()));
    if(pExtendedSurfaceExt) {
        pExtendedSurfaceExt->setWindowProperty("_LUNE_WINDOW_ID", QVariant(mId));
    }
}

void CompositorWindow::sendClose()
{
    QtWayland::ExtendedSurface *pExtendedSurfaceExt = static_cast<QtWayland::ExtendedSurface*>(surface()->extension(QtWayland::ExtendedSurface::interfaceName()));
    if(pExtendedSurfaceExt) {
        pExtendedSurfaceExt->send_close();
    }
}

bool CompositorWindow::isPopup()
{
    QWaylandShellSurface *pShellSurfaceExt = shellSurface();
    return Qt::Popup == pShellSurfaceExt->windowType();
}

void CompositorWindow::checkStatus()
{
    if (mReady)
        return;

    if (mAppId.length() > 0 && mParentWinIdSet && mWindowTypeSet) {
        mReady = true;
        emit readyChanged();
    }
}

void CompositorWindow::onReadyTimeout()
{
    if (mReady)
        return;

    mReady = true;
    mParentWinIdSet = true;
    mWindowTypeSet = true;

    emit readyChanged();
}

void CompositorWindow::onWindowTypeChanged()
{
    QWaylandShellSurface *pShellSurfaceExt = shellSurface();
    Qt::WindowType type = pShellSurfaceExt->windowType();

    qDebug() << Q_FUNC_INFO << type;

    // if it's not too late, adjust the LuneOS window type to overlay
    if(type == Qt::Popup && !mReady) {
        mWindowType = WindowType::Overlay;
        mWindowTypeSet = true;
    }
}

void CompositorWindow::onWindowPropertyChanged(const QString &name, const QVariant &value)
{
    qDebug() << Q_FUNC_INFO << name << value;

    if (name == "_LUNE_APP_ID")
        mAppId = value.toString();
    else if (name == "_LUNE_APP_ICON")
        mAppIcon = value.toString();
    else if (name == "_LUNE_APP_KEEP_ALIVE")
        mKeepAlive = value.toBool();
    else if (name == "_LUNE_WINDOW_TYPE")
    {
        mWindowType = WindowType::fromString(value.toString());
        if(mWindowType == WindowType::Overlay) setFocusOnClick(false);
        mWindowTypeSet = true;
    }
    else if (name == "_LUNE_WINDOW_PARENT_ID") {
        mParentWinId = value.toInt();
        mParentWinIdSet = true;
        parentWinIdChanged();
    }
    else if (name == "_LUNE_WINDOW_LOADING_ANIMATION_DISABLED") {
        mLoadingAnimationDisabled = value.toBool();
        loadingAnimationDisabledChanged();
    }

    checkStatus();
}

void CompositorWindow::onSurfaceMappedChanged()
{
    if (surface())
        qDebug() << Q_FUNC_INFO << "id" << mId << "mapped" << surface()->hasContent();

    emit mappedChanged();
}

unsigned int CompositorWindow::winId() const
{
    return mId;
}

unsigned int CompositorWindow::parentWinId() const
{
    return mParentWinId;
}

unsigned int CompositorWindow::windowType() const
{
    return mWindowType;
}

QString CompositorWindow::appId() const
{
    return mAppId;
}

quint64 CompositorWindow::processId() const
{
    if (surface())
        return surface()->client()->processId();
    return 0;
}

bool CompositorWindow::ready() const
{
    return mReady;
}

QVariant CompositorWindow::userData() const
{
    return mUserData;
}

bool CompositorWindow::keepAlive() const
{
    return mKeepAlive;
}

void CompositorWindow::setUserData(QVariant data)
{
    if (mUserData == data)
        return;

    mUserData = data;
    emit userDataChanged();
}

bool CompositorWindow::checkIsAllowedToStay()
{
    if (!surface())
        return false;

    QFile procExeEntry(QString("/proc/%0/exe").arg(surface()->client()->processId()));
    if (!procExeEntry.exists())
        return false;

    // FIXME this should be moved to a configuration file so we don't have to touch the
    // source code for changing the list of allowed processes
    return (procExeEntry.symLinkTarget() == "/usr/sbin/LunaWebAppManager" ||
            procExeEntry.symLinkTarget() == "/usr/bin/maliit-server");
}

void CompositorWindow::postEvent(int event)
{
    int key = EventType::toKey(static_cast<EventType::Event>(event));
    if (key > 0) {
        QWaylandSeat *defaultSeat = surface()->compositor()->defaultSeat();

        QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier, key, 0, 0);
        defaultSeat->sendFullKeyEvent(keyEvent);

        keyEvent = new QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier, key, 0, 0);
        defaultSeat->sendFullKeyEvent(keyEvent);
    }
}

void CompositorWindow::changeSize(const QSize& size)
{
    QWaylandXdgSurface *pXdgShellSurfaceExt = static_cast<QWaylandXdgSurface*>(shellSurface());
    QWaylandXdgToplevel *pXdgTopLevel = NULL;
    if(pXdgShellSurfaceExt) pXdgTopLevel = pXdgShellSurfaceExt->toplevel();
    
    if(pXdgTopLevel)
    {
        QVector<QWaylandXdgToplevel::State> states;
        if(hasFocus()) states += QWaylandXdgToplevel::ActivatedState;
        pXdgTopLevel->sendConfigure(size, states);
    }
}

void CompositorWindow::setParentWinId(unsigned int id)
{
    mParentWinId = id;

    QtWayland::ExtendedSurface *pExtendedSurfaceExt = static_cast<QtWayland::ExtendedSurface*>(surface()->extension(QtWayland::ExtendedSurface::interfaceName()));
    if(pExtendedSurfaceExt) {
        pExtendedSurfaceExt->setWindowProperty("parentWindowId", id);
    }

    parentWinIdChanged();
}

bool CompositorWindow::mapped() const
{
    if (!surface())
        return false;

    return surface()->hasContent();
}

QString CompositorWindow::appIcon() const
{
    return mAppIcon;
}

bool CompositorWindow::loadingAnimationDisabled() const
{
    return mLoadingAnimationDisabled;
}

QVariantMap CompositorWindow::windowPropertyMap() const
{
    QtWayland::ExtendedSurface *pExtendedSurfaceExt = static_cast<QtWayland::ExtendedSurface*>(surface()->extension(QtWayland::ExtendedSurface::interfaceName()));
    if(pExtendedSurfaceExt)
        return pExtendedSurfaceExt->windowProperties();
    else
        return QVariantMap();
}

} // namespace luna
