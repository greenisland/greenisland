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

#include <GreenIsland/QtWaylandCompositor/QWaylandCompositor>

#include "serverlogging_p.h"
#include "taskmanager.h"
#include "taskmanager_p.h"

namespace GreenIsland {

namespace Server {

/*
 * TaskManagerPrivate
 */

TaskManagerPrivate::TaskManagerPrivate()
    : QWaylandExtensionTemplatePrivate()
    , QtWaylandServer::greenisland_windows()
    , initialized(false)
    , boundResource(Q_NULLPTR)
{
}

TaskManagerPrivate::~TaskManagerPrivate()
{
    while (!map.isEmpty()) {
        TaskItem *task = map.take(map.firstKey());
        TaskItemPrivate::get(task)->send_unmapped();
        delete task;
    }
}

void TaskManagerPrivate::windowMapped(ClientWindow *window)
{
    Q_Q(TaskManager);

    // Can't continue if it's not initialized
    if (!initialized)
        return;

    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(q->extensionContainer());

    TaskItem *task = new TaskItem();
    TaskItemPrivate *dTask = TaskItemPrivate::get(task);
    task->initialize();
    dTask->init(compositor->display(), 1);

    map.insert(window, task);

    send_window_mapped(boundResource->handle, dTask->resource()->handle,
                       dTask->windowType, dTask->windowState,
                       window->title(), window->appId());
}

void TaskManagerPrivate::windowUnmapped(ClientWindow *window)
{
    TaskItem *task = map.take(window);
    TaskItemPrivate::get(task)->send_unmapped();
    delete task;
}

void TaskManagerPrivate::windows_bind_resource(Resource *resource)
{
    if (boundResource) {
        wl_resource_post_error(resource->handle, WL_DISPLAY_ERROR_INVALID_OBJECT,
                               "greenisland_windows can only be bound once");
        return;
    }

    boundResource = resource;
}

/*
 * TaskItemPrivate
 */

TaskItemPrivate::TaskItemPrivate(ClientWindow *w)
    : QWaylandExtensionTemplatePrivate()
    , QtWaylandServer::greenisland_window()
    , window(w)
{
    determineType();
    determineState();

    QObject::connect(w, &ClientWindow::typeChanged, [this] {
        determineType();
    });
    QObject::connect(w, &ClientWindow::titleChanged, [this] {
        send_title_changed(window->title());
    });
    QObject::connect(w, &ClientWindow::appIdChanged, [this] {
        send_title_changed(window->appId());
    });
    QObject::connect(w, &ClientWindow::activeChanged, [this] {
        determineState();
        send_state_changed(windowState);
    });
    QObject::connect(w, &ClientWindow::minimizedChanged, [this] {
        determineState();
        send_state_changed(windowState);
    });
    QObject::connect(w, &ClientWindow::maximizedChanged, [this] {
        determineState();
        send_state_changed(windowState);
    });
    QObject::connect(w, &ClientWindow::fullScreenChanged, [this] {
        determineState();
        send_state_changed(windowState);
    });
}

void TaskItemPrivate::determineType()
{
    switch (window->type()) {
    case ClientWindow::Popup:
        windowType = QtWaylandServer::greenisland_windows::type_popup;
        break;
    case ClientWindow::Transient:
        windowType = QtWaylandServer::greenisland_windows::type_transient;
        break;
    default:
        windowType = QtWaylandServer::greenisland_windows::type_toplevel;
        break;
    }
}

void TaskItemPrivate::determineState()
{
    uint32_t s = QtWaylandServer::greenisland_windows::state_inactive;

    if (window->isActive())
        s |= QtWaylandServer::greenisland_windows::state_active;
    if (window->isMinimized())
        s |= QtWaylandServer::greenisland_windows::state_minimized;
    if (window->isMaximized())
        s |= QtWaylandServer::greenisland_windows::state_maximized;
    if (window->isFullScreen())
        s |= QtWaylandServer::greenisland_windows::state_fullscreen;

    windowState = s;
}

/*
 * TaskManager
 */

TaskManager::TaskManager()
    : QWaylandExtensionTemplate<TaskManager>(*new TaskManagerPrivate())
{
}

TaskManager::TaskManager(QWaylandCompositor *compositor)
    : QWaylandExtensionTemplate<TaskManager>(compositor, *new TaskManagerPrivate())
{
}

void TaskManager::initialize()
{
    Q_D(TaskManager);

    QWaylandExtensionTemplate::initialize();
    QWaylandCompositor *compositor = static_cast<QWaylandCompositor *>(extensionContainer());
    if (!compositor) {
        qCWarning(gLcTaskManager) << "Failed to find QWaylandCompositor when initializing TaskManager";
        return;
    }
    d->init(compositor->display(), 1);
}

const struct wl_interface *TaskManager::interface()
{
    return TaskManagerPrivate::interface();
}

QByteArray TaskManager::interfaceName()
{
    return TaskManagerPrivate::interfaceName();
}

/*
 * TaskItem
 */

TaskItem::TaskItem()
{
}

const struct wl_interface *TaskItem::interface()
{
    return TaskItemPrivate::interface();
}

QByteArray TaskItem::interfaceName()
{
    return TaskItemPrivate::interfaceName();
}

} // namespace Server

} // namespace GreenIsland

#include "moc_taskmanager.cpp"
