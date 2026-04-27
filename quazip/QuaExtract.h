#ifndef QUAEXTRACT_H
#define QUAEXTRACT_H

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

#include "quazip_global.h"
#include "quazip_textcodec.h"

class QIODevice;

/**
 * QuaExtract provides a fluent API for ZIP extraction operations.
 *
 * Example usage:
 * \code
 * QuaExtract()
 *     .withPassword("secret")
 *     .extractDir("archive.zip", "output/");
 * \endcode
 */
class QUAZIP_EXPORT QuaExtract {
public:
    /// Construct a QuaExtract instance.
    QuaExtract();

    /// Set the password for decryption.
    /**
     * \param password The password to use for decrypting files during extraction.
     * \return Reference to this instance for method chaining.
     */
    QuaExtract& withPassword(const QByteArray& password);

    // ==================== Extraction Methods ====================

    /// Extract a single file from an archive.
    /**
     * \param archive The archive to extract from.
     * \param fileName The file to extract from the archive.
     * \param fileDest Where to extract the file. If empty, extracts to current directory.
     * \return The full path to the extracted file, or empty string on failure.
     */
    QString extractFile(const QString& archive, const QString& fileName, const QString& fileDest = QString()) const;

    /// Extract multiple files from an archive.
    /**
     * \param archive The archive to extract from.
     * \param files List of files to extract (empty = extract all).
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractFiles(const QString& archive, const QStringList& files = QStringList(),
                             const QString& dir = QString()) const;

    /// Extract entire archive.
    /**
     * \param archive The archive to extract.
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractDir(const QString& archive, const QString& dir = QString()) const;

    /// Extract entire archive with custom codec.
    /**
     * \param archive The archive to extract.
     * \param fileNameCodec The codec to use for file names.
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractDir(const QString& archive, QuazipTextCodec* fileNameCodec, const QString& dir = QString()) const;

    // ==================== Extraction from QIODevice ====================

    /// Extract multiple files from an archive opened via QIODevice.
    /**
     * \param ioDevice The QIODevice to read the archive from.
     * \param files List of files to extract (empty = extract all).
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractFiles(QIODevice* ioDevice, const QStringList& files = QStringList(),
                             const QString& dir = QString()) const;

    /// Extract entire archive from QIODevice.
    /**
     * \param ioDevice The QIODevice to read the archive from.
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractDir(QIODevice* ioDevice, const QString& dir = QString()) const;

    /// Extract entire archive from QIODevice with custom codec.
    /**
     * \param ioDevice The QIODevice to read the archive from.
     * \param fileNameCodec The codec to use for file names.
     * \param dir Destination directory. If empty, uses current directory.
     * \return List of extracted files, or empty list on failure.
     */
    QStringList extractDir(QIODevice* ioDevice, QuazipTextCodec* fileNameCodec, const QString& dir = QString()) const;

    // ==================== Information Methods ====================

    /// Get list of files in an archive.
    /**
     * \param archive The archive to read.
     * \return List of file names in the archive, or empty list on failure.
     */
    QStringList getFileList(const QString& archive) const;

    /// Get list of files in an archive opened via QIODevice.
    /**
     * \param ioDevice The QIODevice to read the archive from.
     * \return List of file names in the archive, or empty list on failure.
     */
    QStringList getFileList(QIODevice* ioDevice) const;

private:
    QByteArray m_password;
};

#endif // QUAEXTRACT_H
