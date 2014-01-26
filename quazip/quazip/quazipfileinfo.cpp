/*
Copyright (C) 2005-2014 Sergey A. Tachenov

This file is part of QuaZIP.

QuaZIP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
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
