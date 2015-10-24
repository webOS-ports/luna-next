/*
 * Copyright 2013 Canonical Ltd.
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
 */

#ifndef UCFONTUTILS_H
#define UCFONTUTILS_H

#include <QtCore/QObject>

namespace luna
{

class FontUtils : public QObject
{
    Q_OBJECT
public:

    explicit FontUtils(QObject *parent = 0) : QObject(parent) {}

    Q_INVOKABLE qreal sizeToPixels(const QString &size);
    Q_INVOKABLE qreal modularScale(const QString &size);
};

} // namespace luna

#endif // UCFONTUTILS_H
