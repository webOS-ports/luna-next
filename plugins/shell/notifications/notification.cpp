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

Notification::Notification(const QString &ownerId, uint replacesId, const QString &launchId, const QString &launchParam, const QString &title, const QString &body, const QUrl &iconUrl, int priority, int expireTimeout, QObject *parent) :
    QObject(parent),
    ownerId_(ownerId),
    replacesId_(replacesId),
    launchId_(launchId),
    launchParam_(launchParam),
    title_(title),
    body_(body),
    iconUrl_(iconUrl),
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
    title_(notification.title_),
    body_(notification.body_),
    iconUrl_(notification.iconUrl_),
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
    ownerId_ = ownerId;
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
    launchId_ = launchId;
}

QString Notification::launchParam() const
{
    return launchParam_;
}

void Notification::setLaunchParam(const QString &launchParam)
{
    launchParam_ = launchParam;
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

