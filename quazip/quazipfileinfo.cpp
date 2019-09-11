/*
Copyright (C) 2005-2014 Sergey A. Tachenov

This file is part of QuaZIP.

QuaZIP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

QuaZIP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with QuaZIP.  If not, see <http://www.gnu.org/licenses/>.

See COPYING file for the full LGPL text.

Original ZIP package is copyrighted by Gilles Vollant and contributors,
see quazip/(un)zip.h files for details. Basically it's the zlib license.
*/

#include "quazipfileinfo.h"

static QFile::Permissions permissionsFromExternalAttr(quint32 externalAttr) {
    quint32 uPerm = (externalAttr & 0xFFFF0000u) >> 16;
    QFile::Permissions perm = 0;
    if ((uPerm & 0400) != 0)
        perm |= QFile::ReadOwner;
    if ((uPerm & 0200) != 0)
        perm |= QFile::WriteOwner;
    if ((uPerm & 0100) != 0)
        perm |= QFile::ExeOwner;
    if ((uPerm & 0040) != 0)
        perm |= QFile::ReadGroup;
    if ((uPerm & 0020) != 0)
        perm |= QFile::WriteGroup;
    if ((uPerm & 0010) != 0)
        perm |= QFile::ExeGroup;
    if ((uPerm & 0004) != 0)
        perm |= QFile::ReadOther;
    if ((uPerm & 0002) != 0)
        perm |= QFile::WriteOther;
    if ((uPerm & 0001) != 0)
        perm |= QFile::ExeOther;
    return perm;

}

QFile::Permissions QuaZipFileInfo::getPermissions() const
{
    return permissionsFromExternalAttr(externalAttr);
}

QFile::Permissions QuaZipFileInfo64::getPermissions() const
{
    return permissionsFromExternalAttr(externalAttr);
}

bool QuaZipFileInfo64::toQuaZipFileInfo(QuaZipFileInfo &info) const
{
    bool noOverflow = true;
    info.name = name;
    info.versionCreated = versionCreated;
    info.versionNeeded = versionNeeded;
    info.flags = flags;
    info.method = method;
    info.dateTime = dateTime;
    info.crc = crc;
    if (compressedSize > 0xFFFFFFFFu) {
        info.compressedSize = 0xFFFFFFFFu;
        noOverflow = false;
    } else {
        info.compressedSize = compressedSize;
    }
    if (uncompressedSize > 0xFFFFFFFFu) {
        info.uncompressedSize = 0xFFFFFFFFu;
        noOverflow = false;
    } else {
        info.uncompressedSize = uncompressedSize;
    }
    info.diskNumberStart = diskNumberStart;
    info.internalAttr = internalAttr;
    info.externalAttr = externalAttr;
    info.comment = comment;
    info.extra = extra;
    return noOverflow;
}

static QDateTime getNTFSTime(const QByteArray &extra, int position,
                             int *fineTicks)
{
    QDateTime dateTime;
    for (int i = 0; i <= extra.size() - 4; ) {
        unsigned type = static_cast<unsigned>(static_cast<unsigned char>(
                                                  extra.at(i)))
                | (static_cast<unsigned>(static_cast<unsigned char>(
                                                  extra.at(i + 1))) << 8);
        i += 2;
        unsigned length = static_cast<unsigned>(static_cast<unsigned char>(
                                                  extra.at(i)))
                | (static_cast<unsigned>(static_cast<unsigned char>(
                                                  extra.at(i + 1))) << 8);
        i += 2;
        if (type == QUAZIP_EXTRA_NTFS_MAGIC && length >= 32) {
            i += 4; // reserved
            while (i <= extra.size() - 4) {
                unsigned tag = static_cast<unsigned>(
                            static_cast<unsigned char>(extra.at(i)))
                        | (static_cast<unsigned>(
                               static_cast<unsigned char>(extra.at(i + 1)))
                           << 8);
                i += 2;
                int tagsize = static_cast<unsigned>(
                            static_cast<unsigned char>(extra.at(i)))
                        | (static_cast<unsigned>(
                               static_cast<unsigned char>(extra.at(i + 1)))
                           << 8);
                i += 2;
                if (tag == QUAZIP_EXTRA_NTFS_TIME_MAGIC
                        && tagsize >= position + 8) {
                    i += position;
                    quint64 mtime = static_cast<quint64>(
                                static_cast<unsigned char>(extra.at(i)))
                        | (static_cast<quint64>(static_cast<unsigned char>(
                                                 extra.at(i + 1))) << 8)
                        | (static_cast<quint64>(static_cast<unsigned char>(
                                                 extra.at(i + 2))) << 16)
                        | (static_cast<quint64>(static_cast<unsigned char>(
                                                 extra.at(i + 3))) << 24)
                        | (static_cast<quint64>(static_cast<unsigned char>(
                                                 extra.at(i + 4))) << 32)
                        | (static_cast<quint64>(static_cast<unsigned char>(
                                                 extra.at(i + 5))) << 40)
                        | (static_cast<quint64>(static_cast<unsigned char>(
                                                 extra.at(i + 6))) << 48)
                        | (static_cast<quint64>(static_cast<unsigned char>(
                                                 extra.at(i + 7))) << 56);
                    // the NTFS time is measured from 1601 for whatever reason
                    QDateTime base(QDate(1601, 1, 1), QTime(0, 0), Qt::UTC);
                    dateTime = base.addMSecs(mtime / 10000);
                    if (fineTicks != NULL) {
                        *fineTicks = static_cast<int>(mtime % 10000);
                    }
                    i += tagsize - position;
                } else {
                    i += tagsize;
                }

            }
        } else {
            i += length;
        }
    }
    if (fineTicks != NULL && dateTime.isNull()) {
        *fineTicks = 0;
    }
    return dateTime;
}

QDateTime QuaZipFileInfo64::getNTFSmTime(int *fineTicks) const
{
    return getNTFSTime(extra, 0, fineTicks);
}

QDateTime QuaZipFileInfo64::getNTFSaTime(int *fineTicks) const
{
    return getNTFSTime(extra, 8, fineTicks);
}

QDateTime QuaZipFileInfo64::getNTFScTime(int *fineTicks) const
{
    return getNTFSTime(extra, 16, fineTicks);
}

static QDateTime getExtTime(const QByteArray &extra, int flag)
{
    QDateTime dateTime;
    const int mTimeFlag = 1;
    const int aTimeFlag = 2;
    const int cTimeFlag = 4;
    for (int i = 0; i <= extra.size() - 4; ) {
        unsigned type = static_cast<unsigned>(static_cast<unsigned char>(
                                                  extra.at(i)))
                | (static_cast<unsigned>(static_cast<unsigned char>(
                                                  extra.at(i + 1))) << 8);
        i += 2;
        unsigned length = static_cast<unsigned>(static_cast<unsigned char>(
                                                  extra.at(i)))
                | (static_cast<unsigned>(static_cast<unsigned char>(
                                                  extra.at(i + 1))) << 8);
        i += 2;
        if (type == QUAZIP_EXTRA_EXT_TIME_MAGIC && length >= 1) {
            int flags = static_cast<int>(
                        static_cast<unsigned char>(extra.at(i)));
            if ((flags & flag) == 0)
                return dateTime;
            ++i;
            int flagsRemaining = flags;
            while (i <= extra.length() - 4 && flagsRemaining != 0) {
                int nextFlag = flagsRemaining & -flagsRemaining;
                if (nextFlag == flag) {
                    qint64 time = static_cast<qint32>(
                                static_cast<unsigned char>(extra.at(i)))
                            | (static_cast<qint32>(static_cast<unsigned char>(
                                                        extra.at(i + 1))) << 8)
                            | (static_cast<qint32>(static_cast<unsigned char>(
                                                        extra.at(i + 2))) << 16)
                            | (static_cast<qint32>(static_cast<unsigned char>(
                                                        extra.at(i + 3))) << 24);
                    QDateTime base(QDate(1970, 1, 1), QTime(0, 0), Qt::UTC);
                    dateTime = base.addSecs(time);
                    return dateTime;
                } else {
                    flagsRemaining &= flagsRemaining - 1;
                    i += 4;
                }
            }
        } else {
            i += length;
        }
    }
    return dateTime;
}

QDateTime QuaZipFileInfo64::getExtModTime() const
{
    return getExtTime(extra, 1);
}

QDateTime QuaZipFileInfo64::getExtAcTime() const
{
    return getExtTime(extra, 2);
}

QDateTime QuaZipFileInfo64::getExtCrTime() const
{
    return getExtTime(extra, 4);
}
