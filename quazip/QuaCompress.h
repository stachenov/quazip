#ifndef QUACOMPRESS_H
#define QUACOMPRESS_H

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

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include "quazip_global.h"
#include "JlCompress.h"

/**
 * QuaCompress provides a fluent API for ZIP compression operations.
 *
 * Example usage:
 * \code
 * QuaCompress()
 *     .withUtf8Enabled()
 *     .withCompression(QuaCompress::Best)
 *     .compressDir("archive.zip", "mydir/");
 * \endcode
 */
class QUAZIP_EXPORT QuaCompress {
public:
    /// Compression strategy - shared with JlCompress::Options::CompressionStrategy.
    typedef JlCompress::Options::CompressionStrategy CompressionStrategy;

    // Convenience constants for compression strategies
    static constexpr CompressionStrategy Storage  = JlCompress::Options::Storage;
    static constexpr CompressionStrategy Fastest  = JlCompress::Options::Fastest;
    static constexpr CompressionStrategy Faster   = JlCompress::Options::Faster;
    static constexpr CompressionStrategy Standard = JlCompress::Options::Standard;
    static constexpr CompressionStrategy Better   = JlCompress::Options::Better;
    static constexpr CompressionStrategy Best     = JlCompress::Options::Best;
    static constexpr CompressionStrategy Default  = JlCompress::Options::Default;

    /// Construct a QuaCompress instance.
    QuaCompress();

    /// Enable or disable UTF-8 encoding for file names and comments.
    /**
     * \param enabled If true, use UTF-8 encoding. If false, use the configured
     *                codec (or system default).
     * \return Reference to this instance for method chaining.
     */
    QuaCompress& withUtf8Enabled(bool enabled = true);

    /// Set the compression strategy/level.
    /**
     * \param strategy The compression level to use.
     * \return Reference to this instance for method chaining.
     */
    QuaCompress& withStrategy(CompressionStrategy strategy);

    /// Set the date/time to use for compressed files.
    /**
     * \param dateTime The date/time to set. Default is to use current time.
     * \return Reference to this instance for method chaining.
     */
    QuaCompress& withDateTime(const QDateTime& dateTime);

    /// Set the password for encryption.
    /**
     * \param password The password to use for encrypting files during compression.
     * \return Reference to this instance for method chaining.
     */
    QuaCompress& withPassword(const QByteArray& password);

    // ==================== Compression Methods ====================

    /// Compress a single file.
    /**
     * \param newArchive The name of the archive to create.
     * \param file The file to compress.
     * \return true if successful, false otherwise.
     */
    bool compressFile(const QString& newArchive, const QString& file) const;

    /// Compress multiple files.
    /**
     * \param newArchive The name of the archive to create.
     * \param files List of files to compress.
     * \return true if successful, false otherwise.
     */
    bool compressFiles(const QString& newArchive, const QStringList& files) const;

    /// Compress a directory.
    /**
     * \param newArchive The name of the archive to create.
     * \param dir The directory to compress. If empty, uses current directory.
     * \param recursive If true, include subdirectories recursively.
     * \param filters QDir filters to apply when selecting files.
     * \return true if successful, false otherwise.
     */
    bool compressDir(const QString& newArchive, const QString& dir = QString(),
                     bool recursive = true,
                     QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot) const;

    // ==================== Add to Existing Archive ====================

    /// Add a single file to an existing archive.
    /**
     * \param existingArchive The existing archive to add to.
     * \param file The file to add.
     * \return true if successful, false otherwise.
     */
    bool addFile(const QString& existingArchive, const QString& file) const;

    /// Add multiple files to an existing archive.
    /**
     * \param existingArchive The existing archive to add to.
     * \param files List of files to add.
     * \return true if successful, false otherwise.
     *
     * \warning The UTF-8 setting must match the existing archive's encoding.
     */
    bool addFiles(const QString& existingArchive, const QStringList& files) const;

    /// Add a directory to an existing archive.
    /**
     * \param existingArchive The existing archive to add to.
     * \param dir The directory to add. If empty, uses current directory.
     * \param recursive If true, include subdirectories recursively.
     * \param filters QDir filters to apply when selecting files.
     * \return true if successful, false otherwise.
     */
    bool addDir(const QString& existingArchive, const QString& dir = QString(),
                bool recursive = true,
                QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot) const;

private:
    JlCompress::Options m_options {JlCompress::Options::Default};
};

#endif // QUACOMPRESS_H
