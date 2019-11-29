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

#include <sys/time.h>
#if 0
#include <grp.h>
#endif

#include <QWaylandSurface>
#include <QWaylandCompositor>

#include "webos_shell.h"
#include "compositor.h"

namespace luna
{

WlWebosShell::WlWebosShell(QWaylandCompositor *compositor)
    : QWaylandCompositorExtensionTemplate(compositor)
{
}

void WlWebosShell::shell_get_system_pip(Resource *resource)
{
}

void WlWebosShell::shell_get_shell_surface(Resource *resource, uint32_t id, ::wl_resource *surface)
{
    QWaylandSurface *pWlSurface = QWaylandSurface::fromResource(surface);
    /*
     * useless for a minimal implementation
    QWaylandView *pView = pWlSurface->primaryView();
    CompositorWindow *pWindow = static_cast<CompositorWindow*>(pView->renderObject());
    */
    WlWebosShellSurface *pNewShellSurface = new WlWebosShellSurface(this, resource->client(), id, pWlSurface);

    Q_EMIT wlShellSurfaceCreated(pNewShellSurface);
}

WlWebosShellSurface::WlWebosShellSurface(WlWebosShell *wlShell, wl_client *client, quint32 id, QWaylandSurface *surface)
                : QWaylandCompositorExtensionTemplate(wlShell)
                , QtWaylandServer::wl_webos_shell_surface(client, id, 1)
                , m_surface(surface)
{
}

WlWebosShellSurface::~WlWebosShellSurface()
{
}

QWaylandSurface *WlWebosShellSurface::surface()
{
    return m_surface;
}

void WlWebosShellSurface::shell_surface_destroy_resource(Resource *resource)
{
    Q_UNUSED(resource)
    delete this;
}

void WlWebosShellSurface::shell_surface_set_location_hint(Resource *resource, uint32_t hint)
{
}

void WlWebosShellSurface::shell_surface_set_state(Resource *resource, uint32_t state)
{
}

void WlWebosShellSurface::shell_surface_set_property(Resource *resource, const QString &name, const QString &value)
{
}

void WlWebosShellSurface::shell_surface_set_key_mask(Resource *resource, uint32_t webos_key)
{
}


} // namespace luna
