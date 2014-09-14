/***************************************************************************
**
** Copyright (C) 2012 Jolla Ltd.
** Contact: Robin Burchell <robin.burchell@jollamobile.com>
**
** This file is taken from lipstick (https://github.com/nemomobile/lipstick)
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QCoreApplication>
#include <QDebug>
#include "notificationmanager.h"

// Define this if you'd like to see debug messages from the notification manager
#ifdef DEBUG_NOTIFICATIONS
#define NOTIFICATIONS_DEBUG(things) qDebug() << Q_FUNC_INFO << things
#else
#define NOTIFICATIONS_DEBUG(things)
#endif

NotificationManager *NotificationManager::instance_ = 0;

NotificationManager *NotificationManager::instance()
{
    if (instance_ == 0) {
        instance_ = new NotificationManager(qApp);
    }
    return instance_;
}

NotificationManager::NotificationManager(QObject *parent) :
    QObject(parent),
    previousNotificationID(0)
{
    qDebug() << __PRETTY_FUNCTION__;
}

NotificationManager::~NotificationManager()
{
    foreach(uint id, notifications.keys()) {
        removeNotification(id);
    }
}

Notification *NotificationManager::notification(uint id) const
{
    return notifications.value(id);
}

QList<uint> NotificationManager::notificationIds() const
{
    return notifications.keys();
}

uint NotificationManager::GetIdForNotification(Notification *notification) const
{
    return notifications.key(notification, 0);
}

uint NotificationManager::Notify(const QString &ownerId, uint replacesId, const QString &launchId, const QString &launchParam,
                                 const QString &title, const QString &body, const QUrl &iconUrl,
                                 int priority, int expireTimeout)
{
    uint id = replacesId != 0 ? replacesId : nextAvailableNotificationID();

    Notification *notificationToReplace = notifications.value(id);

    if (replacesId == 0 || (notificationToReplace && notificationToReplace->ownerId() == ownerId)) {
        if (replacesId == 0) {
            // Create a new notification
            Notification *notification = new Notification(ownerId, id, launchId, launchParam, title, body, iconUrl, priority, expireTimeout, this);
            connect(notification, SIGNAL(launchInvoked()), this, SLOT(launchNotification()), Qt::QueuedConnection);
            connect(notification, SIGNAL(removeRequested()), this, SLOT(removeNotification()), Qt::QueuedConnection);
            notifications.insert(id, notification);
        }
        else {
            // Only replace an existing notification if it really exists and ownerId is the same
            notificationToReplace->setLaunchId(launchId);
            notificationToReplace->setLaunchParam(launchParam);
            notificationToReplace->setTitle(title);
            notificationToReplace->setBody(body);
            notificationToReplace->setIconUrl(iconUrl);
            notificationToReplace->setPriority(priority);
            notificationToReplace->setExpireTimeout(expireTimeout);
            notificationToReplace->resetTimeStamp();
        }

        NOTIFICATIONS_DEBUG("NOTIFY:" << ownerId << iconUrl << title << body << expireTimeout << "->" << id);
        emit notificationModified(id);
    } else {
        // Return the ID 0 when trying to update a notification which doesn't exist
        id = 0;
    }

    return id;
}

void NotificationManager::CloseNotification(uint id, NotificationClosedReason closeReason)
{
    if (notifications.contains(id)) {
        emit notificationClosed(id, closeReason);

        NOTIFICATIONS_DEBUG("REMOVE:" << id);
        emit notificationRemoved(id);

        // Mark the notification to be destroyed
        removedNotifications.insert(notifications.take(id));

        // Delete it right away ?
        qDeleteAll(removedNotifications);
        removedNotifications.clear();
    }
}

NotificationList NotificationManager::GetNotifications(const QString &ownerId)
{
    QList<Notification *> notificationList;
    foreach (uint id, notifications.keys()) {
        Notification *notification = notifications.value(id);
        if (notification->ownerId() == ownerId) {
            notificationList.append(notification);
        }
    }

    return NotificationList(notificationList);
}

uint NotificationManager::nextAvailableNotificationID()
{
    bool idIncreased = false;

    // Try to find an unused ID. Increase the ID at least once but only up to 2^32-1 times.
    for (uint i = 0; i < UINT32_MAX && (!idIncreased || notifications.contains(previousNotificationID)); i++, idIncreased = true) {
        previousNotificationID++;

        if (previousNotificationID == 0) {
            // 0 is not a valid ID so skip it
            previousNotificationID = 1;
        }
    }

    return previousNotificationID;
}

void NotificationManager::launchNotification()
{
    Notification *notification = qobject_cast<Notification *>(sender());
    if (notification != 0) {
        uint id = notifications.key(notification, 0);
        if (id > 0) {
            emit launchInvoked(id);

            removeNotification(id);
        }
    }
}

void NotificationManager::removeNotification(uint id)
{
    if (id == 0) {
        Notification *notification = qobject_cast<Notification *>(sender());
        if (notification != 0) {
            id = notifications.key(notification, 0);
        }
    }

    CloseNotification(id, NotificationDismissedByUser);
}

void NotificationManager::removeNotifications(const QString &ownerId)
{
    foreach(uint id, notifications.keys()) {
        Notification *notification = notifications.value(id);
        if (notification->ownerId() == ownerId) {
            removeNotification(id);
        }
    }
}
