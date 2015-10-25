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

#include "eventtype.h"

namespace luna
{

int EventType::toKey(EventType::Event event)
{
	int key = -1;

	/* See include/public/messages/SysMgrDeviceKeydefs.h of lun-sysmgr-ipc-messages */
	switch (event) {
		case CoreNaviBack:
			key = Qt::Key_Escape; // will be translated into 0x1B by QtWebEngine (see windowsKeyCodeForKeyEvent in web_event_factory.cpp)
			break;
		case CoreNaviNext:
			key = 0xE0E3;
		default:
			break;
	}

	return key;
}

} // namespace luna
