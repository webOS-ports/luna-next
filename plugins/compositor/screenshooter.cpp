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

#include <QQuickWindow>
#include <QImage>
#include <QDir>
#include <QStandardPaths>
#include <QDateTime>
#include <QDebug>

#include "screenshooter.h"

namespace luna
{

ScreenShooter::ScreenShooter(QQuickItem *parent) :
    QQuickItem(parent)
{
}

QString ScreenShooter::capture(QString path) const
{
    QQuickWindow *parentWindow = window();
    QImage image = parentWindow->grabWindow();
    QString outputPath = QLatin1String("/media/internal/screencaptures/");

    if (!path.isEmpty()) {
        if (path.startsWith("/"))
            outputPath = path;
        else
            outputPath += path;
    }
    else {
        outputPath += QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".png";
    }

    qDebug() << "Saving screenshot at" << outputPath;

    bool saved = image.save(outputPath);
    if (!saved) {
        qDebug() << "Failed to save screenshot";
        outputPath = "";
    }

    return outputPath;
}

} // namespace luna
