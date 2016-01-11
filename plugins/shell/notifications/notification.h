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

#ifndef NOTIFICATION_H
#define NOTIFICATION_H

#include <QStringList>
#include <QDateTime>
#include <QVariantHash>
#include <QUrl>

/*!
 * An object for storing information about a single notification.
 */
class Notification : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ownerId READ ownerId NOTIFY ownerIdChanged)
    Q_PROPERTY(uint replacesId READ replacesId)
    Q_PROPERTY(QString launchId READ launchId NOTIFY launchIdChanged)
    Q_PROPERTY(QString launchParams READ launchParams)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString body READ body NOTIFY bodyChanged)
    Q_PROPERTY(QUrl iconUrl READ iconUrl NOTIFY iconUrlChanged)
    Q_PROPERTY(QString iconPath READ iconPath NOTIFY iconUrlChanged)
    Q_PROPERTY(QString soundClass READ soundClass NOTIFY soundClassChanged)
    Q_PROPERTY(QUrl soundFile READ soundFile NOTIFY soundFileChanged)
    Q_PROPERTY(QString soundFilePath READ soundFilePath NOTIFY soundFileChanged)
    Q_PROPERTY(int duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(bool doNotSuppress READ doNotSuppress NOTIFY doNotSuppressChanged)
    Q_PROPERTY(int priority READ priority NOTIFY priorityChanged)
    Q_PROPERTY(int expireTimeout READ expireTimeout)
    Q_PROPERTY(QDateTime timestamp READ timestamp)

public:
    /*!
     * Creates an object for storing information about a single notification.
     *
     * \param ownerId name of the or application/service sending the notification
     * \param replacesID the ID of the notification
     * \param launchId can default to ownerId, but allowed to be freely set in general for services and apps
     * \param launchParams parameters supplied to app when (re-)launched because user clicked on the notification
     * \param title title text for the notification, no markup
     * \param body body text for the notification, should use some markup
     * \param iconUrl icon url for the notification, only local urls (file://) are allowed
     * \param soundClass sound class for the notification
     * \param soundFile sound file url for the notification, only local urls (file://) are allowed
     * \param duration duration for the notification sound in seconds
     * \param doNotSuppress to determine if banner can be suppressed or not	 
     * \param priority priority of the notification
     * \param expireTimeout expiration timeout for the notification
     * \param parent the parent QObject
     */
    Notification(const QString &ownerId, uint replacesId, const QString &launchId, const QString &launchParams, const QString &title, const QString &body, const QUrl &iconUrl, const QString &soundClass, const QUrl &soundFile, int duration, bool doNotSuppress, int priority, int expireTimeout, QObject *parent = 0);

    /*!
     * Creates a new uninitialized representation of a notification.
     *
     * \param parent the parent QObject
     */
    Notification(QObject *parent = 0);

    //! Returns the name of the application/service sending the notification
    QString ownerId() const;

    //! Sets the name of the application sending the notification
    void setOwnerId(const QString &ownerId);

    //! Returns the ID of the notification
    uint replacesId() const;

    //! Returns the name of the application/service associated to an action on the notification
    QString launchId() const;

    //! Sets the name of the application/service associated to an action on the notification
    void setLaunchId(const QString &launchId);

    //! Returns the parameters that are associated to the action on the notification
    QString launchParams() const;

    //! Sets the parameters that are associated to the action on the notification
    void setLaunchParams(const QString &launchParams);

    //! Returns the title text for the notification
    QString title() const;

    //! Sets the title text for the notification
    void setTitle(const QString &title);

    //! Returns the body text for the notification
    QString body() const;

    //! Sets the body text for the notification
    void setBody(const QString &body);

    //! Returns the icon url for the notification
    QUrl iconUrl() const;

    //! Sets the icon url for the notification
    void setIconUrl(const QUrl &iconUrl);

    //! Returns the icon path for the notification as a string
    QString iconPath() const;

    //! Returns the sound class for the notification
    QString soundClass() const;

    //! Sets the sound Class for the notification
    void setSoundClass(const QString &soundClass);

    //! Returns the sound file for the notification
    QUrl soundFile() const;

    //! Sets the sound file for the notification
    void setSoundFile(const QUrl &soundFile);

    //! Returns the sound file for the notification as a string
    QString soundFilePath() const;

    //! Returns the duration for the notification sound
    int duration() const;

    //! Sets the duration for the notification sound
    void setDuration(int duration);

    //! Returns the doNotSuppress for the notification
    bool doNotSuppress() const;

    //! Sets the doNotSuppress for the notification
    void setDoNotSuppress(bool doNotSuppress);
	
    //! Returns the priority of the notification
    int priority() const;

    //! Sets the priority of the notification
    void setPriority(int priority);

    //! Returns the expiration timeout for the notification
    int expireTimeout() const;

    //! Sets the expiration timeout for the notification
    void setExpireTimeout(int expireTimeout);

    //! Returns the timestamp for the notification
    QDateTime timestamp() const;

    //! Reset the timestamp of the notification
    void resetTimeStamp();

    //! Returns the item count of the notification
    int itemCount() const;

    //! \internal
    /*!
     * Creates a copy of an existing representation of a notification.
     * This constructor should only be used for populating the notification
     * list from D-Bus structures.
     *
     * \param notification the notification representation to a create copy of
     */
    explicit Notification(const Notification &notification);

signals:
    /*!
     * Sent when the notification is activated.
     */
    void launchInvoked();

    //! Sent when the removal of this notification was requested.
    void removeRequested();

    //! Sent when the ownerId has been modified
    void ownerIdChanged();

    //! Sent when the launchId has been modified
    void launchIdChanged();

    //! Sent when the summary has been modified
    void titleChanged();

    //! Sent when the body has been modified
    void bodyChanged();

    //! Sent when the icon url has been modified
    void iconUrlChanged();

    //! Sent when the sound class has been modified
    void soundClassChanged();

    //! Sent when the sound file has been modified
    void soundFileChanged();

    //! Sent when the duration has been modified
    void durationChanged();

    //! Sent when doNotSuppress has been modified
    void doNotSuppressChanged();
	
    //! Sent when the priority has been modified
    void priorityChanged();

private:
    //! Name of the application/service sending the notification
    QString ownerId_;

    //! The ID of the notification
    uint replacesId_;

    //! name of the application/service associated to an action on the notification
    QString launchId_;

    //! parameters associated to an action on the notification
    QString launchParams_;

    //! Title text for the notification
    QString title_;

    //! Body text for the notification (may contain markup)
    QString body_;

    //! icon url for the notification
    QUrl iconUrl_;
	
	//! sound class for the notification
    QString soundClass_;

	//! sound file for the notification
    QUrl soundFile_;

	//! sound duration for the notification
    int duration_;

    //! determine if allow suppression for the notification
    bool doNotSuppress_;

    //! Priority of the notification
    int priority_;

    //! Expiration timeout for the notification
    int expireTimeout_;

    QDateTime timestamp_;
};

Q_DECLARE_METATYPE(Notification)

class NotificationList
{
public:
    NotificationList();
    NotificationList(const QList<Notification *> &notificationList);
    NotificationList(const NotificationList &notificationList);
    QList<Notification *> notifications() const;

private:
    QList<Notification *> notificationList;
};

Q_DECLARE_METATYPE(NotificationList)

#endif // NOTIFICATION_H
