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

#include "quazip_textcodec.h"

#include <QHash>

#ifndef QUAZIP_CAN_USE_QTEXTCODEC
static QHash<QStringConverter::Encoding,QuazipTextCodec*> *static_hash_quazip_codecs = nullptr;

class QuazipTextTextCodecCleanup
{
public:
    explicit QuazipTextTextCodecCleanup()
    {
    }
    ~QuazipTextTextCodecCleanup()
    {
        if (static_hash_quazip_codecs)
        {
            QList<QuazipTextCodec*>list_quazip_codecs = static_hash_quazip_codecs->values();
            qDeleteAll(list_quazip_codecs.begin(),list_quazip_codecs.end());
            static_hash_quazip_codecs->clear();
            delete static_hash_quazip_codecs;
            static_hash_quazip_codecs = nullptr;
        }
    }
};

Q_GLOBAL_STATIC(QuazipTextTextCodecCleanup, createQuazipTextTextCodecCleanup)
#endif

QuazipTextCodec::QuazipTextCodec()
{
}

#ifndef QUAZIP_CAN_USE_QTEXTCODEC
void QuazipTextCodec::setup()
{
    if (static_hash_quazip_codecs) return;
      (void)createQuazipTextTextCodecCleanup();

    static_hash_quazip_codecs = new QHash<QStringConverter::Encoding,QuazipTextCodec*>;
}
#endif

QuazipTextCodec *QuazipTextCodec::codecForName(const QByteArray &name)
{
    #ifndef QUAZIP_CAN_USE_QTEXTCODEC
        QuazipTextCodec::setup();
        QStringConverter::Encoding  encoding = QStringConverter::Utf8;

        std::optional<QStringConverter::Encoding> opt_encoding = QStringConverter::encodingForName(name);
        if (opt_encoding != std::nullopt)
        {
            encoding = opt_encoding.value();
        }
        if (static_hash_quazip_codecs->contains(encoding))
        {
            return static_hash_quazip_codecs->value(encoding);
        }

        QuazipTextCodec *codec = new QuazipTextCodec();

        codec->mEncoding = encoding;
        static_hash_quazip_codecs->insert(encoding,codec);
        return codec;

    #else

    return (QuazipTextCodec*) QTextCodec::codecForName(name);

    #endif
}

QuazipTextCodec *QuazipTextCodec::codecForLocale()
{
#ifdef QUAZIP_CAN_USE_QTEXTCODEC
    return (QuazipTextCodec*) QTextCodec::codecForLocale();
#else
    QuazipTextCodec::setup();
    return QuazipTextCodec::codecForName("System");
#endif
}

QByteArray QuazipTextCodec::fromUnicode(const QString &str) const
{
#ifdef QUAZIP_CAN_USE_QTEXTCODEC
     return QTextCodec::fromUnicode(str);
#else
    auto from = QStringEncoder(mEncoding);
    return from(str);
#endif
}

QString QuazipTextCodec::toUnicode(const QByteArray &a) const
{
#ifdef QUAZIP_CAN_USE_QTEXTCODEC
       return QTextCodec::toUnicode(a);
#else
    auto to = QStringDecoder(mEncoding);
    return to(a);
#endif
}
