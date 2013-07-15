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

#ifndef LUNA_COMPOSITORWINDOW_H_
#define LUNA_COMPOSITORWINDOW_H_

#include "qwaylandsurface.h"
#include "qwaylandsurfaceitem.h"

namespace luna
{

class CompositorWindow : public QWaylandSurfaceItem
{
    Q_OBJECT
    Q_ENUMS(WindowState)
    Q_PROPERTY(WindowState state READ state WRITE setState NOTIFY stateChanged)

public:
    CompositorWindow(unsigned int id, QWaylandSurface *surface, QQuickItem *parent = 0);

    enum WindowState {
        Invisible,
        Carded,
        Maximized,
        Fullscreen
    };

    void setState(CompositorWindow::WindowState state);
    CompositorWindow::WindowState state() const;

    unsigned int windowId() const;

signals:
    void stateChanged();

private:
    WindowState mState;
    unsigned int mWindowId;
};

} // namespace luna

#endif
