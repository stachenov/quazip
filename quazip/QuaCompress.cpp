/*
Copyright (C) 2025 cen1

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

#include "QuaCompress.h"
#include "JlCompress.h"

// Define static constexpr members for C++14 compatibility (Qt5)
constexpr QuaCompress::CompressionStrategy QuaCompress::Storage;
constexpr QuaCompress::CompressionStrategy QuaCompress::Fastest;
constexpr QuaCompress::CompressionStrategy QuaCompress::Faster;
constexpr QuaCompress::CompressionStrategy QuaCompress::Standard;
constexpr QuaCompress::CompressionStrategy QuaCompress::Better;
constexpr QuaCompress::CompressionStrategy QuaCompress::Best;
constexpr QuaCompress::CompressionStrategy QuaCompress::Default;

QuaCompress::QuaCompress()
{
}

QuaCompress& QuaCompress::withUtf8Enabled(bool enabled)
{
    m_options.setUtf8Enabled(enabled);
    return *this;
}

QuaCompress& QuaCompress::withStrategy(CompressionStrategy strategy)
{
    m_options.setCompressionStrategy(strategy);
    return *this;
}

QuaCompress& QuaCompress::withDateTime(const QDateTime& dateTime)
{
    m_options.setDateTime(dateTime);
    return *this;
}

QuaCompress& QuaCompress::withPassword(const QByteArray& password)
{
    m_options.setPassword(password);
    return *this;
}

bool QuaCompress::compressFile(const QString& newArchive, const QString& file) const
{
    return JlCompress::compressFile(newArchive, file, m_options);
}

bool QuaCompress::compressFiles(const QString& newArchive, const QStringList& files) const
{
    return JlCompress::compressFiles(newArchive, files, m_options);
}

bool QuaCompress::compressDir(const QString& newArchive, const QString& dir, const bool recursive, const QDir::Filters filters) const
{
    return JlCompress::compressDir(newArchive, dir, recursive, filters, m_options);
}

bool QuaCompress::addFile(const QString& existingArchive, const QString& file) const
{
    return JlCompress::addFile(existingArchive, file, m_options);
}

bool QuaCompress::addFiles(const QString& existingArchive, const QStringList& files) const
{
    return JlCompress::addFiles(existingArchive, files, m_options);
}

bool QuaCompress::addDir(const QString& existingArchive, const QString& dir, const bool recursive, const QDir::Filters filters) const
{
    return JlCompress::addDir(existingArchive, dir, recursive, filters, m_options);
}
