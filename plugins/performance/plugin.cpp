/*
 * Copyright 2014 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "plugin.h"
#include "cpuusage.h"
#include "texturefromimage.h"
#include "graphmodel.h"

#include <qqml.h>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include "renderingtimes.h"

void PerformancePlugin::registerTypeVersion(const char *uri, int major, int minor)
{
    qmlRegisterType<RenderingTimes>(uri, major, minor, "RenderingTimes");
    qmlRegisterType<CpuUsage>(uri, major, minor, "CpuUsage");
    qmlRegisterType<TextureFromImage>(uri, major, minor, "TextureFromImage");
    qmlRegisterType<GraphModel>();
}

void PerformancePlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("LunaNext.Performance"));
    registerTypeVersion(uri, 0, 1);
}

void PerformancePlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    QQmlExtensionPlugin::initializeEngine(engine, uri);
    QQmlContext* context = engine->rootContext();
    QByteArray performanceOverlay = qgetenv("PERFORMANCE_OVERLAY");
    context->setContextProperty("performanceOverlayEnabled", QVariant(!performanceOverlay.isEmpty()));
}
