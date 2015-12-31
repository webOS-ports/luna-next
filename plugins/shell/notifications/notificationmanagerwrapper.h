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
#include <QUrl>

#include "notificationmanager.h"

class NotificationManagerWrapper : public QObject
{
    Q_OBJECT

public:
    NotificationManagerWrapper();

    Q_INVOKABLE Notification* getNotificationById(uint id);
    Q_INVOKABLE uint notify(const QString &ownerId, uint replacesId, const QString &launchId, const QString &launchParams,
                            const QString &title, const QString &body, const QUrl &iconUrl, const QString &soundClass, const QUrl &soundFile,
                            int duration, bool doNotSuppress, int priority, int expireTimeout);
    Q_INVOKABLE void closeById(uint id, NotificationManager::NotificationClosedReason reason = NotificationManager::CloseNotificationCalled);
    Q_INVOKABLE void closeAllByOwner(const QString& ownerId);
};

#endif // NOTIFICATIONMANAGERWRAPPER_H
