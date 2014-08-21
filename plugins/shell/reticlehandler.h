/*
 * Copyright (C) 2014 Simon Busch <morphis@gravedo.de>
 * Copyright (C) 2014 Nikolay Nizov <nizovn@gmail.com>
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

#ifndef RETICLEHANDLER_H
#define RETICLEHANDLER_H

#include <QQuickItem>

namespace luna
{

class ReticleHandler : public QQuickItem
{
	Q_OBJECT
public:
	explicit ReticleHandler(QQuickItem *parent = 0);

	virtual bool eventFilter(QObject *watched, QEvent *event);

signals:
	void reticleEvent(QPoint pos);
};

} // namespace luna

#endif // RETICLEHANDLER_H
