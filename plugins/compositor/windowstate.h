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

#ifndef WINDOWSTATE_H_
#define WINDOWSTATE_H_

#include <QObject>

namespace luna
{

class WindowState : public QObject
{
	Q_OBJECT
	Q_ENUMS(State)

public:

	enum State {
		Invisible,
		Carded,
		Maximized,
		Fullscreen
	};
};

} // namespace luna

#endif
