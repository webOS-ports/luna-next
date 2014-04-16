/***************************************************************************
**
** Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
**
** This file is part of luna-next.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QtQml>

#include "plugin.h"
#include "notification.h"
#include "notificationlistmodel.h"
#include "notificationmanager.h"
#include "notificationmanagerwrapper.h"

LunaNextShellNotificationsPlugin::LunaNextShellNotificationsPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void LunaNextShellNotificationsPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("LunaNext.Shell.Notifications"));

    qmlRegisterType<NotificationListModel>(uri, 0, 1, "NotificationListModel");
    qmlRegisterType<Notification>(uri, 0, 1, "Notification");
    qmlRegisterType<NotificationManagerWrapper>(uri, 0, 1, "NotificationManager");
}

void LunaNextShellNotificationsPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
}
