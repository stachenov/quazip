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

#ifndef QUAZIP_CAN_USE_QTEXTCODEC

#include <QHash>

static QHash<QStringConverter::Encoding,QuazipTextCodec*> *static_hash_quazip_codecs = nullptr;

class QuazipTextCodecCleanup
{
public:
    explicit QuazipTextCodecCleanup()
    {
    }
    ~QuazipTextCodecCleanup()
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

Q_GLOBAL_STATIC(QuazipTextCodecCleanup, createQuazipTextCodecCleanup)

QuazipTextCodec::QuazipTextCodec()
{
}

void QuazipTextCodec::setup()
{
    if (static_hash_quazip_codecs) return;
      (void)createQuazipTextCodecCleanup();

    static_hash_quazip_codecs = new QHash<QStringConverter::Encoding,QuazipTextCodec*>;
}

QuazipTextCodec *QuazipTextCodec::codecForName(const QByteArray &name)
{
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
}

QuazipTextCodec *QuazipTextCodec::codecForLocale()
{
    QuazipTextCodec::setup();
    return QuazipTextCodec::codecForName("System");
}

QByteArray QuazipTextCodec::fromUnicode(const QString &str) const
{
    auto from = QStringEncoder(mEncoding);
    return from(str);
}

QString QuazipTextCodec::toUnicode(const QByteArray &a) const
{
    auto to = QStringDecoder(mEncoding);
    return to(a);
}

#endif
