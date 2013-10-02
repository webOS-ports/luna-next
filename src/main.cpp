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

#include <QDir>

#include <string.h>
#include <errno.h>
#include <error.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <linux/kd.h>
#include <linux/vt.h>

#include <systemd/sd-daemon.h>
#include <Settings.h>

#include "shellapplication.h"

#define XDG_RUNTIME_DIR_DEFAULT     "/tmp/luna-session"
#define DEFAULT_SHELL_NAME          "card"

#ifndef KDSKBMUTE
#define KDSKBMUTE    0x4B51
#endif

static int setup_tty()
{
    int ttyFd = open("/dev/tty0", O_RDWR);
    if (ttyFd < 0) {
        qWarning("Failed to open /dev/tty0");
        return -1;
    }

    // disable kernel special keys and buffering
    int err = ioctl(ttyFd, KDSKBMUTE, 1) && ioctl(ttyFd, KDSKBMUTE, K_OFF);
    if (err < 0) {
        err = ioctl(ttyFd, KDSKBMODE, K_OFF);
        if (err < 0) {
            qWarning("Failed to set K_OFF keyboard mode: %s", strerror(errno));
            close(ttyFd);
            return -1;
        }
    }

    err =  ioctl(ttyFd, KDSETMODE, KD_GRAPHICS);
    if (err < 0) {
        qWarning("Failed to set KD_GRAPHICS mode on tty: %s", strerror(errno));
        close(ttyFd);
        return -1;
    }

    close(ttyFd);

    return 0;
}

int main(int argc, char *argv[])
{
    // preload all settings for later use
    Settings::LunaSettings();

    if (Settings::LunaSettings()->hardwareType != Settings::HardwareTypeDesktop) {
        if (setup_tty() < 0)
            exit(1);
    }

    QDir xdgRuntimeDir(XDG_RUNTIME_DIR_DEFAULT);

    // cleanup old sessions and recreate
    if (xdgRuntimeDir.exists())
        xdgRuntimeDir.removeRecursively();

    mkdir(XDG_RUNTIME_DIR_DEFAULT, 0700);
    setenv("XDG_RUNTIME_DIR", XDG_RUNTIME_DIR_DEFAULT, 0);

    QString shellName = qgetenv("LUNA_NEXT_SHELL");
    if (shellName.isEmpty())
        shellName = DEFAULT_SHELL_NAME;

    luna::ShellApplication app(argc, argv);
    if (!app.create(shellName))
        exit(1);

    sd_notify(0, "READY=1");

    return app.exec();
}

#include "main.moc"
