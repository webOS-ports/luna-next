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

#ifndef LUNA_COMPOSITORWINDOW_H_
#define LUNA_COMPOSITORWINDOW_H_

#include <QWaylandSurface>
#include <QWaylandQuickItem>
#include <QWaylandQuickShellSurfaceItem>
#include <QWaylandShellSurface>

#include "eventtype.h"
#include "windowtype.h"

class QWaylandXdgSurface;

namespace luna
{

class CompositorWindow : public QWaylandQuickShellSurfaceItem
{
    Q_OBJECT
    Q_PROPERTY(int winId READ winId CONSTANT)
    Q_PROPERTY(int parentWinId READ parentWinId WRITE setParentWinId NOTIFY parentWinIdChanged)
    Q_PROPERTY(int windowType READ windowType CONSTANT)
    Q_PROPERTY(QString appId READ appId CONSTANT)
    Q_PROPERTY(quint64 processId READ processId CONSTANT)
    Q_PROPERTY(QVariant userData READ userData WRITE setUserData NOTIFY userDataChanged)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool mapped READ mapped NOTIFY mappedChanged)
    Q_PROPERTY(QString appIcon READ appIcon CONSTANT)
    Q_PROPERTY(bool loadingAnimationDisabled READ loadingAnimationDisabled NOTIFY loadingAnimationDisabledChanged)
    Q_PROPERTY(QVariantMap windowProperties READ windowPropertyMap CONSTANT)
    Q_PROPERTY(bool isPopup READ isPopup CONSTANT)

public:
    CompositorWindow(unsigned int winId, QQuickItem *parent = 0);
    virtual ~CompositorWindow();

    void initialize(QWaylandXdgSurface *shellSurface);

    unsigned int winId() const;
    unsigned int parentWinId() const;
    unsigned int windowType() const;
    QString appId() const;
    quint64 processId() const;
    bool ready() const;
    bool mapped() const;
    QString appIcon() const;
    bool loadingAnimationDisabled() const;

    QVariant userData() const;
    void setUserData(QVariant);

    bool keepAlive() const;

    void setParentWinId(unsigned int id);

    bool checkIsAllowedToStay();

    bool isPopup();

    Q_INVOKABLE void postEvent(int event);
    Q_INVOKABLE void changeSize(const QSize& size);
    Q_INVOKABLE void forceVisible();

    QVariantMap windowPropertyMap() const;

public slots:
    void sendWindowIdToClient();
    void sendClose();
    void onWindowTypeChanged();
    void onWindowPropertyChanged(const QString&, const QVariant&);

signals:
    void userDataChanged();
    void readyChanged();
    void parentWinIdChanged();
    void mappedChanged();
    void loadingAnimationDisabledChanged();

private slots:
    void onSurfaceMappedChanged();
    void onReadyTimeout();

private:
    unsigned int mId;
    unsigned int mParentWinId;
    bool mParentWinIdSet;
    unsigned int mWindowType;
    bool mWindowTypeSet;
    QString mAppId;
    QVariant mUserData;
    bool mReady;
    QString mAppIcon;
    bool mKeepAlive;
    bool mLoadingAnimationDisabled;

    void checkStatus();
};

} // namespace luna

#endif
