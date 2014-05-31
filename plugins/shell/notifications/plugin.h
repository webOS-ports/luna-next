/***************************************************************************
**
** Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
**
** This file is part of luna-next.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef LUNAPLUGIN_H_
#define LUNAPLUGIN_H_

#include <QQmlExtensionPlugin>

class Q_DECL_EXPORT LunaNextShellNotificationsPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")

public:
    explicit LunaNextShellNotificationsPlugin(QObject *parent = 0);
    void registerTypes(const char *uri);
    void initializeEngine(QQmlEngine * engine, const char * uri);
};

#endif
