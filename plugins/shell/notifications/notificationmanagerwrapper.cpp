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

uint NotificationManagerWrapper::notify(const QString &appName, uint replacesId, const QString &appIcon,
                                                    const QString &summary, const QString &body, const QStringList &actions,
                                                    const QVariantHash &hints, int expireTimeout)
{
    return NotificationManager::instance()->Notify(appName, replacesId, appIcon, summary, body, actions, hints, expireTimeout);
}
