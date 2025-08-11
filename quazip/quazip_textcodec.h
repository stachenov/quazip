#ifndef QUAZIPTEXTCODEC_H
#define QUAZIPTEXTCODEC_H

/*
Copyright (C) 2024 Gregory EUSTACHE, cen1

QuazipTextCodec is a wrapper/abstraction around QTextCodec

This file is part of QuaZip.

QuaZip is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

QuaZip is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with QuaZip.  If not, see <http://www.gnu.org/licenses/>.

See COPYING file for the full LGPL text.
*/

#include <QByteArray>
#include "quazip_global.h"

#ifdef QUAZIP_CAN_USE_QTEXTCODEC
#include <QTextCodec>
typedef QTextCodec QuazipTextCodec;
#else
#include <QStringConverter>
#endif

#ifndef QUAZIP_CAN_USE_QTEXTCODEC
class QUAZIP_EXPORT QuazipTextCodec
{
  public:
    explicit QuazipTextCodec();

    QByteArray fromUnicode(const QString &str) const;
    QString toUnicode(const QByteArray &a) const;

    static QuazipTextCodec *codecForName(const QByteArray &name);
    static QuazipTextCodec *codecForLocale();
  protected:
    static void setup();
    QStringConverter::Encoding mEncoding;
};
#endif // QUAZIP_CAN_USE_QTEXTCODEC

#endif // QUAZIPTEXTCODEC_H
