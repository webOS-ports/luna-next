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

#include <QDBusArgument>
#include "notificationmanager.h"
#include "notification.h"

Notification::Notification(const QString &appName, uint replacesId, const QString &appIcon, const QString &summary, const QString &body, const QStringList &actions, const QVariantHash &hints, int expireTimeout, QObject *parent) :
    QObject(parent),
    appName_(appName),
    replacesId_(replacesId),
    appIcon_(appIcon),
    summary_(summary),
    body_(body),
    actions_(actions),
    hints_(hints),
    expireTimeout_(expireTimeout)
{
}

Notification::Notification(QObject *parent) :
    QObject(parent),
    replacesId_(0),
    expireTimeout_(-1)
{
}

Notification::Notification(const Notification &notification) :
    QObject(notification.parent()),
    appName_(notification.appName_),
    replacesId_(notification.replacesId_),
    appIcon_(notification.appIcon_),
    summary_(notification.summary_),
    body_(notification.body_),
    actions_(notification.actions_),
    hints_(notification.hints_),
    expireTimeout_(notification.expireTimeout_)
{
}

QString Notification::appName() const
{
    return appName_;
}

void Notification::setAppName(const QString &appName)
{
    appName_ = appName;
}

uint Notification::replacesId() const
{
    return replacesId_;
}

QString Notification::appIcon() const
{
    return appIcon_;
}

void Notification::setAppIcon(const QString &appIcon)
{
    appIcon_ = appIcon;
}

QString Notification::summary() const
{
    return summary_;
}

void Notification::setSummary(const QString &summary)
{
    if (summary_ != summary) {
        summary_ = summary;
        emit summaryChanged();
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

QStringList Notification::actions() const
{
    return actions_;
}

void Notification::setActions(const QStringList &actions)
{
    actions_ = actions;
}

QVariantHash Notification::hints() const
{
    return hints_;
}

void Notification::setHints(const QVariantHash &hints)
{
    QString oldIcon = icon();
    QDateTime oldTimestamp = timestamp();
    QString oldPreviewIcon = previewIcon();
    QString oldPreviewSummary = previewSummary();
    QString oldPreviewBody = previewBody();
    int oldUrgency = urgency();
    int oldItemCount = itemCount();
    int oldPriority = priority();
    QString oldCategory = category();

    hints_ = hints;

    if (oldIcon != icon()) {
        emit iconChanged();
    }

    if (oldTimestamp != timestamp()) {
        emit timestampChanged();
    }

    if (oldPreviewIcon != previewIcon()) {
        emit previewIconChanged();
    }

    if (oldPreviewSummary != previewSummary()) {
        emit previewSummaryChanged();
    }

    if (oldPreviewBody != previewBody()) {
        emit previewBodyChanged();
    }

    if (oldUrgency != urgency()) {
        emit urgencyChanged();
    }

    if (oldItemCount != itemCount()) {
        emit itemCountChanged();
    }

    if (oldPriority != priority()) {
        emit priorityChanged();
    }

    if (oldCategory != category()) {
        emit categoryChanged();
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

QString Notification::icon() const
{
    return appIcon_.isEmpty() ? hints_.value(NotificationManager::HINT_ICON).toString() : appIcon_;
}

QDateTime Notification::timestamp() const
{
    return hints_.value(NotificationManager::HINT_TIMESTAMP).toDateTime();
}

QString Notification::previewIcon() const
{
    return hints_.value(NotificationManager::HINT_PREVIEW_ICON).toString();
}

QString Notification::previewSummary() const
{
    return hints_.value(NotificationManager::HINT_PREVIEW_SUMMARY).toString();
}

QString Notification::previewBody() const
{
    return hints_.value(NotificationManager::HINT_PREVIEW_BODY).toString();
}

int Notification::urgency() const
{
    return hints_.value(NotificationManager::HINT_URGENCY).toInt();
}

int Notification::itemCount() const
{
    return hints_.value(NotificationManager::HINT_ITEM_COUNT).toInt();
}

int Notification::priority() const
{
    return hints_.value(NotificationManager::HINT_PRIORITY).toInt();
}

QString Notification::category() const
{
    return hints_.value(NotificationManager::HINT_CATEGORY).toString();
}

bool Notification::isUserRemovable() const
{
    return hints_.value(NotificationManager::HINT_USER_REMOVABLE, QVariant(true)).toBool();
}

QDBusArgument &operator<<(QDBusArgument &argument, const Notification &notification)
{
    argument.beginStructure();
    argument << notification.appName_;
    argument << notification.replacesId_;
    argument << notification.appIcon_;
    argument << notification.summary_;
    argument << notification.body_;
    argument << notification.actions_;
    argument << notification.hints_;
    argument << notification.expireTimeout_;
    argument.endStructure();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, Notification &notification)
{
    argument.beginStructure();
    argument >> notification.appName_;
    argument >> notification.replacesId_;
    argument >> notification.appIcon_;
    argument >> notification.summary_;
    argument >> notification.body_;
    argument >> notification.actions_;
    argument >> notification.hints_;
    argument >> notification.expireTimeout_;
    argument.endStructure();
    return argument;
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

QDBusArgument &operator<<(QDBusArgument &argument, const NotificationList &notificationList)
{
    argument.beginArray(qMetaTypeId<Notification>());
    foreach (Notification *notification, notificationList.notificationList) {
        argument << *notification;
    }
    argument.endArray();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, NotificationList &notificationList)
{
    argument.beginArray();
    notificationList.notificationList.clear();
    while (!argument.atEnd()) {
        Notification *notification = new Notification;
        argument >> *notification;
        notificationList.notificationList.append(notification);
    }
    argument.endArray();
    return argument;
}
