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

#include "serverlogging_p.h"
#include "screenbackend.h"
#include "screenbackend_p.h"

namespace GreenIsland {

namespace Server {

/*
 * ScreenPrivate
 */

void ScreenPrivate::setPosition(const QPoint &pos)
{
    Q_Q(Screen);

    if (m_position == pos)
        return;

    m_position = pos;
    Q_EMIT q->positionChanged();
}

void ScreenPrivate::setSize(const QSize &size)
{
    Q_Q(Screen);

    if (m_size == size)
        return;

    m_size = size;
    Q_EMIT q->sizeChanged();
}

void ScreenPrivate::setRefreshRate(int refreshRate)
{
    Q_Q(Screen);

    if (m_refreshRate == refreshRate)
        return;

    m_refreshRate = refreshRate;
    Q_EMIT q->refreshRateChanged();
}

void ScreenPrivate::setPhysicalSize(const QSizeF &size)
{
    Q_Q(Screen);

    if (m_physicalSize == size)
        return;

    m_physicalSize = size;
    Q_EMIT q->physicalSizeChanged();
}

void ScreenPrivate::setSubpixel(Screen::Subpixel subpixel)
{
    Q_Q(Screen);

    if (m_subpixel == subpixel)
        return;

    m_subpixel = subpixel;
    Q_EMIT q->subpixelChanged();
}

void ScreenPrivate::setTransform(Screen::Transform transform)
{
    Q_Q(Screen);

    if (m_transform == transform)
        return;

    m_transform = transform;
    Q_EMIT q->transformChanged();
}

void ScreenPrivate::setScaleFactor(int scale)
{
    Q_Q(Screen);

    if (m_scaleFactor == scale)
        return;

    m_scaleFactor = scale;
    Q_EMIT q->scaleFactorChanged();
}

/*
 * Screen
 */

Screen::Screen(QObject *parent)
    : QObject(*new ScreenPrivate(), parent)
{
}

QScreen *Screen::screen() const
{
    Q_D(const Screen);
    return d->m_screen;
}

QString Screen::manufacturer() const
{
    Q_D(const Screen);
    return d->m_manufacturer;
}

QString Screen::model() const
{
    Q_D(const Screen);
    return d->m_model;
}

QPoint Screen::position() const
{
    Q_D(const Screen);
    return d->m_position;
}

QSize Screen::size() const
{
    Q_D(const Screen);
    return d->m_size;
}

int Screen::refreshRate() const
{
    Q_D(const Screen);
    return d->m_refreshRate;
}

QSizeF Screen::physicalSize() const
{
    Q_D(const Screen);
    return d->m_physicalSize;
}

Screen::Subpixel Screen::subpixel() const
{
    Q_D(const Screen);
    return d->m_subpixel;
}

Screen::Transform Screen::transform() const
{
    Q_D(const Screen);
    return d->m_transform;
}

int Screen::scaleFactor() const
{
    Q_D(const Screen);
    return d->m_scaleFactor;
}

/*
 * ScreenBackend
 */

ScreenBackend::ScreenBackend(QObject *parent)
    : QObject(*new ScreenBackendPrivate(), parent)
{
}

ScreenBackend::~ScreenBackend()
{
    qCDebug(gLcScreenBackend) << "Removing all outputs...";

    Q_D(ScreenBackend);

    auto it = d->screens.begin();
    while (it != d->screens.end()) {
        Screen *screen = (*it);
        it = d->screens.erase(it);
        delete screen;
    }
}

QList<Screen *> ScreenBackend::screens() const
{
    Q_D(const ScreenBackend);
    return d->screens;
}

void ScreenBackend::acquireConfiguration()
{
    qCWarning(gLcScreenBackend) << "ScreenBackend::acquireConfiguration() must be reimplemented";
}

} // namespace Server

} // namespace GreenIsland

#include "moc_screenbackend.cpp"
