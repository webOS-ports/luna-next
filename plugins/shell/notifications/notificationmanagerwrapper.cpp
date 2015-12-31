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

#include "notificationmanagerwrapper.h"
#include "notificationmanager.h"

NotificationManagerWrapper::NotificationManagerWrapper()
{
    NotificationManager::instance();
}

Notification* NotificationManagerWrapper::getNotificationById(uint id)
{
    return NotificationManager::instance()->notification(id);
}

uint NotificationManagerWrapper::notify(const QString &ownerId, uint replacesId, const QString &launchId, const QString &launchParams,
                                        const QString &title, const QString &body, const QUrl &iconUrl, const QString &soundClass,
                                        const QUrl &soundFile, int duration, bool doNotSuppress, int priority, int expireTimeout)
{
    return NotificationManager::instance()->Notify(ownerId, replacesId, launchId, launchParams, title, body, iconUrl, soundClass, soundFile, duration, doNotSuppress, priority, expireTimeout);
}

void NotificationManagerWrapper::closeById(uint id, NotificationManager::NotificationClosedReason reason)
{
    return NotificationManager::instance()->CloseNotification(id, reason);
}

void NotificationManagerWrapper::closeAllByOwner(const QString &ownerId)
{
    NotificationManager *manager = NotificationManager::instance();
    NotificationList notificationList = manager->GetNotifications(ownerId);
    Q_FOREACH(Notification *notification, notificationList.notifications()) {
        uint id = manager->GetIdForNotification(notification);
        // zero is an invalid notification id so ignore it
        if (id == 0)
            continue;
        manager->CloseNotification(id);
    }
}
