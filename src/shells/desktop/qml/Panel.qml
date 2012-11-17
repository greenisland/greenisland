/****************************************************************************
 * This file is part of Desktop Shell.
 *
 * Copyright (c) 2012 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

import QtQuick 2.0
import FluidCore 1.0

Item {
    // Panel height
    property real padding: 2
    property real panelRealHeight: theme.smallIconSize + (padding * 2)

    x: quickview.screenGeometry.x
    y: quickview.screenGeometry.y
    width: quickview.screenGeometry.width
    height: panelRealHeight + frame.margins.bottom

    FrameSvgItem {
        id: frame
        anchors.fill: parent
        enabledBorders: FrameSvgItem.BottomBorder
        imagePath: "widgets/panel-background"
        prefix: "north-mini"
    }

    PanelView {
        anchors {
            fill: frame
            verticalCenter: frame.verticalCenter
            bottomMargin: frame.margins.bottom
        }
    }
}