/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
 * Copyright (C) 2015 The Qt Company Ltd.
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL213$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1, or version 3.
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

#ifndef GREENISLAND_EGLFSVIVINTEGRATION_H
#define GREENISLAND_EGLFSVIVINTEGRATION_H

#include <GreenIsland/Platform/EGLDeviceIntegration>

namespace GreenIsland {

namespace Platform {

class EglFSVivIntegration : public EGLDeviceIntegration
{
public:
    void platformInit() Q_DECL_OVERRIDE;
    QSize screenSize() const Q_DECL_OVERRIDE;
    EGLNativeWindowType createNativeWindow(QPlatformWindow *window, const QSize &size, const QSurfaceFormat &format) Q_DECL_OVERRIDE;
    void destroyNativeWindow(EGLNativeWindowType window) Q_DECL_OVERRIDE;
    void *wlDisplay() const Q_DECL_OVERRIDE;
    EGLNativeDisplayType platformDisplay() const Q_DECL_OVERRIDE;

private:
    QSize mScreenSize;
    wl_display *mWaylandDisplay;
    EGLNativeDisplayType mNativeDisplay;
};

} // namespace Platform

} // namespace GreenIsland

#endif // GREENISLAND_EGLFSVIVINTEGRATION_H
