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

#include <QtCore/QRect>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/qpa/qplatformscreen.h>

#include "nativescreenbackend.h"

Q_LOGGING_CATEGORY(NATIVE_BACKEND, "greenisland.screenbackend.native")

namespace GreenIsland {

NativeScreenBackend::NativeScreenBackend(Compositor *compositor, QObject *parent)
    : ScreenBackend(compositor, parent)
{
}

void NativeScreenBackend::acquireConfiguration()
{
    disconnect(qGuiApp, &QGuiApplication::screenAdded,
               this, &NativeScreenBackend::screenAdded);
    disconnect(qGuiApp, &QGuiApplication::screenRemoved,
               this, &NativeScreenBackend::screenRemoved);

    Q_FOREACH (QScreen *screen, qGuiApp->screens())
        screenAdded(screen);

    Q_EMIT configurationAcquired();

    connect(qGuiApp, &QGuiApplication::screenAdded,
            this, &NativeScreenBackend::screenAdded);
    connect(qGuiApp, &QGuiApplication::screenRemoved,
            this, &NativeScreenBackend::screenRemoved);
}

void NativeScreenBackend::screenAdded(QScreen *screen)
{
    qCDebug(NATIVE_BACKEND) << "Screen added" << screen->name() << screen->availableGeometry();

//#if QTCOMPOSITOR_VERSION >= QT_VERSION_CHECK(5, 6, 0)
#if 0
    QWaylandOutputMode *mode =
            new QWaylandOutputMode(QStringLiteral("defaultMode"),
                                   screen->availableGeometry().size(),
                                   screen->refreshRate() * 1000);

    Output *output = new Output(compositor(),
                                screen->name(),
                                QStringLiteral("Green Island"),
                                screen->name(),
                                QWaylandOutputModeList() << mode);
#else
    Output *output = new Output(compositor(),
                                screen->name(),
                                QStringLiteral("Green Island"),
                                screen->name());
#endif
    output->window()->setScreen(screen);
    output->window()->setFlags(Qt::FramelessWindowHint);
    output->setPrimary(qGuiApp->primaryScreen() == screen);
    m_screenMap.insert(screen, output);
    changeGeometry(screen);
    changePhysicalSize(screen);
    changeOrientation(screen);
    changeSubpixelAntialiasing(screen);
    Q_EMIT outputAdded(output);

    connect(screen, &QScreen::availableGeometryChanged, this,
            [this, screen](const QRect &) {
        changeGeometry(screen);
    }, Qt::DirectConnection);
    connect(screen, &QScreen::physicalSizeChanged, this,
            [this, screen](const QSizeF &) {
        changePhysicalSize(screen);
    }, Qt::DirectConnection);
    connect(screen, &QScreen::primaryOrientationChanged, this,
            [this, screen](Qt::ScreenOrientation) {
        changeOrientation(screen);
    }, Qt::DirectConnection);
}

void NativeScreenBackend::screenRemoved(QScreen *screen)
{
    qCDebug(NATIVE_BACKEND) << "Screen removed" << screen->name() << screen->availableGeometry();

    if (!m_screenMap.contains(screen))
        return;

    Output *output = m_screenMap.take(screen);
    m_outputs.removeOne(output);

    // Determine the new primary output
    QScreen *primaryScreen = qGuiApp->primaryScreen();
    if (output->isPrimary() && m_screenMap.contains(primaryScreen)) {
        Output *primaryOutput = Q_NULLPTR;

        if (primaryScreen == screen) {
            // Removed screen is still primary let's assign the primary
            // status to the first output then
            primaryOutput = m_outputs.at(0);
        } else {
            // Primary found let's use that
            primaryOutput = m_screenMap[primaryScreen];
        }

        // Unset previous primary
        Q_FOREACH (Output *o, m_outputs) {
            if (o == primaryOutput)
                continue;
            o->setPrimary(false);
        }

        // Set new primary
        primaryOutput->setPrimary(true);
        Q_EMIT primaryOutputChanged(primaryOutput);
    }

    Q_EMIT outputRemoved(output);
    output->deleteLater();
}

void NativeScreenBackend::changeGeometry(QScreen *screen)
{
    if (!m_screenMap.contains(screen))
        return;

    Output *output = m_screenMap[screen];
    output->setPosition(screen->availableGeometry().topLeft());
    output->window()->setPosition(output->position());

//#if QTCOMPOSITOR_VERSION < QT_VERSION_CHECK(5, 6, 0)
#if 1
    int refreshRate = screen->refreshRate() * 1000;
    output->setMode({ screen->availableGeometry().size(),
                      refreshRate });
#endif
}

void NativeScreenBackend::changePhysicalSize(QScreen *screen)
{
    if (!m_screenMap.contains(screen))
        return;

    Output *output = m_screenMap[screen];
    output->setPhysicalSize(screen->physicalSize().toSize());
}

void NativeScreenBackend::changeOrientation(QScreen *screen)
{
    if (!m_screenMap.contains(screen))
        return;

    Output *output = m_screenMap[screen];
    switch (screen->primaryOrientation()) {
    case Qt::PortraitOrientation:
        output->setTransform(QWaylandOutput::Transform90);
        break;
    case Qt::InvertedLandscapeOrientation:
        output->setTransform(QWaylandOutput::Transform180);
        break;
    case Qt::InvertedPortraitOrientation:
        output->setTransform(QWaylandOutput::Transform270);
        break;
    default:
        break;
    }
}

void NativeScreenBackend::changeSubpixelAntialiasing(QScreen *screen)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    if (!screen->handle())
        return;
    if (!m_screenMap.contains(screen))
        return;

    QWaylandOutput::Subpixel wlType = QWaylandOutput::SubpixelUnknown;
    QPlatformScreen::SubpixelAntialiasingType type = screen->handle()->subpixelAntialiasingTypeHint();
    switch (type) {
    case QPlatformScreen::Subpixel_None:
        wlType = QWaylandOutput::SubpixelNone;
        break;
    case QPlatformScreen::Subpixel_RGB:
        wlType = QWaylandOutput::SubpixelHorizontalRgb;
        break;
    case QPlatformScreen::Subpixel_BGR:
        wlType = QWaylandOutput::SubpixelHorizontalBgr;
        break;
    case QPlatformScreen::Subpixel_VRGB:
        wlType = QWaylandOutput::SubpixelVerticalRgb;
        break;
    case QPlatformScreen::Subpixel_VBGR:
        wlType = QWaylandOutput::SubpixelVerticalBgr;
        break;
    }

    Output *output = m_screenMap[screen];
    output->setSubpixel(wlType);
#else
    Q_UNUSED(screen)
#endif
}

}

#include "moc_nativescreenbackend.cpp"
