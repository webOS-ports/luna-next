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


#include <QGuiApplication>
#include <QRect>
#include <QScreen>
#include <QDir>

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

#include <systemd/sd-daemon.h>
#include <Settings.h>

#include "compositor.h"

#define XDG_RUNTIME_DIR_DEFAULT "/tmp/luna-session"

int main(int argc, char *argv[])
{
    if (!qgetenv("DISPLAY").isEmpty())
        setenv("QT_QPA_PLATFORM", "xcb", 0);
    else {
        setenv("EGL_PLATFORM", "fbdev", 0);
        setenv("QT_QPA_PLATFORM", "eglfs", 0);
        setenv("QT_COMPOSITOR_NEGATE_INVERTED_Y", "1", 0);
        setenv("QT_QPA_EGLFS_HIDECURSOR", "1", 0);
    }

    // preload all settings for later use
    Settings *settings = Settings::LunaSettings();

    QGuiApplication app(argc, argv);

    QDir xdgRuntimeDir(XDG_RUNTIME_DIR_DEFAULT);

    // cleanup old sessions and recreate
    if (xdgRuntimeDir.exists())
        xdgRuntimeDir.removeRecursively();

    mkdir(XDG_RUNTIME_DIR_DEFAULT, 0700);
    setenv("XDG_RUNTIME_DIR", XDG_RUNTIME_DIR_DEFAULT, 0);

    luna::Compositor compositor(QUrl("qrc:///qml/main.qml"));
    compositor.setTitle(QLatin1String("LunaNext"));
    compositor.setGeometry(QRect(QPoint(0, 0), QGuiApplication::primaryScreen()->size()));
    compositor.show();

    if (app.arguments().indexOf("--systemd") >= 0)
        sd_notify(0, "READY=1");

    return app.exec();
}

#include "main.moc"
