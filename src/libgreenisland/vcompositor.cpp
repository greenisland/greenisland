/****************************************************************************
 * This file is part of libGreenIsland.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QDebug>
#include <QGuiApplication>
#include <QOpenGLFunctions>
#include <QStringList>

#include <qpa/qplatformnativeinterface.h>

#include "vcompositor.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

VCompositor::VCompositor(QWindow *window)
    : QWaylandCompositor(window)
{
}

VCompositor::~VCompositor()
{
    closeShell();
}

void VCompositor::showGraphicsInfo()
{
    const char *str;

    QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
    if (nativeInterface) {
        EGLDisplay display = nativeInterface->nativeResourceForWindow("EglDisplay", window());
        if (display) {
            str = eglQueryString(display, EGL_VERSION);
            qDebug("EGL version: %s", str ? str : "(null)");

            str = eglQueryString(display, EGL_VENDOR);
            qDebug("EGL vendor: %s", str ? str : "(null)");

            str = eglQueryString(display, EGL_CLIENT_APIS);
            qDebug("EGL client APIs: %s", str ? str : "(null)");

            str = eglQueryString(display, EGL_EXTENSIONS);
            logExtensions(QStringLiteral("EGL extensions:"),
                          str ? QString(str) : QStringLiteral("(null)"));
        }
    }

    str = (char *)glGetString(GL_VERSION);
    qDebug("GL version: %s", str ? str : "(null)");

    str = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
    qDebug("GLSL version: %s", str ? str : "(null)");

    str = (char *)glGetString(GL_VENDOR);
    qDebug("GL vendor: %s", str ? str : "(null)");

    str = (char *)glGetString(GL_RENDERER);
    qDebug("GL renderer: %s", str ? str : "(null)");

    str = (char *)glGetString(GL_EXTENSIONS);
    logExtensions(QStringLiteral("GL extensions:"),
                  str ? QString(str) : QStringLiteral("(null)"));
}

void VCompositor::runShell()
{
}

void VCompositor::closeShell()
{
}

void VCompositor::logExtensions(const QString &label, const QString &extensions)
{
    QString msg = label;
    QStringList parts = extensions.split(QLatin1Char(' '));

    for (int i = 0; i < parts.size(); i++) {
        QString part = parts.at(i);

        if (msg.size() + part.size() + 15 > 78)
            msg += QLatin1Char('\n') + QStringLiteral("               ") + part;
        else
            msg += QStringLiteral(" ") + part;
    }

    qDebug() << qPrintable(msg.trimmed());
}
