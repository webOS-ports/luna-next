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
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDir>
#include <sys/statfs.h>
#include "notificationmanager.h"

#define NOTIFICATIONS_DEBUG(things) qDebug() << Q_FUNC_INFO << things

//! Path of the privileged storage directory relative to the home directory
static const char *PRIVILEGED_DATA_PATH= "/var/luna/data/";

//! Minimum amount of disk space needed for the notification database in kilobytes
static const uint MINIMUM_FREE_SPACE_NEEDED_IN_KB = 1024;

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
    previousNotificationID(0),
    database(new QSqlDatabase),
    committed(true)
{
    qDebug() << __PRETTY_FUNCTION__;

    // Commit the modifications to the database 10 seconds after the last modification so that writing to disk doesn't affect user experience
    databaseCommitTimer.setInterval(10000);
    databaseCommitTimer.setSingleShot(true);
    connect(&databaseCommitTimer, SIGNAL(timeout()), this, SLOT(commit()));

    restoreNotifications();
}

NotificationManager::~NotificationManager()
{
    database->commit();
    delete database;

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

uint NotificationManager::Notify(const QString &ownerId, uint replacesId, const QString &launchId, const QString &launchParams,
                                 const QString &title, const QString &body, const QUrl &iconUrl, const QString &soundClass,
                                 const QUrl &soundFile, int duration, bool doNotSuppress, int priority, int expireTimeout)
{
    uint id = replacesId != 0 ? replacesId : nextAvailableNotificationID();

    Notification *notificationToReplace = notifications.value(id);

    if (replacesId == 0 || (notificationToReplace && notificationToReplace->ownerId() == ownerId)) {
        if (replacesId == 0) {
            // Create a new notification
            Notification *notification = new Notification(ownerId, id, launchId, launchParams, title, body, iconUrl, soundClass, soundFile, duration, doNotSuppress, priority, expireTimeout, this);
            connect(notification, SIGNAL(launchInvoked()), this, SLOT(launchNotification()), Qt::QueuedConnection);
            connect(notification, SIGNAL(removeRequested()), this, SLOT(removeNotification()), Qt::QueuedConnection);
            notifications.insert(id, notification);
        }
        else {
            // Only replace an existing notification if it really exists and ownerId is the same
            notificationToReplace->setLaunchId(launchId);
            notificationToReplace->setLaunchParams(launchParams);
            notificationToReplace->setTitle(title);
            notificationToReplace->setBody(body);
            notificationToReplace->setIconUrl(iconUrl);
            notificationToReplace->setSoundClass(soundClass);
            notificationToReplace->setSoundFile(soundFile);
            notificationToReplace->setDuration(duration);
            notificationToReplace->setDoNotSuppress(doNotSuppress);
            notificationToReplace->setPriority(priority);
            notificationToReplace->setExpireTimeout(expireTimeout);
            notificationToReplace->resetTimeStamp();

            // Delete the existing notification from the database
            execSQL(QString("DELETE FROM notifications WHERE id=?"), QVariantList() << id);
        }

        // Add the notification to the database
        execSQL("INSERT INTO notifications VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)", QVariantList() << id << ownerId << launchId << launchParams << title << body << iconUrl << soundClass << soundFile << duration << doNotSuppress << priority << expireTimeout);

        NOTIFICATIONS_DEBUG("NOTIFY:" << ownerId << iconUrl << soundClass << soundFile << duration << doNotSuppress << title << body << expireTimeout << "->" << id);
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

        // Remove the notification from database
        execSQL(QString("DELETE FROM notifications WHERE id=?"), QVariantList() << id);

        NOTIFICATIONS_DEBUG("REMOVE:" << id);
        emit notificationRemoved(id);

        // Mark the notification to be destroyed
        removedNotifications.insert(notifications.take(id));
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

void NotificationManager::restoreNotifications()
{
    if (connectToDatabase()) {
        if (checkTableValidity()) {
            fetchData();
        } else {
            database->close();
        }
    }
}

bool NotificationManager::connectToDatabase()
{
    QString databasePath = QString(PRIVILEGED_DATA_PATH);
    if (!QDir::root().exists(databasePath)) {
        QDir::root().mkpath(databasePath);
    }
    QString databaseName = databasePath + "/notifications.db";

    *database = QSqlDatabase::addDatabase("QSQLITE", metaObject()->className());
    database->setDatabaseName(databaseName);
    bool success = checkForDiskSpace(databasePath, MINIMUM_FREE_SPACE_NEEDED_IN_KB);
    if (success) {
        success = database->open();
        if (!success) {
            NOTIFICATIONS_DEBUG(database->lastError().driverText() << databaseName << database->lastError().databaseText());

            // If opening the database fails, try to recreate the database
            removeDatabaseFile(databaseName);
            success = database->open();
            NOTIFICATIONS_DEBUG("Unable to open database file. Recreating. Success: " << success);
        }
    } else {
        NOTIFICATIONS_DEBUG("Not enough free disk space available. Unable to open database.");
    }

    if (success) {
        // Set up the database mode to write-ahead locking to improve performance
        QSqlQuery(*database).exec("PRAGMA journal_mode=WAL");
    }

    return success;
}

bool NotificationManager::checkForDiskSpace(const QString &path, unsigned long freeSpaceNeeded)
{
    struct statfs st;
    bool spaceAvailable = false;
    if (statfs(path.toUtf8().data(), &st) != -1) {
        unsigned long freeSpaceInKb = (st.f_bsize * st.f_bavail) / 1024;
        if (freeSpaceInKb > freeSpaceNeeded) {
            spaceAvailable = true;
        }
    }
    return spaceAvailable;
}

void NotificationManager::removeDatabaseFile(const QString &path)
{
    // Remove also -shm and -wal files created when journal-mode=WAL is being used
    QDir::root().remove(path + "-shm");
    QDir::root().remove(path + "-wal");
    QDir::root().remove(path);
}

bool NotificationManager::checkTableValidity()
{
    bool result = true;
    bool recreateNotificationsTable = false;

    {
        // Check that the notifications table schema is as expected
        QSqlTableModel notificationsTableModel(0, *database);
        notificationsTableModel.setTable("notifications");
        recreateNotificationsTable = (notificationsTableModel.fieldIndex("id") == -1 ||
                                      notificationsTableModel.fieldIndex("ownerId") == -1 ||
                                      notificationsTableModel.fieldIndex("launchId") == -1 ||
                                      notificationsTableModel.fieldIndex("launchParams") == -1 ||
                                      notificationsTableModel.fieldIndex("title") == -1 ||
                                      notificationsTableModel.fieldIndex("body") == -1 ||
                                      notificationsTableModel.fieldIndex("iconUrl") == -1 ||
                                      notificationsTableModel.fieldIndex("soundClass") == -1 ||
                                      notificationsTableModel.fieldIndex("soundFile") == -1 ||
                                      notificationsTableModel.fieldIndex("duration") == -1 ||
                                      notificationsTableModel.fieldIndex("doNotSuppress") == -1 ||
                                      notificationsTableModel.fieldIndex("priority") == -1 ||
                                      notificationsTableModel.fieldIndex("expire_timeout") == -1);
    }

    if (recreateNotificationsTable) {
        result &= recreateTable("notifications", "id INTEGER PRIMARY KEY, ownerId TEXT, launchId TEXT, launchParams TEXT, title TEXT, body TEXT, iconUrl TEXT, soundClass TEXT, soundFile TEXT, duration INTEGER, doNotSuppress INTEGER, priority INTEGER, expire_timeout INTEGER");
    }

    return result;
}

bool NotificationManager::recreateTable(const QString &tableName, const QString &definition)
{
    bool result = false;

    if (database->isOpen()) {
        QSqlQuery(*database).exec("DROP TABLE " + tableName);
        result = QSqlQuery(*database).exec("CREATE TABLE " + tableName + " (" + definition + ")");
    }

    return result;
}

void NotificationManager::fetchData()
{
    // Create the notifications
    QSqlQuery notificationsQuery("SELECT * FROM notifications", *database);
    QSqlRecord notificationsRecord = notificationsQuery.record();
    int notificationsTableIdFieldIndex = notificationsRecord.indexOf("id");
    int notificationsTableOwnerIdFieldIndex = notificationsRecord.indexOf("ownerId");
    int notificationsTableLaunchIdFieldIndex = notificationsRecord.indexOf("launchId");
    int notificationsTableLaunchParamsFieldIndex = notificationsRecord.indexOf("launchParams");
    int notificationsTableTitleFieldIndex = notificationsRecord.indexOf("title");
    int notificationsTableBodyFieldIndex = notificationsRecord.indexOf("body");
    int notificationsTableIconUrlFieldIndex = notificationsRecord.indexOf("iconUrl");
    int notificationsTableSoundClassFieldIndex = notificationsRecord.indexOf("soundClass");
    int notificationsTableSoundFileFieldIndex = notificationsRecord.indexOf("soundFile");
    int notificationsTableDurationFieldIndex = notificationsRecord.indexOf("duration");
    int notificationsTableDoNotSuppressFieldIndex = notificationsRecord.indexOf("doNotSuppress");
    int notificationsTablePriorityFieldIndex = notificationsRecord.indexOf("priority");
    int notificationsTableExpireTimeoutFieldIndex = notificationsRecord.indexOf("expire_timeout");
    while (notificationsQuery.next()) {
        uint id = notificationsQuery.value(notificationsTableIdFieldIndex).toUInt();
        QString ownerId = notificationsQuery.value(notificationsTableOwnerIdFieldIndex).toString();
        QString launchId = notificationsQuery.value(notificationsTableLaunchIdFieldIndex).toString();
        QString launchParams = notificationsQuery.value(notificationsTableLaunchParamsFieldIndex).toString();
        QString title = notificationsQuery.value(notificationsTableTitleFieldIndex).toString();
        QString body = notificationsQuery.value(notificationsTableBodyFieldIndex).toString();
        QUrl iconUrl = notificationsQuery.value(notificationsTableIconUrlFieldIndex).toUrl();
        QString soundClass = notificationsQuery.value(notificationsTableSoundClassFieldIndex).toString();
        QUrl soundFile = notificationsQuery.value(notificationsTableSoundFileFieldIndex).toUrl();
        int duration = notificationsQuery.value(notificationsTableDurationFieldIndex).toInt();
        bool doNotSuppress = notificationsQuery.value(notificationsTableDoNotSuppressFieldIndex).toBool();
        int priority = notificationsQuery.value(notificationsTablePriorityFieldIndex).toInt();
        int expireTimeout = notificationsQuery.value(notificationsTableExpireTimeoutFieldIndex).toInt();
        Notification *notification = new Notification(ownerId, id, launchId, launchParams, title, body, iconUrl, soundClass, soundFile, duration, doNotSuppress, priority, expireTimeout, this);
        connect(notification, SIGNAL(launchInvoked()), this, SLOT(launchNotification()), Qt::QueuedConnection);
        connect(notification, SIGNAL(removeRequested()), this, SLOT(removeNotification()), Qt::QueuedConnection);
        notifications.insert(id, notification);

        NOTIFICATIONS_DEBUG("RESTORED:" << ownerId << launchId << launchParams << title << body << iconUrl << soundClass << soundFile << duration << doNotSuppress << priority << expireTimeout << "->" << id);
        emit notificationModified(id);

        if (id > previousNotificationID) {
            // Use the highest notification ID found as the previous notification ID
            previousNotificationID = id;
        }
    }
}

void NotificationManager::commit()
{
    // Any aditional rules about when database commits are allowed can be added here
    if (!committed) {
        database->commit();
        committed = true;
    }

    qDeleteAll(removedNotifications);
    removedNotifications.clear();
}

void NotificationManager::execSQL(const QString &command, const QVariantList &args)
{
    if (!database->isOpen()) {
        return;
    }

    if (committed) {
        committed = false;
        database->transaction();
    }

    QSqlQuery query(*database);
    query.prepare(command);

    foreach(const QVariant &arg, args) {
        query.addBindValue(arg);
    }

    query.exec();

    if (query.lastError().isValid()) {
        NOTIFICATIONS_DEBUG(command << args << query.lastError());
    }

    databaseCommitTimer.start();
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
