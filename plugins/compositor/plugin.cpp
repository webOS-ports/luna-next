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

#include <QObject>
#include <QtQml>

#include "plugin.h"
#include "compositor.h"
#include "windowstate.h"
#include "screenshooter.h"
#include "eventtype.h"
#include "windowtype.h"
#include "displaycontroller.h"
#include "windowmodel.h"

static QObject *displaycontroller_callback(QQmlEngine *e, QJSEngine *)
{
    return new luna::DisplayController();
}

LunaNextCompositorPlugin::LunaNextCompositorPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void LunaNextCompositorPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("LunaNext.Compositor"));
    qmlRegisterType<luna::Compositor>(uri, 0, 1, "Compositor");
    qmlRegisterType<luna::WindowModel>(uri, 0, 1, "WindowModel");
    qmlRegisterUncreatableType<luna::WindowState>(uri, 0, 1, "WindowState", "WindowState can't be used as component!");
    qmlRegisterUncreatableType<luna::WindowType>(uri, 0, 1, "WindowType", "WindowType can't be used as component!");
    qmlRegisterUncreatableType<luna::EventType>(uri, 0, 1, "EventType", "EventType can't be used as component!");
    qmlRegisterSingletonType<luna::DisplayController>(uri, 0, 1, "DisplayController", displaycontroller_callback);
    qmlRegisterType<luna::ScreenShooter>(uri, 0, 1, "ScreenShooter");
}

void LunaNextCompositorPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
}
