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

#include "windowtype.h"

namespace luna
{

unsigned int WindowType::fromString(const QString& str)
{
    if (str == "launcher")
        return WindowType::Launcher;
    else if (str == "dashboard")
        return WindowType::Dashboard;
    else if (str == "banneralert")
        return WindowType::BannerAlert;
    else if (str == "popupalert")
        return WindowType::PopupAlert;
    else if (str == "overlay")
        return WindowType::Overlay;
    else if (str == "pin")
        return WindowType::Pin;

    return WindowType::Card;
}

} // namespace luna
