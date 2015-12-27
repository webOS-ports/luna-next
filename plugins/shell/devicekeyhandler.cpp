/*
 * Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
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
#include <QKeyEvent>
#include <QDebug>

#include "devicekeyhandler.h"

namespace luna
{

DeviceKeyHandler::DeviceKeyHandler(QObject *parent) :
    QObject(parent)
{
    qApp->installEventFilter(this);
}

bool DeviceKeyHandler::eventFilter(QObject *, QEvent *event)
{
    bool filtered = false;
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        filtered = true;

        switch (keyEvent->key()) {
        case Qt::Key_Home:
            emit homePressed();
            break;
        case Qt::Key_Escape:
            emit escapePressed();
            break;
        case Qt::Key_End:
            emit endPressed();
            break;
        case Qt::Key_F6:
            emit f6Pressed();
            break;
        case Qt::Key_F7:
            emit f7Pressed();
            break;
        case Qt::Key_F8:
            emit f8Pressed();
            break;
        case Qt::Key_F9:
            emit f9Pressed();
            break;
        default:
            filtered = false;
            break;
        }
    }

    return filtered;
}

} // namespace luna
