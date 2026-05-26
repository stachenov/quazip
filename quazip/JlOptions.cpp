/*
Copyright (C) 2026 cen1

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

Original ZIP package is copyrighted by Gilles Vollant and contributors,
see quazip/(un)zip.h files for details. Basically it's the zlib license.
*/

#include "JlCompress.h"
#include <QSharedData>

struct JlCompress::Options::JlOptions : public QSharedData {
    // If set, used as last modified on file inside the archive.
    // If compressing a directory, used for all files.
    QDateTime m_dateTime;

    JlCompress::Options::CompressionStrategy m_compressionStrategy{JlCompress::Options::Default};

    /* Enables UTF-8 support for filenames and comments.
     *
     * For methods that create new archives (compressFile, compressFiles, compressDir):
     *   - This flag determines the encoding for all files in the new archive.
     *
     * For methods that add to existing archives (addFile, addFiles, addDir):
     *   - This flag MUST match the encoding already used in the existing archive.
     *   - QuaZip does not auto-detect the existing encoding.
     *   - Mismatched encoding will create an inconsistent archive.
     *
     * For methods that receive QuaZip* zip as an argument:
     *   - This flag is ignored (zip is already opened).
     *   - You must call setUtf8Enabled() on the QuaZip object before open().
     * */
    bool m_utf8Enabled {};

    /* Password for encryption/decryption.
     *
     * If set during compression, the files will be encrypted with this password.
     * If set during extraction, this password will be used to decrypt the files.
     * Empty password means no encryption/decryption.
     * */
    QByteArray m_password;
};

JlCompress::Options::Options(const CompressionStrategy& strategy)
    : d(new JlOptions) // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
{
    d->m_compressionStrategy = strategy;
}

JlCompress::Options::Options(const QDateTime& dateTime, const CompressionStrategy& strategy,
                             bool utf8Enabled, const QByteArray& password)
    : d(new JlOptions) // NOLINT(clang-analyzer-cplusplus.NewDeleteLeaks)
{
    d->m_dateTime = dateTime;
    d->m_compressionStrategy = strategy;
    d->m_utf8Enabled = utf8Enabled;
    d->m_password = password;
}

JlCompress::Options::Options(const Options& other) noexcept = default;
JlCompress::Options& JlCompress::Options::operator=(const Options& other) noexcept = default;
JlCompress::Options::~Options() = default;

QDateTime JlCompress::Options::getDateTime() const {
    return d->m_dateTime;
}

void JlCompress::Options::setDateTime(const QDateTime &dateTime) {
    d->m_dateTime = dateTime;
}

JlCompress::Options::CompressionStrategy JlCompress::Options::getCompressionStrategy() const {
    return d->m_compressionStrategy;
}

int JlCompress::Options::getCompressionMethod() const {
    return d->m_compressionStrategy != Default ? d->m_compressionStrategy >> 4 : Z_DEFLATED;
}

int JlCompress::Options::getCompressionLevel() const {
    return d->m_compressionStrategy != Default ? d->m_compressionStrategy & 0x0f : Z_DEFAULT_COMPRESSION;
}

void JlCompress::Options::setCompressionStrategy(const CompressionStrategy &strategy) {
    d->m_compressionStrategy = strategy;
}

bool JlCompress::Options::getUtf8Enabled() const {
    return d->m_utf8Enabled;
}

void JlCompress::Options::setUtf8Enabled(bool utf8Enabled) {
    d->m_utf8Enabled = utf8Enabled;
}

QByteArray JlCompress::Options::getPassword() const {
    return d->m_password;
}

void JlCompress::Options::setPassword(const QByteArray& password) {
    d->m_password = password;
}
