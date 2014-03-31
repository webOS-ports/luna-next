/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include <QtQml>

#include "plugin.h"
#include "settingsadapter.h"
#include "lunaserviceadapter.h"
#include "units.h"

static QObject *settings_callback(QQmlEngine *e, QJSEngine *)
{
    return new luna::SettingsAdapter();
}

static QObject *units_callback(QQmlEngine *e, QJSEngine *)
{
    return new luna::Units();
}

LunaNextCommonPlugin::LunaNextCommonPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void LunaNextCommonPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("LunaNext.Common"));
    qmlRegisterSingletonType<luna::SettingsAdapter>(uri, 0, 1, "Settings", settings_callback);
    qmlRegisterType<luna::LunaServiceAdapter>(uri, 0, 1, "LunaService");
    qmlRegisterSingletonType<luna::Units>(uri, 0, 1, "Units", units_callback);
}

void LunaNextCommonPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
}
