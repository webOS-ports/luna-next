/***************************************************************************
**
** Copyright (C) 2012 Jolla Ltd.
** Contact: Robin Burchell <robin.burchell@jollamobile.com>
**
** This file is part of lipstick.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QTimer>

#include "notificationmanager.h"
#include "notificationlistmodel.h"

NotificationListModel::NotificationListModel(QObject *parent) :
    QObjectListModel(parent)
{
    connect(NotificationManager::instance(), SIGNAL(notificationModified(uint)), this, SLOT(updateNotification(uint)));
    connect(NotificationManager::instance(), SIGNAL(notificationRemoved(uint)), this, SLOT(removeNotification(uint)));

    QTimer::singleShot(0, this, SLOT(init()));
}

NotificationListModel::~NotificationListModel()
{
}

void NotificationListModel::init()
{
    foreach(uint id, NotificationManager::instance()->notificationIds()) {
        updateNotification(id);
    }
}

void NotificationListModel::updateNotification(uint id)
{
    Notification *notification = NotificationManager::instance()->notification(id);

    if (notification != 0) {
        int index = indexOf(notification);
        if (notificationShouldBeShown(notification)) {
            // Place the notifications in the model latest first, moving existing notifications if necessary
            int expectedIndex = indexFor(notification);
            if (index < 0) {
                insertItem(expectedIndex, notification);
            } else if (index != expectedIndex) {
                move(index, expectedIndex);
            }
        } else if (index >= 0) {
            removeItem(notification);
        }
    }
}

int NotificationListModel::indexFor(Notification *notification)
{
    for (int index = 0; index < itemCount(); index++) {
        Notification *notificationAtIndex = static_cast<Notification *>(get(index));
        if (notificationAtIndex->timestamp() <= notification->timestamp()) {
            return index;
        }
    }
    return itemCount();
}

void NotificationListModel::removeNotification(uint id)
{
    removeItem(NotificationManager::instance()->notification(id));
}

bool NotificationListModel::notificationShouldBeShown(Notification *notification)
{
    return !(notification->body().isEmpty() && notification->title().isEmpty()) && notification->priority() >= 0;
}
