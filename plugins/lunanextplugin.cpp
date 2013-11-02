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

#include "lunanextplugin.h"
#include "compositor/compositor.h"
#include "settingsadapter.h"
#include "windowstate.h"
#include "reticleitem.h"
#include "lunaserviceadapter.h"
#include "fpscounter.h"
#include "screenshooter.h"
#include "eventtype.h"
#include "windowtype.h"
#include "statusbarservicesconnector.h"
#include "units.h"

static QObject *settings_callback(QQmlEngine *e, QJSEngine *)
{
    return new luna::SettingsAdapter();
}

static QObject *units_callback(QQmlEngine *e, QJSEngine *)
{
    return new luna::Units();
}

static QObject *statusbarservicesconnector_callback(QQmlEngine *e, QJSEngine *)
{
    return luna::StatusBarServicesConnector::instance();
}

LunaNextPlugin::LunaNextPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void LunaNextPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("LunaNext"));
    qmlRegisterType<luna::Compositor>(uri, 0, 1, "Compositor");
    qmlRegisterSingletonType<luna::SettingsAdapter>(uri, 0, 1, "Settings", settings_callback);
    qmlRegisterUncreatableType<luna::WindowState>(uri, 0, 1, "WindowState", "WindowState can't be used as component!");
    qmlRegisterUncreatableType<luna::WindowType>(uri, 0, 1, "WindowType", "WindowType can't be used as component!");
    qmlRegisterUncreatableType<luna::EventType>(uri, 0, 1, "EventType", "EventType can't be used as component!");
    qmlRegisterType<luna::ReticleItem>(uri, 0, 1, "Reticle");
    qmlRegisterType<luna::LunaServiceAdapter>(uri, 0, 1, "LunaService");
    qmlRegisterType<luna::FpsCounter>(uri, 0, 1, "FpsCounter");
    qmlRegisterType<luna::ScreenShooter>(uri, 0, 1, "ScreenShooter");
    qmlRegisterSingletonType<luna::StatusBarServicesConnector>(uri, 0, 1, "statusBarServicesConnector",
        statusbarservicesconnector_callback);
    qmlRegisterUncreatableType<luna::StatusBar>(uri, 0, 1, "StatusBarIconIndex", "StatusBarIconIndex can't be used as component");
    qmlRegisterSingletonType<luna::Units>(uri, 0, 1, "Units", units_callback);
}
