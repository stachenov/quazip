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

#include "QuaExtract.h"
#include "JlCompress.h"

QuaExtract::QuaExtract()
{
}

QuaExtract& QuaExtract::withPassword(const QByteArray& password)
{
    m_password = password;
    return *this;
}

QString QuaExtract::extractFile(const QString& archive, const QString& fileName, const QString& fileDest) const
{
    return JlCompress::extractFile(archive, fileName, fileDest, m_password);
}

QStringList QuaExtract::extractFiles(const QString& archive, const QStringList& files, const QString& dir) const
{
    return JlCompress::extractFiles(archive, files, dir, m_password);
}

QStringList QuaExtract::extractDir(const QString& archive, const QString& dir) const
{
    return JlCompress::extractDir(archive, dir, m_password);
}

QStringList QuaExtract::extractDir(const QString& archive, QuazipTextCodec* fileNameCodec, const QString& dir) const
{
    return JlCompress::extractDir(archive, fileNameCodec, dir);
}

QStringList QuaExtract::extractFiles(QIODevice* ioDevice, const QStringList& files, const QString& dir) const
{
    return JlCompress::extractFiles(ioDevice, files, dir);
}

QStringList QuaExtract::extractDir(QIODevice* ioDevice, const QString& dir) const
{
    return JlCompress::extractDir(ioDevice, dir);
}

QStringList QuaExtract::extractDir(QIODevice* ioDevice, QuazipTextCodec* fileNameCodec, const QString& dir) const
{
    return JlCompress::extractDir(ioDevice, fileNameCodec, dir);
}

QStringList QuaExtract::getFileList(const QString& archive) const
{
    return JlCompress::getFileList(archive);
}

QStringList QuaExtract::getFileList(QIODevice* ioDevice) const
{
    return JlCompress::getFileList(ioDevice);
}
