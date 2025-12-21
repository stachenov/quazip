#ifndef QUACOMPRESS_H
#define QUACOMPRESS_H

/*
Copyright (C) 2025cen1

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

Original ZIP package is copyrighted by Gilles Vollant, see
quazip/(un)zip.h files for details, basically it's zlib license.
*/

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QDir>
#include <QtCore/QDateTime>

#include "quazip_global.h"
#include "quazip_textcodec.h"
#include "JlCompress.h"

class QuaZip;
class QIODevice;

/// Utility class for compression/decompression with fluent API.
/**
 * QuaCompress provides a modern fluent interface for working with ZIP archives.
 * It offers the same functionality as JlCompress but with method chaining for
 * better readability and flexibility.
 *
 * Example usage:
 * \code
 * // Create archive with UTF-8 encoding
 * QuaCompress()
 *     .withUtf8Enabled()
 *     .withCompression(JlCompress::Options::Better)
 *     .compressDir("archive.zip", "source_dir");
 *
 * // Add files to existing archive
 * QuaCompress()
 *     .withUtf8Enabled()
 *     .addFiles("archive.zip", fileList);
 *
 * // Extract with custom codec (matches JlCompress API)
 * QuaCompress().extractDir("archive.zip",
 *                          QuazipTextCodec::codecForName("Windows-1251"),
 *                          "output_dir");
 * \endcode
 *
 * For backwards compatibility, use JlCompress static methods instead.
 *
 * \sa JlCompress
 */
class QUAZIP_EXPORT QuaCompress {
public:
    /// Compression strategy - shared with JlCompress::Options::CompressionStrategy.
    typedef JlCompress::Options::CompressionStrategy CompressionStrategy;

    /// Constructs a QuaCompress instance with default settings.
    /**
     * Default settings:
     * - UTF-8 encoding: disabled
     * - Compression strategy: JlCompress::Options::Default
     * - Date/time: current time (when compressing)
     */
    QuaCompress();

    /// Destructor.
    ~QuaCompress();

    // ==================== Configuration Methods (Fluent API) ====================

    /// Enable or disable UTF-8 encoding for file names and comments.
    /**
     * \param enabled If true, use UTF-8 encoding. If false, use the configured
     *                codec (or system default).
     * \return Reference to this instance for method chaining.
     *
     * \note When adding files to existing archives, this must match the encoding
     *       already used in the archive to avoid inconsistencies.
     */
    QuaCompress& withUtf8Enabled(bool enabled = true);

    /// Set the compression strategy/level.
    /**
     * \param strategy The compression level to use.
     * \return Reference to this instance for method chaining.
     */
    QuaCompress& withCompression(CompressionStrategy strategy);

    /// Set the date/time to use for compressed files.
    /**
     * \param dateTime The date/time to set. If invalid, current time will be used.
     * \return Reference to this instance for method chaining.
     */
    QuaCompress& withDateTime(const QDateTime& dateTime);

    /// Set the password for encryption/decryption.
    /**
     * \param password The password to use for encrypting files during compression
     *                 or decrypting files during extraction.
     * \return Reference to this instance for method chaining.
     *
     * \note For compression, this password is used to encrypt all files added to the archive.
     * \note For extraction, this password is used to decrypt password-protected files.
     */
    QuaCompress& withPassword(const QByteArray& password);


    // ==================== Compression Methods ====================

    /// Compress a single file.
    /**
     * \param fileCompressed The name of the archive to create.
     * \param file The file to compress.
     * \return true if successful, false otherwise.
     */
    bool compressFile(QString fileCompressed, QString file);

    /// Compress multiple files.
    /**
     * \param fileCompressed The name of the archive to create.
     * \param files List of files to compress.
     * \return true if successful, false otherwise.
     */
    bool compressFiles(QString fileCompressed, QStringList files);

    /// Compress a directory.
    /**
     * \param fileCompressed The name of the archive to create.
     * \param dir The directory to compress. If empty, uses current directory.
     * \param recursive If true, include subdirectories recursively.
     * \param filters QDir filters to apply when selecting files.
     * \return true if successful, false otherwise.
     */
    bool compressDir(QString fileCompressed, QString dir = QString(),
                     bool recursive = true,
                     QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot);

    // ==================== Add to Existing Archive ====================

    /// Add a single file to an existing archive.
    /**
     * \param fileCompressed The existing archive to add to.
     * \param file The file to add.
     * \return true if successful, false otherwise.
     *
     * \warning The UTF-8 setting must match the existing archive's encoding.
     */
    bool addFile(QString fileCompressed, QString file);

    /// Add multiple files to an existing archive.
    /**
     * \param fileCompressed The existing archive to add to.
     * \param files List of files to add.
     * \return true if successful, false otherwise.
     *
     * \warning The UTF-8 setting must match the existing archive's encoding.
     */
    bool addFiles(QString fileCompressed, QStringList files);

    /// Add a directory to an existing archive.
    /**
     * \param fileCompressed The existing archive to add to.
     * \param dir The directory to add. If empty, uses current directory.
     * \param recursive If true, include subdirectories recursively.
     * \param filters QDir filters to apply when selecting files.
     * \return true if successful, false otherwise.
     *
     * \warning The UTF-8 setting must match the existing archive's encoding.
     */
    bool addDir(QString fileCompressed, QString dir = QString(),
                bool recursive = true,
                QDir::Filters filters = QDir::AllEntries | QDir::NoDotAndDotDot);

    // ==================== Extraction Methods ====================

    /// Extract a single file from an archive.
    /**
     * \param fileCompressed The archive to extract from.
     * \param fileName The file to extract from the archive.
     * \param fileDest Where to extract the file. If empty, extracts to current directory.
     * \return The full path to the extracted file, or empty string on failure.
     */
    QString extractFile(QString fileCompressed, QString fileName, QString fileDest = QString());

    /// Extract multiple files from an archive.
    /**
     * \param fileCompressed The archive to extract from.
     * \param files List of files to extract (empty = extract all).
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractFiles(QString fileCompressed, QStringList files = QStringList(),
                             QString dir = QString());

    /// Extract entire archive.
    /**
     * \param fileCompressed The archive to extract.
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractDir(QString fileCompressed, QString dir = QString());

    /// Extract entire archive with custom codec.
    /**
     * \param fileCompressed The archive to extract.
     * \param fileNameCodec The codec to use for file names.
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractDir(QString fileCompressed, QuazipTextCodec* fileNameCodec, QString dir = QString());

    // ==================== Extraction with Password ====================

    /// Extract a single file from a password-protected archive.
    /**
     * \param fileCompressed The archive to extract from.
     * \param fileName The file to extract from the archive.
     * \param fileDest Where to extract the file. If empty, extracts to current directory.
     * \param password The password for decryption.
     * \return The full path to the extracted file, or empty string on failure.
     */
    QString extractFile(QString fileCompressed, QString fileName, QString fileDest, const QByteArray& password);

    /// Extract multiple files from a password-protected archive.
    /**
     * \param fileCompressed The archive to extract from.
     * \param files List of files to extract.
     * \param dir Destination directory. If empty, uses current directory.
     * \param password The password for decryption.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractFiles(QString fileCompressed, QStringList files, QString dir, const QByteArray& password);

    /// Extract entire password-protected archive.
    /**
     * \param fileCompressed The archive to extract.
     * \param dir Destination directory. If empty, uses current directory.
     * \param password The password for decryption.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractDir(QString fileCompressed, QString dir, const QByteArray& password);

    // ==================== Extraction from QIODevice ====================

    /// Extract multiple files from an archive opened via QIODevice.
    /**
     * \param ioDevice The QIODevice to read the archive from.
     * \param files List of files to extract (empty = extract all).
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractFiles(QIODevice* ioDevice, QStringList files = QStringList(),
                             QString dir = QString());

    /// Extract entire archive from QIODevice.
    /**
     * \param ioDevice The QIODevice to read the archive from.
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractDir(QIODevice* ioDevice, QString dir = QString());

    /// Extract entire archive from QIODevice with custom codec.
    /**
     * \param ioDevice The QIODevice to read the archive from.
     * \param fileNameCodec The codec to use for file names.
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractDir(QIODevice* ioDevice, QuazipTextCodec* fileNameCodec, QString dir = QString());

    // ==================== Information Methods ====================

    /// Get list of files in an archive.
    /**
     * \param fileCompressed The archive to read.
     * \return List of file names in the archive, or empty list on failure.
     */
    QStringList getFileList(QString fileCompressed);

    /// Get list of files in an archive opened via QIODevice.
    /**
     * \param ioDevice The QIODevice to read the archive from.
     * \return List of file names in the archive, or empty list on failure.
     */
    QStringList getFileList(QIODevice* ioDevice);

private:
    // Use JlCompress::Options to store configuration (avoid duplication)
    JlCompress::Options m_options;

    // Disable copy
    QuaCompress(const QuaCompress&) = delete;
    QuaCompress& operator=(const QuaCompress&) = delete;
};

#endif // QUACOMPRESS_H
