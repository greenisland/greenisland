/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL$
 *
 * This file may be used under the terms of the GNU Lesser General Public
 * License version 2.1 or later as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPLv21 included in the packaging of
 * this file.  Please review the following information to ensure the
 * GNU Lesser General Public License version 2.1 requirements will be
 * met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
 *
 * Alternatively, this file may be used under the terms of the GNU General
 * Public License version 2.0 or later as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPLv2 included in the
 * packaging of this file.  Please review the following information to ensure
 * the GNU General Public License version 2.0 requirements will be
 * met: http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtCore/qmath.h>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "fakescreenbackend.h"
#include "screenbackend_p.h"
#include "serverlogging_p.h"

namespace GreenIsland {

namespace Server {

FakeScreenBackend::FakeScreenBackend(QObject *parent)
    : ScreenBackend(parent)
{
}

void FakeScreenBackend::setConfiguration(const QString &fileName)
{
    m_fileName = fileName;
}

void FakeScreenBackend::acquireConfiguration()
{
    qCDebug(gLcFakeScreenBackend) << "Load configuration from" << m_fileName;

    QFile file(m_fileName);
    if (!file.open(QFile::ReadOnly)) {
        qCWarning(gLcFakeScreenBackend) << "Could not open configuration file"
                                << m_fileName << "for reading";
        return;
    }

    QByteArray data = file.readAll();

    file.close();

    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qCWarning(gLcFakeScreenBackend) << "Invalid configuration file, no top-level JSON object";
        return;
    }

    bool primarySet = false;

    const QJsonObject object = doc.object();
    const QJsonArray outputs = object.value(QStringLiteral("outputs")).toArray();

    for (int i = 0; i < outputs.size(); i++) {
        const QVariantMap outputSettings = outputs.at(i).toObject().toVariantMap();
        qCDebug(gLcFakeScreenBackend) << "Output settings:" << outputSettings;

        QString name = outputSettings.value(QStringLiteral("name")).toString();
        qCDebug(gLcFakeScreenBackend) << "Output name:" << name;

        bool primary = outputSettings.value(QStringLiteral("primary")).toBool();
        qCDebug(gLcFakeScreenBackend) << "Output primary:" << primary;

        int scale = outputSettings.value(QStringLiteral("scale")).toInt();
        if (scale <= 0)
            scale = 1;
        qCDebug(gLcFakeScreenBackend) << "Scale:" << scale;

        const QVariantMap posValue = outputSettings.value(QStringLiteral("position")).toMap();
        int x = posValue.value(QStringLiteral("x")).toInt();
        int y = posValue.value(QStringLiteral("y")).toInt();
        QPoint pos(x, y);
        qCDebug(gLcFakeScreenBackend) << "Output position:" << pos;

        const QVariantMap modeValue = outputSettings.value(QStringLiteral("mode")).toMap();
        const QVariantMap sizeValue = modeValue.value(QStringLiteral("size")).toMap();
        int w = sizeValue.value(QStringLiteral("width")).toInt();
        int h = sizeValue.value(QStringLiteral("height")).toInt();
        QSize size = QSize(w, h);
        int refreshRate = modeValue.value(QStringLiteral("refreshRate")).toInt();
        qCDebug(gLcFakeScreenBackend) << "Output size:" << size;
        qCDebug(gLcFakeScreenBackend) << "Output refresh rate:" << refreshRate;

        const QVariantMap physicalSizeValue = outputSettings.value(QStringLiteral("physicalSize")).toMap();
        QSize physicalSize;
        physicalSize.setWidth(physicalSizeValue.value(QStringLiteral("width"), -1).toInt());
        physicalSize.setHeight(physicalSizeValue.value(QStringLiteral("height"), -1).toInt());
        if (!physicalSize.isValid()) {
            physicalSize.setWidth(qFloor(w * 0.26458));
            physicalSize.setHeight(qFloor(h * 0.26458));
        }
        qCDebug(gLcFakeScreenBackend) << "Physical size millimiters:" << size;

        Qt::ScreenOrientation orientation =
                static_cast<Qt::ScreenOrientation>(outputSettings.value(QStringLiteral("orientation")).toInt());
        qCDebug(gLcFakeScreenBackend) << "Output orientation:" << orientation;

        Screen *screen = new Screen(this);
        ScreenPrivate *screenPrivate = Screen::get(screen);
        screenPrivate->m_manufacturer = QStringLiteral("Green Island");
        screenPrivate->m_model = name;
        if (physicalSize.isValid())
            screenPrivate->setPhysicalSize(physicalSize);
        screenPrivate->setPosition(pos);
        screenPrivate->setSize(size);
        screenPrivate->setRefreshRate(refreshRate);
        screenPrivate->setScaleFactor(scale);

        QList<Screen::Mode> modes;
        modes.append({size, qreal(refreshRate) / 1000});
        screenPrivate->setModes(modes);

        switch (orientation) {
        case Qt::PortraitOrientation:
            screenPrivate->setTransform(QWaylandOutput::Transform90);
            break;
        case Qt::InvertedLandscapeOrientation:
            screenPrivate->setTransform(QWaylandOutput::Transform180);
            break;
        case Qt::InvertedPortraitOrientation:
            screenPrivate->setTransform(QWaylandOutput::Transform270);
            break;
        default:
            break;
        }

        ScreenBackend::get(this)->screens.append(screen);
        Q_EMIT screenAdded(screen);

        if (primary) {
            primarySet = true;
            Q_EMIT primaryScreenChanged(screen);
        }
    }

    // Automatically promote to primary the first screen if the configuration
    // doesn't explicitely assign one
    if (!primarySet)
        Q_EMIT primaryScreenChanged(ScreenBackend::get(this)->screens.at(0));
}

} // namespace Server

} // namespace GreenIsland

#include "moc_fakescreenbackend.cpp"
