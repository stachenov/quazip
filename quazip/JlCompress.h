#ifndef JLCOMPRESSFOLDER_H_
#define JLCOMPRESSFOLDER_H_

/*
Copyright (C) 2010 Roberto Pompermaier
Copyright (C) 2005-2016 Sergey A. Tachenov

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

#include "quazip.h"
#include "quazipfile.h"
#include "quazipfileinfo.h"
#include "quazip_qt_compat.h"
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>

/// Utility class for typical operations.
/**
  This class contains a number of useful static functions to perform
  simple operations, such as mass ZIP packing or extraction.
  */
class QUAZIP_EXPORT JlCompress {
public:
    enum CompressionStrategy
    {
        /// Storage without compression
        Storage  = 0x00,
        /// The fastest compression speed
        Fastest  = 0x81,
        /// Relatively fast compression speed
        Faster   = 0x83,
        /// Standard compression speed and ratio
        Standard = 0x85,
        /// Better compression ratio
        Better   = 0x87,
        /// The best compression ratio
        Best     = 0x89
    };

    static bool copyData(QIODevice &inFile, QIODevice &outFile);
    static QStringList extractDir(QuaZip &zip, const QString &dir);
    static QStringList getFileList(QuaZip *zip);
    static QString extractFile(QuaZip &zip, QString fileName, QString fileDest);
    static QStringList extractFiles(QuaZip &zip, const QStringList &files, const QString &dir);
    /// Compress a single file.
    /**
      \param zip Opened zip to compress the file to.
      \param fileName The full path to the source file.
      \param fileDest The full name of the file inside the archive.
      \param strategy The strategy of compression
      \return true if success, false otherwise.
      */
    static bool compressFile(QuaZip* zip, QString fileName, QString fileDest, CompressionStrategy strategy = Standard);
    /// Compress a subdirectory.
    /**
      \param parentZip Opened zip containing the parent directory.
      \param dir The full path to the directory to pack.
      \param parentDir The full path to the directory corresponding to
      the root of the ZIP.
      \param recursive Whether to pack sub-directories as well or only
      files.
      \param strategy The strategy of compression
      \return true if success, false otherwise.
      */
    static bool compressSubDir(QuaZip* parentZip, QString dir, QString parentDir, bool recursive,
                               QDir::Filters filters, CompressionStrategy strategy = Standard);
    /// Extract a single file.
    /**
      \param zip The opened zip archive to extract from.
      \param fileName The full name of the file to extract.
      \param fileDest The full path to the destination file.
      \return true if success, false otherwise.
      */
    static bool extractFile(QuaZip* zip, QString fileName, QString fileDest);
    /// Remove some files.
    /**
      \param listFile The list of files to remove.
      \return true if success, false otherwise.
      */
    static bool removeFile(QStringList listFile);

    /// Compress a single file.
    /**
      \param fileCompressed The name of the archive.
      \param file The file to compress.
      \param strategy The strategy of compression
      \return true if success, false otherwise.
      */
    static bool compressFile(QString fileCompressed, QString file, CompressionStrategy strategy = Standard);
    /// Compress a list of files.
    /**
      \param fileCompressed The name of the archive.
      \param files The file list to compress.
      \param strategy The strategy of compression
      \return true if success, false otherwise.
      */
    static bool compressFiles(QString fileCompressed, QStringList files, CompressionStrategy strategy = Standard);
    /// Compress a whole directory.
    /**
      Does not compress hidden files. See compressDir(QString, QString, bool, QDir::Filters).

      \param fileCompressed The name of the archive.
      \param dir The directory to compress.
      \param recursive Whether to pack the subdirectories as well, or
      just regular files.
      \param strategy The strategy of compression
      \return true if success, false otherwise.
      */
    static bool compressDir(QString fileCompressed, QString dir, bool recursive = true,
                            CompressionStrategy strategy = Standard);
    /**
     * @brief Compress a whole directory.
     *
     * Unless filters are specified explicitly, packs
     * only regular non-hidden files (and subdirs, if @c recursive is true).
     * If filters are specified, they are OR-combined with
     * <tt>%QDir::AllDirs|%QDir::NoDotAndDotDot</tt> when searching for dirs
     * and with <tt>QDir::Files</tt> when searching for files.
     *
     * @param fileCompressed path to the resulting archive
     * @param dir path to the directory being compressed
     * @param recursive if true, then the subdirectories are packed as well
     * @param filters what to pack, filters are applied both when searching
     * for subdirs (if packing recursively) and when looking for files to pack
     * @param strategy The strategy of compression
     * @return true on success, false otherwise
     */
    static bool compressDir(QString fileCompressed, QString dir, bool recursive, QDir::Filters filters,
                            CompressionStrategy strategy = Standard);

    /// Extract a single file.
    /**
      \param fileCompressed The name of the archive.
      \param fileName The file to extract.
      \param fileDest The destination file, assumed to be identical to
      \a file if left empty.
      \return The list of the full paths of the files extracted, empty on failure.
      */
    static QString extractFile(QString fileCompressed, QString fileName, QString fileDest = QString());
    /// Extract a list of files.
    /**
      \param fileCompressed The name of the archive.
      \param files The file list to extract.
      \param dir The directory to put the files to, the current
      directory if left empty.
      \return The list of the full paths of the files extracted, empty on failure.
      */
    static QStringList extractFiles(QString fileCompressed, QStringList files, QString dir = QString());
    /// Extract a whole archive.
    /**
      \param fileCompressed The name of the archive.
      \param dir The directory to extract to, the current directory if
      left empty.
      \return The list of the full paths of the files extracted, empty on failure.
      */
    static QStringList extractDir(QString fileCompressed, QString dir = QString());
    /// Extract a whole archive.
    /**
      \param fileCompressed The name of the archive.
      \param fileNameCodec The codec to use for file names.
      \param dir The directory to extract to, the current directory if
      left empty.
      \return The list of the full paths of the files extracted, empty on failure.
      */
    static QStringList extractDir(QString fileCompressed, QTextCodec* fileNameCodec, QString dir = QString());
    /// Get the file list.
    /**
      \return The list of the files in the archive, or, more precisely, the
      list of the entries, including both files and directories if they
      are present separately.
      */
    static QStringList getFileList(QString fileCompressed);
    /// Extract a single file.
    /**
      \param ioDevice pointer to device with compressed data.
      \param fileName The file to extract.
      \param fileDest The destination file, assumed to be identical to
      \a file if left empty.
      \return The list of the full paths of the files extracted, empty on failure.
      */
    static QString extractFile(QIODevice *ioDevice, QString fileName, QString fileDest = QString());
    /// Extract a list of files.
    /**
      \param ioDevice pointer to device with compressed data.
      \param files The file list to extract.
      \param dir The directory to put the files to, the current
      directory if left empty.
      \return The list of the full paths of the files extracted, empty on failure.
      */
    static QStringList extractFiles(QIODevice *ioDevice, QStringList files, QString dir = QString());
    /// Extract a whole archive.
    /**
      \param ioDevice pointer to device with compressed data.
      \param dir The directory to extract to, the current directory if
      left empty.
      \return The list of the full paths of the files extracted, empty on failure.
      */
    static QStringList extractDir(QIODevice *ioDevice, QString dir = QString());
    /// Extract a whole archive.
    /**
      \param ioDevice pointer to device with compressed data.
      \param fileNameCodec The codec to use for file names.
      \param dir The directory to extract to, the current directory if
      left empty.
      \return The list of the full paths of the files extracted, empty on failure.
      */
    static QStringList extractDir(QIODevice* ioDevice, QTextCodec* fileNameCodec, QString dir = QString());
    /// Get the file list.
    /**
      \return The list of the files in the archive, or, more precisely, the
      list of the entries, including both files and directories if they
      are present separately.
      */
    static QStringList getFileList(QIODevice *ioDevice); 
};

#endif /* JLCOMPRESSFOLDER_H_ */
