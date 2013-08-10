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

import QtQuick 2.0
import LunaNext 0.1

Item {
    id: container

    // anchors.fill: parent
    z: 1000

    Reticle {
        id: reticle
    }

    MouseArea {
        enabled: Settings.showReticle
        propagateComposedEvents: true

        anchors.fill: parent

        onPressed: {
            reticle.startAt(Qt.point(mouseX, mouseY));
            mouse.accepted = false;
        }

        /* We need to forward all not needed mouse events to the object lower in the
         * visual stack */
        onClicked: mouse.accepted = false
        onExited: mouse.accepted = false
        onPositionChanged: mouse.accepted = false
        onPressAndHold: mouse.accepted = false
        onReleased: mouse.accepted = false
        onWheel: mouse.accepted = false
    }
}
