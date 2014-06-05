/***************************************************************************
**
** Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef NOTIFICATIONMANAGERWRAPPER_H
#define NOTIFICATIONMANAGERWRAPPER_H

#include <QObject>
#include <QString>

#include "notificationmanager.h"

class NotificationManagerWrapper : public QObject
{
    Q_OBJECT

public:
    NotificationManagerWrapper();

    Q_INVOKABLE Notification* getNotificationById(uint id);
    Q_INVOKABLE uint notify(const QString &appName, uint replacesId, const QString &appIcon, const QString &summary, const QString &body, const QStringList &actions, const QVariantHash &hints, int expireTimeout);
    Q_INVOKABLE void closeById(uint id, NotificationManager::NotificationClosedReason reason = NotificationManager::CloseNotificationCalled);
};

#endif // NOTIFICATIONMANAGERWRAPPER_H
