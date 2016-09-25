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

#ifndef NOTIFICATIONLISTMODEL_H
#define NOTIFICATIONLISTMODEL_H

#include "qobjectlistmodel.h"

class Notification;

class NotificationListModel : public QObjectListModel
{
    Q_OBJECT

public:
    explicit NotificationListModel(QObject *parent = 0);
    virtual ~NotificationListModel();

private slots:
    void init();
    void updateNotification(uint id);
    void removeNotification(uint id);

protected:
    /*!
     * Checks whether the given notification should be shown. A notification
     * should be shown when it's class is not system and it has a body and a
     * summary.
     *
     * \param notification the notification to check
     * \return \c true if the notification should be shown, \c false otherwise
     */
    virtual bool notificationShouldBeShown(Notification *notification);

    /*!
     * Checks where the notification should be placed so that the
     * notifications in the model are ordered by timestamp.
     *
     * \param notification the notification for which to get the position
     * \return index in which the notification shoud be placed
     */
    virtual int indexFor(Notification *notification);

private:
    Q_DISABLE_COPY(NotificationListModel)
};

#endif // NOTIFICATIONLISTMODEL_H
