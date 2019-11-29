/*
 * Copyright (C) 2019 Christophe Chapuis <chris.chapuis@gmail.com>
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

#ifndef LUNAWEBOSSHELL_H
#define LUNAWEBOSSHELL_H

#include <QObject>
#include <QMap>
#include <QWaylandCompositorExtension>

#include "qwayland-server-webos-shell.h"

class QWaylandSurface;

namespace luna
{

class WlWebosShellSurface;
class Compositor;

class WlWebosShell : public QWaylandCompositorExtensionTemplate<WlWebosShell>, public QtWaylandServer::wl_webos_shell
{
    Q_OBJECT
public:
    WlWebosShell(QWaylandCompositor *compositor);

signals:
    void wlShellSurfaceCreated(WlWebosShellSurface *shellSurface);

private:
    void shell_get_system_pip(Resource *resource) Q_DECL_OVERRIDE;
    void shell_get_shell_surface(Resource *resource, uint32_t id, ::wl_resource *surface) Q_DECL_OVERRIDE;
};

class WlWebosShellSurface : public QWaylandCompositorExtensionTemplate<WlWebosShellSurface>, public QtWaylandServer::wl_webos_shell_surface
{
    Q_OBJECT
public:
    WlWebosShellSurface(WlWebosShell *wlShell, wl_client *client, quint32 id, QWaylandSurface *surface);
    ~WlWebosShellSurface();

    QWaylandSurface *surface();

protected:
    void shell_surface_destroy_resource(Resource *resource) Q_DECL_OVERRIDE;

    void shell_surface_set_location_hint(Resource *resource, uint32_t hint) Q_DECL_OVERRIDE;
    void shell_surface_set_state(Resource *resource, uint32_t state) Q_DECL_OVERRIDE;
    void shell_surface_set_property(Resource *resource, const QString &name, const QString &value) Q_DECL_OVERRIDE;
    void shell_surface_set_key_mask(Resource *resource, uint32_t webos_key) Q_DECL_OVERRIDE;

private:
    QWaylandSurface *m_surface;
};

} // namespace luna

#endif
