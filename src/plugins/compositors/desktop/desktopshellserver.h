/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef DESKTOPSHELLSERVER_H
#define DESKTOPSHELLSERVER_H

#include <QtCompositor/private/qwlcompositor_p.h>
#include <wayland-util.h>

#include "wayland-desktop-extension-server-protocol.h"
#include "compositor.h"

class DesktopShellServer
{
public:
    DesktopShellServer(DesktopCompositor *compositor, QtWayland::Compositor *handle);
    ~DesktopShellServer();

private:
    static void bind_func(struct wl_client *client, void *data,
                          uint32_t version, uint32_t id);

    static void destroy_resource(wl_resource *resource);

    static void set_surface(struct wl_client *client,
                            struct wl_resource *resource,
                            struct wl_resource *surface);

    static void set_geometry(struct wl_client *client,
                             struct wl_resource *resource,
                             int32_t x, int32_t y,
                             int32_t w, int32_t h);

    static const struct desktop_shell_interface shell_interface;

    DesktopCompositor *m_compositor;
    QtWayland::Compositor *m_compositorHandle;
    QList<wl_resource *> m_resources;
    QtWayland::Surface *m_surface;
};

#endif // DESKTOPSHELLSERVER_H
