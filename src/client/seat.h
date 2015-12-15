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

#ifndef WLSEAT_H
#define WLSEAT_H

#include "registry.h"

#include "qwayland-wayland.h"

namespace GreenIsland {

class WlPointer;

class WlSeat : public QtWayland::wl_seat
{
public:
    WlSeat(WlRegistry *registry, wl_compositor *compositor,
           quint32 name, quint32 version);
    ~WlSeat();

    inline quint32 version() const { return m_version; }

    inline wl_compositor *compositor() const { return m_compositor; }
    inline WlPointer *pointer() const { return m_pointer; }

protected:
    void seat_capabilities(uint32_t capabilities) Q_DECL_OVERRIDE;

private:
    quint32 m_version;
    wl_compositor *m_compositor;
    WlPointer *m_pointer;
};

}

#endif // WLSEAT_H