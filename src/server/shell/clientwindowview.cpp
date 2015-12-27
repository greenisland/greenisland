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

#include <QtGui/QGuiApplication>

#include <GreenIsland/QtWaylandCompositor/QWaylandQuickShellSurfaceItem>

#include "clientwindow.h"
#include "clientwindow_p.h"
#include "clientwindowview.h"
#include "clientwindowview_p.h"
#include "serverlogging_p.h"
#include "windowmanager.h"
#include "windowmanager_p.h"
#include "extensions/quickxdgpopupitem.h"
#include "extensions/quickxdgsurfaceitem.h"

namespace GreenIsland {

namespace Server {

/*
 * ClientWindowViewPrivate
 */

void ClientWindowViewPrivate::setShellSurfaceItem(QWaylandQuickItem *item)
{
    Q_Q(ClientWindowView);

    if (shellSurfaceItem == item)
        return;

    if (shellSurfaceItem) {
        q->disconnect(shellSurfaceItem, &QWaylandQuickItem::widthChanged,
                      q, &ClientWindowView::shellSurfaceItemWidthChanged);
        q->disconnect(shellSurfaceItem, &QWaylandQuickItem::heightChanged,
                      q, &ClientWindowView::shellSurfaceItemHeightChanged);
    }

    shellSurfaceItem = item;
    if (item) {
        item->setParentItem(q);
        item->setX(0);
        item->setY(0);
        item->setZ(0);

        q->connect(item, &QWaylandQuickItem::widthChanged,
                   q, &ClientWindowView::shellSurfaceItemWidthChanged);
        q->connect(item, &QWaylandQuickItem::heightChanged,
                   q, &ClientWindowView::shellSurfaceItemHeightChanged);
    }

    Q_EMIT q->shellSurfaceItemChanged();
}

bool ClientWindowViewPrivate::mousePressEvent(QMouseEvent *event)
{
    // If the modifier is pressed we initiate a move operation
    Qt::KeyboardModifier mod = Qt::MetaModifier;
    if (QGuiApplication::queryKeyboardModifiers().testFlag(mod) && event->buttons().testFlag(Qt::LeftButton)) {
        grabberState = ClientWindowViewPrivate::MoveState;
        moveState.initialized = false;
        return true;
    }

    return false;
}

bool ClientWindowViewPrivate::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if (grabberState != ClientWindowViewPrivate::DefaultState) {
        grabberState = ClientWindowViewPrivate::DefaultState;
        return true;
    }

    return false;
}

bool ClientWindowViewPrivate::mouseMoveEvent(QMouseEvent *event)
{
    Q_Q(ClientWindowView);

    if (grabberState == ClientWindowViewPrivate::MoveState) {
        if (!moveState.initialized) {
            moveState.initialOffset = q->mapFromItem(Q_NULLPTR, event->windowPos());
            moveState.initialized = true;
        }

        if (!q->parentItem())
            return false;

        QQuickItem *moveItem = ClientWindowPrivate::get(window)->moveItem;
        QPointF parentPos = q->parentItem()->mapFromItem(Q_NULLPTR, event->windowPos());
        QPointF pos = parentPos - moveState.initialOffset;
        moveItem->setPosition(pos);

        return true;
    }

    return false;
}

/*
 * ClientWindowView
 */

ClientWindowView::ClientWindowView(QQuickItem *parent)
    : QQuickItem(*new ClientWindowViewPrivate(), parent)
{
    setAcceptHoverEvents(false);
    setAcceptedMouseButtons(Qt::AllButtons);
    setFiltersChildMouseEvents(true);
}

QWaylandOutput *ClientWindowView::output() const
{
    Q_D(const ClientWindowView);
    return d->output;
}

QWaylandQuickItem *ClientWindowView::shellSurfaceItem() const
{
    Q_D(const ClientWindowView);
    return d->shellSurfaceItem;
}

void ClientWindowView::initialize(ClientWindow *window, QWaylandOutput *output)
{
    Q_D(ClientWindowView);

    if (d->initialized) {
        qCWarning(gLcCore, "ClientWindowView is already initialized");
        return;
    }

    ClientWindowPrivate *dWindow = ClientWindowPrivate::get(window);

    // Create the shell surface item
    if (dWindow->interfaceName == QWaylandShellSurface::interfaceName()) {
        QWaylandShellSurface *shellSurface =
                QWaylandShellSurface::findIn(dWindow->surface);
        if (!shellSurface) {
            qCWarning(gLcCore, "Shell surface not found");
            return;
        }

        QWaylandQuickShellSurfaceItem *shellSurfaceItem =
                new QWaylandQuickShellSurfaceItem();
        shellSurfaceItem->setSurface(dWindow->surface);
        shellSurfaceItem->setShellSurface(shellSurface);

        d->setShellSurfaceItem(shellSurfaceItem);
        shellSurfaceItem->setMoveItem(dWindow->moveItem);
    } else if (dWindow->interfaceName == XdgSurface::interfaceName()) {
        XdgSurface *shellSurface = XdgSurface::findIn(dWindow->surface);
        if (!shellSurface) {
            qCWarning(gLcCore, "Shell surface not found");
            return;
        }

        QuickXdgSurfaceItem *shellSurfaceItem =
                new QuickXdgSurfaceItem();
        shellSurfaceItem->setSurface(dWindow->surface);
        shellSurfaceItem->setShellSurface(shellSurface);

        d->setShellSurfaceItem(shellSurfaceItem);
        shellSurfaceItem->setMoveItem(dWindow->moveItem);
    } else if (dWindow->interfaceName == XdgPopup::interfaceName()) {
        XdgPopup *shellSurface = XdgPopup::findIn(dWindow->surface);
        if (!shellSurface) {
            qCWarning(gLcCore, "Shell surface not found");
            return;
        }

        QuickXdgPopupItem *shellSurfaceItem =
                new QuickXdgPopupItem();
        shellSurfaceItem->setSurface(dWindow->surface);
        shellSurfaceItem->setShellSurface(shellSurface);

        d->setShellSurfaceItem(shellSurfaceItem);
    } else {
        qCWarning(gLcCore, "Surface implements an unknown interface");
        return;
    }

    // Save
    d->window = window;
    d->output = output;
    dWindow->views.append(this);

    // Activate window when it has focus
    connect(this, &QWaylandQuickItem::focusChanged, this, [this, d](bool focused) {
        ClientWindowPrivate::get(d->window)->setActive(focused);
    });
    connect(shellSurfaceItem(), &QWaylandQuickItem::focusChanged, this, [this, d](bool focused) {
        ClientWindowPrivate::get(d->window)->setActive(focused);
    });

    // Lock the buffer when the surface is being destroyed
    // so QtQuick has a chance to animate destruction
    connect(window->surface(), &QWaylandSurface::surfaceDestroyed, this, [this] {
        shellSurfaceItem()->view()->setBufferLock(true);
    });

    // Finish initialization
    Q_EMIT outputChanged();
    Q_EMIT window->viewsChanged();
    d->initialized = true;
}

bool ClientWindowView::childMouseEventFilter(QQuickItem *item, QEvent *event)
{
    Q_D(ClientWindowView);

    // Make sure we will filter only events for the shell surface item
    if (item != d->shellSurfaceItem)
        return false;

    // Filter mouse events
    switch (event->type()) {
    case QEvent::MouseButtonPress:
        return d->mousePressEvent(static_cast<QMouseEvent *>(event));
    case QEvent::MouseButtonRelease:
        return d->mouseReleaseEvent(static_cast<QMouseEvent *>(event));
    case QEvent::MouseMove:
        return d->mouseMoveEvent(static_cast<QMouseEvent *>(event));
    default:
        break;
    }

    // Do not filter out events we don't care about
    return false;
}

void ClientWindowView::shellSurfaceItemWidthChanged()
{
    Q_D(ClientWindowView);
    setWidth(d->shellSurfaceItem->width());
}

void ClientWindowView::shellSurfaceItemHeightChanged()
{
    Q_D(ClientWindowView);
    setHeight(d->shellSurfaceItem->height());
}

} // namespace Server

} // namespace GreenIsland

#include "moc_clientwindowview.cpp"
