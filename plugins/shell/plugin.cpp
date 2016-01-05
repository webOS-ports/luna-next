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
#include "reticleitem.h"
#include "fpscounter.h"
#include "devicekeyhandler.h"
#include "gesturehandler.h"
#include "inversemouseareatype.h"
#include "volumekeys.h"

LunaNextShellPlugin::LunaNextShellPlugin(QObject *parent) :
    QQmlExtensionPlugin(parent)
{
}

void LunaNextShellPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("LunaNext.Shell"));
    qmlRegisterType<luna::ReticleItem>(uri, 0, 1, "Reticle");
    qmlRegisterType<luna::FpsCounter>(uri, 0, 1, "FpsCounter");
    qmlRegisterType<luna::DeviceKeyHandler>(uri, 0, 1, "DeviceKeyHandler");
    qmlRegisterType<luna::GestureHandler>(uri, 0, 1, "GestureHandler");
    qmlRegisterType<InverseMouseAreaType>(uri, 0, 1, "InverseMouseArea");
    qmlRegisterType<VolumeKeys>(uri, 0, 1, "VolumeKeys");
}

void LunaNextShellPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
}
