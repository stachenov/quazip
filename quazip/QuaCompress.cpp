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

// ==================== Constructor / Destructor ====================

QuaCompress::QuaCompress()
    : m_options()  // Default Options: utf8=false, compression=Default
{
}

QuaCompress::~QuaCompress()
{
}

// ==================== Configuration Methods (Fluent API) ====================

QuaCompress& QuaCompress::withUtf8Enabled(bool enabled)
{
    m_options.setUtf8Enabled(enabled);
    return *this;
}

QuaCompress& QuaCompress::withCompression(CompressionStrategy strategy)
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

// ==================== Compression Methods ====================

bool QuaCompress::compressFile(QString newArchive, QString file)
{
    return JlCompress::compressFile(newArchive, file, m_options);
}

bool QuaCompress::compressFiles(QString newArchive, QStringList files)
{
    return JlCompress::compressFiles(newArchive, files, m_options);
}

bool QuaCompress::compressDir(QString newArchive, QString dir,
                               bool recursive, QDir::Filters filters)
{
    return JlCompress::compressDir(newArchive, dir, recursive, filters, m_options);
}

// ==================== Add to Existing Archive ====================

bool QuaCompress::addFile(QString existingArchive, QString file)
{
    return JlCompress::addFile(existingArchive, file, m_options);
}

bool QuaCompress::addFiles(QString existingArchive, QStringList files)
{
    return JlCompress::addFiles(existingArchive, files, m_options);
}

bool QuaCompress::addDir(QString existingArchive, QString dir,
                         bool recursive, QDir::Filters filters)
{
    return JlCompress::addDir(existingArchive, dir, recursive, filters, m_options);
}

// ==================== Extraction Methods ====================

QString QuaCompress::extractFile(QString archive, QString fileName, QString fileDest)
{
    // Use password from options if set
    if (!m_options.getPassword().isEmpty()) {
        return JlCompress::extractFile(archive, fileName, fileDest, m_options.getPassword());
    }
    return JlCompress::extractFile(archive, fileName, fileDest);
}

QStringList QuaCompress::extractFiles(QString archive, QStringList files, QString dir)
{
    // Use password from options if set
    if (!m_options.getPassword().isEmpty()) {
        return JlCompress::extractFiles(archive, files, dir, m_options.getPassword());
    }
    return JlCompress::extractFiles(archive, files, dir);
}

QStringList QuaCompress::extractDir(QString archive, QString dir)
{
    // Use password from options if set
    if (!m_options.getPassword().isEmpty()) {
        return JlCompress::extractDir(archive, dir, m_options.getPassword());
    }
    return JlCompress::extractDir(archive, dir);
}

QStringList QuaCompress::extractDir(QString archive, QuazipTextCodec* fileNameCodec, QString dir)
{
    return JlCompress::extractDir(archive, fileNameCodec, dir);
}

// ==================== Extraction from QIODevice ====================

QStringList QuaCompress::extractFiles(QIODevice* ioDevice, QStringList files, QString dir)
{
    return JlCompress::extractFiles(ioDevice, files, dir);
}

QStringList QuaCompress::extractDir(QIODevice* ioDevice, QString dir)
{
    return JlCompress::extractDir(ioDevice, dir);
}

QStringList QuaCompress::extractDir(QIODevice* ioDevice, QuazipTextCodec* fileNameCodec, QString dir)
{
    return JlCompress::extractDir(ioDevice, fileNameCodec, dir);
}

// ==================== Information Methods ====================

QStringList QuaCompress::getFileList(QString archive)
{
    return JlCompress::getFileList(archive);
}

QStringList QuaCompress::getFileList(QIODevice* ioDevice)
{
    return JlCompress::getFileList(ioDevice);
}
