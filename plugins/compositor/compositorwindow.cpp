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
#include <QWaylandCompositor>
#include <QWaylandInputDevice>
#include <QTimer>

#include "compositorwindow.h"
#include "windowtype.h"

namespace luna
{

CompositorWindow::CompositorWindow(unsigned int winId, QWaylandQuickSurface *surface, QQuickItem *parent)
    : QWaylandSurfaceItem(surface, parent),
      mId(winId),
      mParentWinId(0),
      mWindowType(WindowType::Card),
      mClosed(false),
      mRemovePosted(false),
      mReady(false)
{
    QVariantMap properties = surface->windowProperties();
    QMapIterator<QString,QVariant> iter(properties);
    while (iter.hasNext()) {
        iter.next();
        onWindowPropertyChanged(iter.key(), iter.value());
    }

    connect(surface, SIGNAL(windowPropertyChanged(const QString&,const QVariant&)), this, SLOT(onWindowPropertyChanged(const QString&, const QVariant&)));
    connect(this, &QWaylandSurfaceItem::surfaceDestroyed, this, &QObject::deleteLater);

    QTimer::singleShot(0, this, SLOT(sendWindowIdToClient()));

    qDebug() << Q_FUNC_INFO << "id" << mId << "type" << mWindowType << "appId" << mAppId;
}

CompositorWindow::~CompositorWindow()
{
    qDebug() << Q_FUNC_INFO << "id" << mId << "type" << mWindowType << "appId" << mAppId;
}

void CompositorWindow::sendWindowIdToClient()
{
    surface()->setWindowProperty("windowId", QVariant(mId));
}

void CompositorWindow::checkStatus()
{
    if (mReady)
        return;

    if (mAppId.length() > 0) {
        mReady = true;
        emit readyChanged();
    }
}

void CompositorWindow::onWindowPropertyChanged(const QString &name, const QVariant &value)
{
    if (name == "appId") {
        mAppId = value.toString();

        if (mAppId == "com.palm.launcher")
            mWindowType = WindowType::Launcher;
    }
    else if (name == "type")
        mWindowType = WindowType::fromString(value.toString());
    else if (name == "parentWindowId") {
        mParentWinId = value.toInt();
        parentWinIdChanged();
    }

    checkStatus();
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
        return surface()->processId();
    return 0;
}

QVariant CompositorWindow::userData() const
{
    return mUserData;
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

    QFile procExeEntry(QString("/proc/%0/exe").arg(surface()->processId()));
    if (!procExeEntry.exists())
        return false;

    // FIXME this should be moved to a configuration file so we don't have to touch the
    // source code for changing the list of allowed processes
    return (procExeEntry.symLinkTarget() == "/usr/sbin/webapp-launcher" ||
            procExeEntry.symLinkTarget() == "/usr/sbin/LunaWebAppManager" ||
            procExeEntry.symLinkTarget() == "/usr/bin/maliit-server" ||
            procExeEntry.symLinkTarget() == "/usr/palm/applications/org.webosports.app.phone/org.webosports.app.phone");
}

void CompositorWindow::setClosed(bool closed)
{
    mClosed = closed;
}

void CompositorWindow::tryRemove()
{
    if (mClosed && !mRemovePosted) {
        mRemovePosted = true;
        QCoreApplication::postEvent(this, new QEvent(QEvent::User));
    }
}

bool CompositorWindow::event(QEvent *event)
{
    bool handled = QWaylandSurfaceItem::event(event);

    if (event->type() == QEvent::User) {
        mRemovePosted = false;
        delete this;
    }

    return handled;
}

void CompositorWindow::postEvent(int event)
{
    int key = EventType::toKey(static_cast<EventType::Event>(event));
    if (key > 0) {
        QWaylandInputDevice *inputDevice = surface()->compositor()->defaultInputDevice();

        QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, key, Qt::NoModifier);
        inputDevice->sendFullKeyEvent(surface(), keyEvent);

        keyEvent = new QKeyEvent(QEvent::KeyRelease, key, Qt::NoModifier);
        inputDevice->sendFullKeyEvent(surface(), keyEvent);
    }
}

void CompositorWindow::changeSize(const QSize& size)
{
    surface()->requestSize(size);
}

void CompositorWindow::setParentWinId(unsigned int id)
{
    mParentWinId = id;
    if (surface())
        surface()->setWindowProperty("parentWindowId", id);
    parentWinIdChanged();
}

} // namespace luna
