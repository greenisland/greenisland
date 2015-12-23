/****************************************************************************
 * This file is part of Green Island.
 *
 * Copyright (C) 2015 Pier Luigi Fiorini
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

#ifndef GREENISLANDCLIENT_BUFFER_H
#define GREENISLANDCLIENT_BUFFER_H

#include <QtCore/QObject>

#include <GreenIsland/client/greenislandclient_export.h>

namespace GreenIsland {

namespace Client {

class BufferPrivate;
class ShmPool;
class ShmPoolPrivate;

class GREENISLANDCLIENT_EXPORT Buffer : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(Buffer)
    Q_PROPERTY(uchar *address READ address CONSTANT)
    Q_PROPERTY(Format format READ format CONSTANT)
    Q_PROPERTY(QSize size READ size CONSTANT)
    Q_PROPERTY(quint32 stride READ stride CONSTANT)
    Q_PROPERTY(bool released READ isReleased WRITE setReleased NOTIFY releasedChanged)
    Q_PROPERTY(bool used READ isUsed WRITE setUsed NOTIFY usedChanged)
public:
    enum Format {
        Format_ARGB32,
        Format_RGB32
    };
    Q_ENUM(Format)

    Buffer(QObject *parent = Q_NULLPTR);

    uchar *address() const;
    Format format() const;
    QSize size() const;
    qint32 stride() const;

    bool isReleased() const;
    void setReleased(bool released);

    bool isUsed() const;
    void setUsed(bool used);

    void copy(const void *source);

Q_SIGNALS:
    void releasedChanged();
    void usedChanged();

private:
    explicit Buffer(ShmPool *pool, const QSize &size, qint32 stride, qint32 offset, Format format);

    friend class ShmPoolPrivate;
};

typedef QWeakPointer<Buffer> BufferPtr;
typedef QSharedPointer<Buffer> BufferSharedPtr;

} // namespace Client

} //namespace GreenIsland

#endif // GREENISLANDCLIENT_BUFFER_H