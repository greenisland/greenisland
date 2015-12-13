/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2014-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QtCore/QElapsedTimer>
#include <QtCore/private/qobject_p.h>
#include <QtGui/QScreen>

#include <GreenIsland/Platform/EglFSScreen>

#include "quickoutput.h"
#include "logging.h"
#include "screen/screenbackend.h"

namespace GreenIsland {

namespace Server {

/*
 * WindowFilter
 */

class WindowFilter : public QObject
{
public:
    WindowFilter(QObject *parent = Q_NULLPTR)
        : QObject(parent)
        , hotSpotLastTime(0)
        , hotSpotEntered(0)
    {
        quickOutput = static_cast<QuickOutput *>(parent);
        if (quickOutput) {
            connect(quickOutput, &QuickOutput::windowChanged, this, [this] {
                // Remove the event filter if it was previously installed
                if (!quickWindow.isNull()) {
                    quickWindow->removeEventFilter(this);
                    quickWindow.clear();
                }

                // Install the event filter when a window is set
                QQuickWindow *window = static_cast<QQuickWindow *>(quickOutput->window());
                if (window) {
                    quickWindow = window;
                    window->installEventFilter(this);
                    timer.restart();
                }
            });
        }
    }

    QuickOutput *quickOutput;
    QPointer<QQuickWindow> quickWindow;
    QElapsedTimer timer;
    quint64 hotSpotLastTime;
    quint64 hotSpotEntered;

protected:
    bool eventFilter(QObject *object, QEvent *event) Q_DECL_OVERRIDE
    {
        // Do not filter other objects
        if (object != quickWindow)
            return QObject::eventFilter(object, event);

        // We are only interested in mouse move events
        if (event->type() != QEvent::MouseMove)
            return false;

        // We also cannot continue without the output
        if (!quickOutput)
            return false;

        // Coordinates
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        const QPoint pt = mouseEvent->localPos().toPoint();
        const QRect r = quickOutput->geometry();

        // Hot spots are triggered after the threshold
        qint64 time = timer.elapsed();
        if (time - hotSpotLastTime >= quickOutput->hotSpotThreshold()) {
            // Determine which hotspot was triggered
            QuickOutput::HotSpot hotSpot;
            const int w = quickOutput->hotSpotSize().width();
            const int h = quickOutput->hotSpotSize().height();
            bool triggered = true;
            if (pt.x() <= r.left() + w && pt.y() <= r.top() + h)
                hotSpot = QuickOutput::HotSpot::TopLeftHotSpot;
            else if (pt.x() >= r.right() - w && pt.y() <= r.top() + h)
                hotSpot = QuickOutput::HotSpot::TopRightHotSpot;
            else if (pt.x() <= r.left() + w && pt.y() >= r.bottom() - h)
                hotSpot = QuickOutput::HotSpot::BottomLeftHotSpot;
            else if (pt.x() >= r.right() - w && pt.y() >= r.bottom() - h)
                hotSpot = QuickOutput::HotSpot::BottomRightHotSpot;
            else {
                triggered = false;
                hotSpotEntered = 0;
            }

            // Trigger an action
            if (triggered) {
                if (hotSpotEntered == 0)
                    hotSpotEntered = time;
                else if (time - hotSpotEntered < quickOutput->hotSpotPushTime()) {
                    hotSpotLastTime = time;
                    Q_EMIT quickOutput->hotSpotTriggered(hotSpot);
                }
            }
        }

        return false;
    }
};

/*
 * OutputPrivate
 */

class QuickOutputPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QuickOutput)
public:
    QuickOutputPrivate()
        : initialized(false)
        , nativeScreen(Q_NULLPTR)
        , hotSpotSize(QSize(5, 5))
        , hotSpotThreshold(1000)
        , hotSpotPushTime(50)
    {
    }

    bool initialized;
    Screen *nativeScreen;
    QSize hotSpotSize;
    quint64 hotSpotThreshold;
    quint64 hotSpotPushTime;
};

/*
 * Output
 */

QuickOutput::QuickOutput()
    : QWaylandQuickOutput()
    , d_ptr(new QuickOutputPrivate())
{
    // Filter events on the output window
    new WindowFilter(this);
}

QuickOutput::QuickOutput(QWaylandCompositor *compositor)
    : QWaylandQuickOutput(compositor, Q_NULLPTR)
    , d_ptr(new QuickOutputPrivate())
{
    // Filter events on the output window
    new WindowFilter(this);
}

Screen *QuickOutput::nativeScreen() const
{
    Q_D(const QuickOutput);
    return d->nativeScreen;
}

void QuickOutput::setNativeScreen(Screen *screen)
{
    Q_D(QuickOutput);

    if (d->nativeScreen == screen)
        return;

    if (d->initialized) {
        qCWarning(GREENISLAND_COMPOSITOR)
                << "Setting GreenIsland::Server::Output::nativeScreen "
                   "after initialization has no effect";
        return;
    }

    d->nativeScreen = screen;
    Q_EMIT nativeScreenChanged();
}

QuickOutput::PowerState QuickOutput::powerState() const
{
    Q_D(const QuickOutput);

    // Power state is supported only with native screens and our QPA
    Platform::EglFSScreen *screen = Q_NULLPTR;
    if (d->nativeScreen && d->nativeScreen->screen())
        screen = static_cast<Platform::EglFSScreen *>(
                    d->nativeScreen->screen()->handle());
    if (!screen) {
        qCWarning(GREENISLAND_COMPOSITOR)
                << "QuickOutput::powerState always returns "
                << "ON without native screens or the greenisland QPA";
        return PowerStateOn;
    }

    return static_cast<QuickOutput::PowerState>(screen->powerState());
}

void QuickOutput::setPowerState(PowerState state)
{
    Q_D(QuickOutput);

    // Power state is supported only with native screens and our QPA
    Platform::EglFSScreen *screen = Q_NULLPTR;
    if (d->nativeScreen && d->nativeScreen->screen())
        screen = static_cast<Platform::EglFSScreen *>(
                    d->nativeScreen->screen()->handle());
    if (!screen) {
        qCWarning(GREENISLAND_COMPOSITOR)
                << "Setting QuickOutput::powerState without native screens "
                << "or without the greenisland QPA has no effect";
        return;
    }

    Platform::EglFSScreen::PowerState pstate =
            static_cast<Platform::EglFSScreen::PowerState>(state);
    if (screen->powerState() != pstate) {
        screen->setPowerState(pstate);
        Q_EMIT powerStateChanged();
    }
}

QSize QuickOutput::hotSpotSize() const
{
    Q_D(const QuickOutput);
    return d->hotSpotSize;
}

void QuickOutput::setHotSpotSize(const QSize &size)
{
    Q_D(QuickOutput);

    if (d->hotSpotSize == size)
        return;

    d->hotSpotSize = size;
    Q_EMIT hotSpotSizeChanged();
}

quint64 QuickOutput::hotSpotThreshold() const
{
    Q_D(const QuickOutput);
    return d->hotSpotThreshold;
}

void QuickOutput::setHotSpotThreshold(quint64 value)
{
    Q_D(QuickOutput);

    if (d->hotSpotThreshold == value)
        return;

    d->hotSpotThreshold = value;
    Q_EMIT hotSpotThresholdChanged();
}

quint64 QuickOutput::hotSpotPushTime() const
{
    Q_D(const QuickOutput);
    return d->hotSpotPushTime;
}

void QuickOutput::setHotSpotPushTime(quint64 value)
{
    Q_D(QuickOutput);

    if (d->hotSpotThreshold == value)
        return;

    d->hotSpotPushTime = value;
    Q_EMIT hotSpotPushTimeChanged();
}

void QuickOutput::initialize()
{
    Q_D(QuickOutput);

    QWaylandQuickOutput::initialize();

    QQuickWindow *quickWindow = qobject_cast<QQuickWindow *>(window());
    if (!quickWindow) {
        qCWarning(GREENISLAND_COMPOSITOR,
                  "Could not locate QQuickWindow on initializing"
                  "GreenIsland::Server::Output %p.\n", this);
        return;
    }

    // By default windows use the primary screen, but this will make
    // the compositor fail when using the eglfs or greenisland QPA plugins
    // because they both need one window for each screen.
    // Avoid failures by setting the screen from the native screen
    // that comes from our screen manager.
    if (d->nativeScreen)
        quickWindow->setScreen(d->nativeScreen->screen());

    // We want to read contents to make a screenshot
    connect(quickWindow, &QQuickWindow::afterRendering,
            this, &QuickOutput::readContent,
            Qt::DirectConnection);

    // Set the window visible now
    quickWindow->setVisible(true);

    d->initialized = true;
}

void QuickOutput::readContent()
{
    // TODO: Update the screencaster protocol with the new API
    // and record a frame here
}

} // namespace Server

} // namespace GreenIsland

#include "moc_quickoutput.cpp"
