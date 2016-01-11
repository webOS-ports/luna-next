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

#include <QTimer>

#include "notificationmanager.h"
#include "notification.h"

Notification::Notification(const QString &ownerId, uint replacesId, const QString &launchId, const QString &launchParams, const QString &title, const QString &body, const QUrl &iconUrl, const QString &soundClass, const QUrl &soundFile, int duration, bool doNotSuppress, int priority, int expireTimeout, QObject *parent) :
    QObject(parent),
    ownerId_(ownerId),
    replacesId_(replacesId),
    launchId_(launchId),
    launchParams_(launchParams),
    title_(title),
    body_(body),
    iconUrl_(iconUrl),
    soundClass_(soundClass),
    soundFile_(soundFile),
    duration_(duration),
    doNotSuppress_(doNotSuppress),
    priority_(priority),
    expireTimeout_(expireTimeout),
    timestamp_(QDateTime::currentDateTimeUtc())
{
}

Notification::Notification(QObject *parent) :
    QObject(parent),
    replacesId_(0),
    expireTimeout_(-1),
    timestamp_(QDateTime::currentDateTimeUtc())
{
}

Notification::Notification(const Notification &notification) :
    QObject(notification.parent()),
    ownerId_(notification.ownerId_),
    replacesId_(notification.replacesId_),
    launchId_(notification.launchId_),
    launchParams_(notification.launchParams_),
    title_(notification.title_),
    body_(notification.body_),
    iconUrl_(notification.iconUrl_),
    soundClass_(notification.soundClass_),
    soundFile_(notification.soundFile_),
    duration_(notification.duration_),
    doNotSuppress_(notification.doNotSuppress_),
    priority_(notification.priority_),
    expireTimeout_(notification.expireTimeout_),
    timestamp_(notification.timestamp_)
{
}

QString Notification::ownerId() const
{
    return ownerId_;
}

void Notification::setOwnerId(const QString &ownerId)
{
    if (ownerId_ != ownerId) {
        ownerId_ = ownerId;
        emit ownerIdChanged();
    }
}

uint Notification::replacesId() const
{
    return replacesId_;
}

QString Notification::launchId() const
{
    return launchId_;
}

void Notification::setLaunchId(const QString &launchId)
{
    if (launchId_ != launchId) {
        launchId_ = launchId;
        emit launchIdChanged();
    }
}

QString Notification::launchParams() const
{
    return launchParams_;
}

void Notification::setLaunchParams(const QString &launchParams)
{
    launchParams_ = launchParams;
}

QString Notification::title() const
{
    return title_;
}

void Notification::setTitle(const QString &title)
{
    if (title_ != title) {
        title_ = title;
        emit titleChanged();
    }
}

QString Notification::body() const
{
    return body_;
}

void Notification::setBody(const QString &body)
{
    if (body_ != body) {
        body_ = body;
        emit bodyChanged();
    }
}

QUrl Notification::iconUrl() const
{
    return iconUrl_;
}

void Notification::setIconUrl(const QUrl &iconUrl)
{
    if (iconUrl_ != iconUrl) {
        iconUrl_ = iconUrl;
        emit iconUrlChanged();
    }
}

QString Notification::iconPath() const
{
    return iconUrl_.toString();
}

QString Notification::soundClass() const
{
    return soundClass_;
}

void Notification::setSoundClass(const QString &soundClass)
{
    if (soundClass_ != soundClass) {
        soundClass_ = soundClass;
        emit soundClassChanged();
    }
}

QUrl Notification::soundFile() const
{
    return soundFile_;
}

void Notification::setSoundFile(const QUrl &soundFile)
{
    if (soundFile_ != soundFile) {
        soundFile_ = soundFile;
        emit soundFileChanged();
    }
}

QString Notification::soundFilePath() const
{
    return soundFile_.toString();
}

int Notification::duration() const
{
    return duration_;
}

void Notification::setDuration(int duration)
{
    if (duration_ != duration) {
        duration_ = duration;
        emit durationChanged();
    }
}

bool Notification::doNotSuppress() const
{
    return doNotSuppress_;
}

void Notification::setDoNotSuppress(bool doNotSuppress)
{
    if (doNotSuppress_ != doNotSuppress) {
        doNotSuppress_ = doNotSuppress;
        emit doNotSuppressChanged();
    }
}

int Notification::priority() const
{
    return priority_;
}

void Notification::setPriority(int priority)
{
    if (priority_ != priority) {
        priority_ = priority;
        emit priorityChanged();
    }
}

int Notification::expireTimeout() const
{
    return expireTimeout_;
}

void Notification::setExpireTimeout(int expireTimeout)
{
    expireTimeout_ = expireTimeout;
}

QDateTime Notification::timestamp() const
{
    return timestamp_;
}

void Notification::resetTimeStamp()
{
    timestamp_ = QDateTime::currentDateTimeUtc();
}

NotificationList::NotificationList()
{
}

NotificationList::NotificationList(const QList<Notification *> &notificationList) :
    notificationList(notificationList)
{
}

NotificationList::NotificationList(const NotificationList &notificationList) :
    notificationList(notificationList.notificationList)
{
}

QList<Notification *> NotificationList::notifications() const
{
    return notificationList;
}

