/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef GREENISLAND_SCREENSHOOTER_P_H
#define GREENISLAND_SCREENSHOOTER_P_H

#include <QtQuick/QQuickItem>

#include <GreenIsland/QtWaylandCompositor/QWaylandOutput>
#include <GreenIsland/QtWaylandCompositor/QWaylandSurface>
#include <GreenIsland/QtWaylandCompositor/private/qwaylandextension_p.h>

#include <GreenIsland/Server/Screenshooter>
#include <GreenIsland/server/private/qwayland-server-greenisland-screenshooter.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Green Island API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

namespace GreenIsland {

namespace Server {

class GREENISLANDSERVER_EXPORT ScreenshooterPrivate
        : public QWaylandExtensionTemplatePrivate
        , public QtWaylandServer::greenisland_screenshooter
{
    Q_DECLARE_PUBLIC(Screenshooter)
public:
    ScreenshooterPrivate();

    static ScreenshooterPrivate *get(Screenshooter *screenshooter) { return screenshooter->d_func(); }

protected:
    void screenshooter_capture_output(Resource *resource,
                                      uint32_t id,
                                      struct ::wl_resource *outputResource) Q_DECL_OVERRIDE;
    void screenshooter_capture_active(Resource *resource,
                                      uint32_t id) Q_DECL_OVERRIDE;
    void screenshooter_capture_surface(Resource *resource,
                                       uint32_t id) Q_DECL_OVERRIDE;
    void screenshooter_capture_area(Resource *resource,
                                    uint32_t id) Q_DECL_OVERRIDE;
};

class GREENISLANDSERVER_EXPORT ScreenshotPrivate
        : public QWaylandExtensionTemplatePrivate
        , public QtWaylandServer::greenisland_screenshot
{
    Q_DECLARE_PUBLIC(Screenshot)
public:
    ScreenshotPrivate();

    QImage grabItem(QQuickItem *item);

    Screenshot::CaptureType captureType;
    QWaylandOutput *output;
    QWaylandSurface *selectedSurface;
    QQuickItem *selectedArea;

    static ScreenshotPrivate *get(Screenshot *screenshot) { return screenshot->d_func(); }

protected:
    void screenshot_destroy(Resource *resource) Q_DECL_OVERRIDE;
    void screenshot_record(Resource *resource,
                           struct ::wl_resource *bufferResource) Q_DECL_OVERRIDE;
};

} // namespace Server

} // namespace GreenIsland

#endif // GREENISLAND_SCREENSHOOTER_P_H
