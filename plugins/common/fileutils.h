/*
 * Copyright (C) 2013 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2016 Nikolay Nizov <nizovn@gmail.com>
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

#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QtCore/QObject>

namespace luna
{

class FileUtils : public QObject
{
    Q_OBJECT
public:

    explicit FileUtils(QObject *parent = 0) : QObject(parent) {}

    Q_INVOKABLE bool exists(const QString &resource);
};

} // namespace luna

#endif // FILEUTILS_H
