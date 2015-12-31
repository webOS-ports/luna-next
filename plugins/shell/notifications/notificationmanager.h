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

#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include "notification.h"
#include <QObject>
#include <QTimer>
#include <QSet>
#include <QList>

class QSqlDatabase;

/*!
 * \class NotificationManager
 *
 * \brief The notification manager allows applications to display notifications to the user.
 *
 * The notification manager implements a desktop notifications service based
 * on the <a href="http://www.galago-project.org/specs/notification/0.9/">Desktop Notifications Specification</a>.
 * The service is registered as org.freedesktop.Notifications on the D-Bus
 * session bus in the path /org/freedesktop/Notifications.
 */
class NotificationManager : public QObject
{
    Q_OBJECT

public:
    //! Notifation closing reasons used in the NotificationClosed signal
    enum NotificationClosedReason {
        //! The notification expired.
        NotificationExpired = 1,
        //! The notification was dismissed by the user.
        NotificationDismissedByUser,
        //! The notification was closed by a call to CloseNotification.
        CloseNotificationCalled
    };

    /*!
     * Returns a singleton instance of the notification manager.
     *
     * \return an instance of the notification manager
     */
    static NotificationManager *instance();

    /*!
     * Returns a notification with the given ID.
     *
     * \param id the ID of the notification to return
     * \return the notification with the given ID
     */
    Notification *notification(uint id) const;

    /*!
     * Returns a list of notification IDs.
     *
     * \return a list of notification IDs.
     */
    QList<uint> notificationIds() const;

    uint GetIdForNotification(Notification *notification) const;

    /*!
     * Sends a notification to the notification server.
     *
     * \param ownerId name of the or application/service sending the notification
     * \param replacesID the ID of the notification
     * \param launchId can default to ownerId, but allowed to be freely set in general for services and apps
     * \param launchParams parameters supplied to app when (re-)launched because user clicked on the notification
     * \param title title text for the notification, no markup
     * \param body body text for the notification, should use some markup
     * \param iconUrl icon url for the notification, only local urls (file://) are allowed
     * \param priority priority of the notification
     * \param expireTimeout expiration timeout for the notification
     * \param parent the parent QObject
     */
    uint Notify(const QString &ownerId, uint replacesId, const QString &launchId, const QString &launchParams,
                const QString &title, const QString &body, const QUrl &iconUrl, const QString &soundClass,
                const QUrl &soundFile, int duration, bool doNotSuppress, int priority, int expireTimeout);

    /*!
     * Causes a notification to be forcefully closed and removed from the user's view.
     * It can be used, for example, in the event that what the notification pertains
     * to is no longer relevant, or to cancel a notification with no expiration time.
     * The NotificationClosed signal is emitted by this method.
     *
     * \param id the ID of the notification to be closed
     */
    void CloseNotification(uint id, NotificationClosedReason closeReason = CloseNotificationCalled);

    /*!
     * Returns the notifications sent by a specified application.
     *
     * \param appName the name of the application to get notifications for
     * \return a list of notifications for the application
     */
    NotificationList GetNotifications(const QString &ownerId);

signals:
    /*!
     * A completed notification is one that has timed out, or has been dismissed by the user.
     *
     * \param id The ID of the notification that was closed.
     * \param reason The reason the notification was closed. 1 - The notification expired. 2 - The notification was dismissed by the user. 3 - The notification was closed by a call to CloseNotification. 4 - Undefined/reserved reasons.
     */
    void notificationClosed(uint id, uint reason);

    /*!
     * This signal is emitted when one of the following occurs:
     *   * The user performs some global "invoking" action upon a notification. For instance, clicking somewhere on the notification itself.
     *   * The user invokes a specific action as specified in the original Notify request. For example, clicking on an action button.
     *
     * \param id The ID of the notification emitting the ActionInvoked signal.
     * \param actionKey The key of the action invoked. These match the keys sent over in the list of actions.
     */
    void launchInvoked(uint id);

    /*!
     * Emitted when a notification is modified (added or updated).
     *
     * \param id the ID of the modified notification
     */
    void notificationModified(uint id);

    /*!
     * Emitted when a notification is removed.
     *
     * \param id the ID of the removed notification
     */
    void notificationRemoved(uint id);

public slots:
    /*!
     * Removes all notifications which are user removable.
     */
    void removeNotifications(const QString &ownerId);

private slots:

    /*!
     * Commits the current database transaction, if any.
     * Also destroys any removed notifications.
     */
    void commit();

    /*!
     * Invokes the given action if it is has been defined. The
     * sender is expected to be a Notification.
     *
     * \param action the action to be invoked
     */
    void launchNotification();

    /*!
     * Removes a notification if it is removable by the user.
     *
     * \param id the ID of the notification to be removed
     */
    void removeNotification(uint id = 0);

private:
    /*!
     * Creates a new notification manager.
     *
     * \param parent the parent object
     */
    explicit NotificationManager(QObject *parent = 0);

    //! Destroys the notification manager.
    virtual ~NotificationManager();

    /*!
     * Returns the next available notification ID
     *
     * \return The next available notification ID
     */
    uint nextAvailableNotificationID();


    //! Restores the notifications from a database on the disk
    void restoreNotifications();

    /*!
     * Creates a connection to the Sqlite database.
     *
     * \return \c true if the connection was successfully established, \c false otherwise
     */
    bool connectToDatabase();

    /*!
     * Checks whether there is enough free disk space available.
     *
     * \param path any path to the file system from which the space should be checked
     * \param freeSpaceNeeded free space needed in kilobytes
     * \return \c true if there is enough free space in given file system, \c false otherwise
     */
    static bool checkForDiskSpace(const QString &path, unsigned long freeSpaceNeeded);

    /*!
     * Removes a database file from the filesystem. Removes related -wal and -shm files as well.
     *
     * \param path the path of the database file to be removed
     */
    static void removeDatabaseFile(const QString &path);

    /*!
     * Ensures that all database tables have the requires fields.
     * Recreates the tables if needed.
     *
     * \return \c true if the database can be used, \c false otherwise
     */
    bool checkTableValidity();

    /*!
     * Recreates a table in the database.
     *
     * \param tableName the name of the table to be created
     * \param definition SQL definition for the table
     * \return \c true if the table was created, \c false otherwise
     */
    bool recreateTable(const QString &tableName, const QString &definition);

    //! Fills the notifications hash table with data from the database
    void fetchData();

    /*!
     * Executes a SQL command in the database. Starts a new transaction if none is active currently, otherwise
     * the command goes to the active transaction. Restarts the transaction commit timer.
     * \param command the SQL command
     * \param args list of values to be bound to the positional placeholders ('?' -character) in the command.
     */
    void execSQL(const QString &command, const QVariantList &args = QVariantList());

    //! The singleton notification manager instance
    static NotificationManager *instance_;

    //! Hash of all notifications keyed by notification IDs
    QHash<uint, Notification*> notifications;

    //! Notifications waiting to be destroyed
    QSet<Notification *> removedNotifications;

    //! Previous notification ID used
    uint previousNotificationID;

    //! Database for the notifications
    QSqlDatabase *database;

    //! Whether the current database transaction has been committed to the database
    bool committed;

    //! Timer for triggering the commit of the current database transaction
    QTimer databaseCommitTimer;
};

#endif // NOTIFICATIONMANAGER_H
