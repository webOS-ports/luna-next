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

#ifndef DEVICEKEYHANDLER_H
#define DEVICEKEYHANDLER_H

#include <QObject>

namespace luna
{

class DeviceKeyHandler : public QObject
{
    Q_OBJECT
public:
    explicit DeviceKeyHandler(QObject *parent = 0);

    virtual bool eventFilter(QObject *watched, QEvent *event);

signals:
    void volumeUpPressed();
    void volumeDownPressed();
    void homePressed();
    void escapePressed();
    void endPressed();
    void f6Pressed();
    void f7Pressed();
    void f8Pressed();
    void f9Pressed();
};

} // namespace luna

#endif // DEVICEKEYHANDLER_H
