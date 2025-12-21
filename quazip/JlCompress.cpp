/*
Copyright (C) 2010 Roberto Pompermaier
Copyright (C) 2005-2014 Sergey A. Tachenov

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
#include <memory>

bool JlCompress::copyData(QIODevice &inFile, QIODevice &outFile)
{
    while (!inFile.atEnd()) {
        char buf[4096];
        qint64 readLen = inFile.read(buf, 4096);
        if (readLen <= 0)
            return false;
        if (outFile.write(buf, readLen) != readLen)
            return false;
    }
    return true;
}

bool JlCompress::compressFile(QuaZip* zip, QString fileName, QString fileDest) {
  return compressFile(zip, fileName, fileDest, Options());
}

bool JlCompress::compressFile(QuaZip* zip, QString fileName, QString fileDest, const Options& options) {
    // zip: object where to add the file
    // fileName: real file name
    // fileDest: file name inside the zip object

    if (!zip) return false;
    if (zip->getMode()!=QuaZip::mdCreate &&
        zip->getMode()!=QuaZip::mdAppend &&
        zip->getMode()!=QuaZip::mdAdd) return false;

    QuaZipFile outFile(zip);
    const char *password = options.getPassword().isEmpty() ? nullptr : options.getPassword().constData();
    if (options.getDateTime().isNull()) {
      if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileDest, fileName), password, 0, options.getCompressionMethod(), options.getCompressionLevel())) return false;
    }
    else {
      if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileDest, fileName, options.getDateTime()), password, 0, options.getCompressionMethod(), options.getCompressionLevel())) return false;
    }

    QFileInfo input(fileName);
    if (quazip_is_symlink(input)) {
        // Not sure if we should use any specialized codecs here.
        // After all, a symlink IS just a byte array. And
        // this is mostly for Linux, where UTF-8 is ubiquitous these days.
        QString path = quazip_symlink_target(input);
        QString relativePath = input.dir().relativeFilePath(path);
        outFile.write(QFile::encodeName(relativePath));
    } else {
        QFile inFile;
        inFile.setFileName(fileName);
        if (!inFile.open(QIODevice::ReadOnly))
            return false;
        if (!copyData(inFile, outFile) || outFile.getZipError()!=UNZ_OK)
            return false;
        inFile.close();
    }

    outFile.close();
    return outFile.getZipError() == UNZ_OK;
}

bool JlCompress::compressSubDir(QuaZip* zip, QString dir, QString origDir, bool recursive, QDir::Filters filters) {
  return compressSubDir(zip, dir, origDir, recursive, filters, Options());
}

bool JlCompress::compressSubDir(QuaZip* zip, QString dir, QString origDir, bool recursive, QDir::Filters filters, const Options& options) {
    // zip: object where to add the file
    // dir: current real directory
    // origDir: original real directory
    // (path(dir)-path(origDir)) = path inside the zip object

    if (!zip) return false;
    if (zip->getMode()!=QuaZip::mdCreate &&
        zip->getMode()!=QuaZip::mdAppend &&
        zip->getMode()!=QuaZip::mdAdd) return false;

    QDir directory(dir);
    if (!directory.exists()) return false;

    QDir origDirectory(origDir);
    if (dir != origDir) {
        QuaZipFile dirZipFile(zip);
        const char *password = options.getPassword().isEmpty() ? nullptr : options.getPassword().constData();
        std::unique_ptr<QuaZipNewInfo> qzni;
        if (options.getDateTime().isNull()) {
            qzni = std::make_unique<QuaZipNewInfo>(origDirectory.relativeFilePath(dir) + QLatin1String("/"), dir);
        }
        else {
            qzni = std::make_unique<QuaZipNewInfo>(origDirectory.relativeFilePath(dir) + QLatin1String("/"), dir, options.getDateTime());
        }
        if (!dirZipFile.open(QIODevice::WriteOnly, *qzni, password, 0, 0)) {
            return false;
        }
        dirZipFile.close();
    }

    // Whether to compress the subfolders, recursion
    if (recursive) {
        // For each subfolder
        QFileInfoList files = directory.entryInfoList(QDir::AllDirs|QDir::NoDotAndDotDot|filters);
        for (const auto& file : files) {
            if (!file.isDir()) // needed for Qt < 4.7 because it doesn't understand AllDirs
                continue;
            // Compress subdirectory
            if(!compressSubDir(zip,file.absoluteFilePath(),origDir,recursive,filters, options)) return false;
        }
    }

    // For each file in directory
    QFileInfoList files = directory.entryInfoList(QDir::Files|filters);
    for (const auto& file : files) {
        // If it's not a file or it's the compressed file being created
        if(!file.isFile()||file.absoluteFilePath()==zip->getZipName()) continue;

        // Create relative name for the compressed file
        QString filename = origDirectory.relativeFilePath(file.absoluteFilePath());

        // Compress the file
        if (!compressFile(zip,file.absoluteFilePath(),filename, options)) return false;
    }

    return true;
}

std::pair<bool, bool> JlCompress::extractFile(QuaZip* zip, QString fileName, QString fileDest, const QString& baseDir) {
    // Delegate to password-enabled version with empty password
    return extractFile(zip, fileName, fileDest, QByteArray(), baseDir);
}

std::pair<bool, bool> JlCompress::extractFile(QuaZip* zip, QString fileName, QString fileDest, const QByteArray& password, const QString& baseDir) {
    if (!zip) return {false, false};
    if (zip->getMode()!=QuaZip::mdUnzip) return {false, false};

    // Path traversal protection: validate fileDest is within baseDir if baseDir is provided
    if (!baseDir.isEmpty()) {
        QString cleanBase = QDir::cleanPath(baseDir);
        QDir directory(cleanBase);
        QString absCleanBase = directory.absolutePath();
        if (!absCleanBase.endsWith(QLatin1Char('/')))
            absCleanBase += QLatin1Char('/');

        QString absCleanPath = QDir::cleanPath(QFileInfo(fileDest).absoluteFilePath());
        if (!absCleanPath.startsWith(absCleanBase)) {
            return {true, true}; // success=true, wasSkipped=true (skip path traversal attempt)
        }
    }

    if (!fileName.isEmpty())
        zip->setCurrentFile(fileName);
    QuaZipFile inFile(zip);
    // Convert QByteArray to const char* for QuaZipFile API
    const char *pwd = password.isEmpty() ? nullptr : password.constData();
    if(!inFile.open(QIODevice::ReadOnly, pwd) || inFile.getZipError()!=UNZ_OK) return {false, false};

    // Check existence of resulting file
    QDir curDir;
    if (fileDest.endsWith(QLatin1String("/"))) {
        if (!curDir.mkpath(fileDest)) {
            return {false, false};
        }
    } else {
        if (!curDir.mkpath(QFileInfo(fileDest).absolutePath())) {
            return {false, false};
        }
    }

    QuaZipFileInfo64 info;
    if (!zip->getCurrentFileInfo(&info))
        return {false, false};

    QFile::Permissions srcPerm = info.getPermissions();
    if (fileDest.endsWith(QLatin1String("/")) && QFileInfo(fileDest).isDir()) {
        if (srcPerm != 0) {
            QFile(fileDest).setPermissions(srcPerm);
        }
        return {true, false};
    }

    if (info.isSymbolicLink()) {
        QString target = QFile::decodeName(inFile.readAll());
        return {QFile::link(target, fileDest), false};
    }

    QFile outFile;
    outFile.setFileName(fileDest);
    if(!outFile.open(QIODevice::WriteOnly)) return {false, false};

    if (!copyData(inFile, outFile) || inFile.getZipError()!=UNZ_OK) {
        outFile.close();
        removeFile(QStringList(fileDest));
        return {false, false};
    }
    outFile.close();

    inFile.close();
    if (inFile.getZipError()!=UNZ_OK) {
        removeFile(QStringList(fileDest));
        return {false, false};
    }

    if (srcPerm != 0) {
        outFile.setPermissions(srcPerm);
    }
    return {true, false};
}

bool JlCompress::removeFile(QStringList listFile) {
    bool ret = true;
    // For each file
    for (int i=0; i<listFile.count(); i++) {
        // Remove
        ret = ret && QFile::remove(listFile.at(i));
    }
    return ret;
}

bool JlCompress::compressFile(QString fileCompressed, QString file) {
  return compressFile(fileCompressed, file, JlCompress::Options());
}

bool JlCompress::compressFile(QString fileCompressed, QString file, const Options& options) {
    // Create zip
    QuaZip zip(fileCompressed);
    zip.setUtf8Enabled(options.getUtf8Enabled());

    QDir().mkpath(QFileInfo(fileCompressed).absolutePath());
    if(!zip.open(QuaZip::mdCreate)) {
        QFile::remove(fileCompressed);
        return false;
    }

    // Add file
    if (!compressFile(&zip,file,QFileInfo(file).fileName(), options)) {
        QFile::remove(fileCompressed);
        return false;
    }

    // Close zip
    zip.close();
    if(zip.getZipError()!=0) {
        QFile::remove(fileCompressed);
        return false;
    }

    return true;
}

bool JlCompress::compressFiles(QString fileCompressed, QStringList files) {
    return compressFiles(fileCompressed, files, Options());
}

bool JlCompress::compressFiles(QString fileCompressed, QStringList files, const Options& options) {
  // Create zip
  QuaZip zip(fileCompressed);
  zip.setUtf8Enabled(options.getUtf8Enabled());

  QDir().mkpath(QFileInfo(fileCompressed).absolutePath());
  if(!zip.open(QuaZip::mdCreate)) {
    QFile::remove(fileCompressed);
    return false;
  }

  // Compress files
  QFileInfo info;
  for (int index = 0; index < files.size(); ++index) {
    const QString & file(files.at(index));
    info.setFile(file);
    if (!info.exists() || !compressFile(&zip,file,info.fileName(), options)) {
      QFile::remove(fileCompressed);
      return false;
    }
  }

  // Close zip
  zip.close();
  if(zip.getZipError()!=0) {
    QFile::remove(fileCompressed);
    return false;
  }

  return true;
}

bool JlCompress::compressDir(QString fileCompressed, QString dir, bool recursive) {
    return compressDir(fileCompressed, dir, recursive, QDir::Filters());
}

bool JlCompress::compressDir(QString fileCompressed, QString dir,
                             bool recursive, QDir::Filters filters)
{
    return compressDir(fileCompressed, dir, recursive, filters, Options());
}

bool JlCompress::compressDir(QString fileCompressed, QString dir,
                             bool recursive, QDir::Filters filters, const Options& options)
{
  // Create zip
  QuaZip zip(fileCompressed);
  zip.setUtf8Enabled(options.getUtf8Enabled());
  QDir().mkpath(QFileInfo(fileCompressed).absolutePath());
  if(!zip.open(QuaZip::mdCreate)) {
    QFile::remove(fileCompressed);
    return false;
  }

  // Add the files and subdirectories
  // Adds dir contents but not the dir itself
  if (!compressSubDir(&zip, dir, dir, recursive, filters, options)) {
    QFile::remove(fileCompressed);
    return false;
  }

  // Close zip
  zip.close();
  if(zip.getZipError()!=0) {
    QFile::remove(fileCompressed);
    return false;
  }

  return true;
}

bool JlCompress::addFile(QString fileCompressed, QString file) {
  return addFiles(fileCompressed, QStringList() << file);
}

bool JlCompress::addFile(QString fileCompressed, QString file, const Options& options) {
    return addFiles(fileCompressed, QStringList() << file, options);
}

bool JlCompress::addFiles(QString fileCompressed, QStringList files) {
    return addFiles(fileCompressed, files, Options());
}

bool JlCompress::addFiles(QString fileCompressed, QStringList files, const Options& options) {
  // Verify archive exists
  if (!QFile::exists(fileCompressed)) {
    return false;
  }

  // Open existing zip
  QuaZip zip(fileCompressed);
  // Set UTF-8 flag before opening. Must match the existing archive's encoding.
  zip.setUtf8Enabled(options.getUtf8Enabled());
  if(!zip.open(QuaZip::mdAdd)) {
    return false;
  }

  // Add files
  QFileInfo info;
  for (const QString& file : files) {
    info.setFile(file);
    // Check isFile() to reject directories (but accept symlinks, since isFile() follows symlinks)
    if (!info.exists() || !info.isFile() || !compressFile(&zip,file,info.fileName(), options)) {
      zip.close();
      return false;
    }
  }

  // Close zip
  zip.close();
  return zip.getZipError() == 0;
}

bool JlCompress::addDir(QString fileCompressed, QString dir, bool recursive) {
    return addDir(fileCompressed, dir, recursive, QDir::Filters());
}

bool JlCompress::addDir(QString fileCompressed, QString dir,
                        bool recursive, QDir::Filters filters) {
    return addDir(fileCompressed, dir, recursive, filters, Options());
}

bool JlCompress::addDir(QString fileCompressed, QString dir,
                        bool recursive, QDir::Filters filters, const Options& options) {
  // Verify archive exists
  if (!QFile::exists(fileCompressed)) {
    return false;
  }

  // Open existing zip
  QuaZip zip(fileCompressed);
  // Set UTF-8 flag before opening. Must match the existing archive's encoding.
  zip.setUtf8Enabled(options.getUtf8Enabled());
  if(!zip.open(QuaZip::mdAdd)) {
    return false;
  }

  // Add the files and subdirectories
  if (!compressSubDir(&zip, dir, dir, recursive, filters, options)) {
    zip.close();
    return false;
  }

  // Close zip
  zip.close();
  return zip.getZipError() == 0;
}

QString JlCompress::extractFile(QString fileCompressed, QString fileName, QString fileDest) {
    // Delegate to password-enabled version with empty password
    return extractFile(fileCompressed, fileName, fileDest, QByteArray());
}

QString JlCompress::extractFile(QuaZip &zip, QString fileName, QString fileDest)
{
    if(!zip.open(QuaZip::mdUnzip)) {
        return QString();
    }

    // Extract file
    if (fileDest.isEmpty())
        fileDest = fileName;
    auto [success, skipped] = extractFile(&zip, fileName, fileDest);
    if (!success) {
        return QString();
    }

    // Close zip
    zip.close();
    if(zip.getZipError()!=0) {
        removeFile(QStringList(fileDest));
        return QString();
    }
    return QFileInfo(fileDest).absoluteFilePath();
}

QStringList JlCompress::extractFiles(QString fileCompressed, QStringList files, QString dir) {
    // Delegate to password-enabled version with empty password
    return extractFiles(fileCompressed, files, dir, QByteArray());
}

QStringList JlCompress::extractFiles(QuaZip &zip, const QStringList &files, const QString &dir)
{
    if(!zip.open(QuaZip::mdUnzip)) {
        return QStringList();
    }

    // Prepare base directory for path traversal protection
    QString cleanDir = QDir::cleanPath(dir);
    QDir directory(cleanDir);
    QString absCleanDir = directory.absolutePath();

    // Extract file
    QStringList extracted;
    for (int i=0; i<files.count(); i++) {
        QString absPath = QDir(dir).absoluteFilePath(files.at(i));
        auto [success, skipped] = extractFile(&zip, files.at(i), absPath, absCleanDir);
        if (!success) {
            removeFile(extracted);
            return QStringList();
        }
        if (!skipped) {
            extracted.append(absPath);
        }
    }

    // Close zip
    zip.close();
    if(zip.getZipError()!=0) {
        removeFile(extracted);
        return QStringList();
    }

    return extracted;
}

QStringList JlCompress::extractDir(QString fileCompressed, QuazipTextCodec* fileNameCodec, QString dir) {
    // Open zip
    QuaZip zip(fileCompressed);
    if (fileNameCodec)
        zip.setFileNameCodec(fileNameCodec);
    return extractDir(zip, dir);
}

QStringList JlCompress::extractDir(QString fileCompressed, QString dir) {
    // Delegate to password-enabled version with empty password
    return extractDir(fileCompressed, dir, QByteArray());
}

QStringList JlCompress::extractDir(QuaZip &zip, const QString &dir)
{
    if(!zip.open(QuaZip::mdUnzip)) {
        return QStringList();
    }
    QString cleanDir = QDir::cleanPath(dir);
    QDir directory(cleanDir);
    QString absCleanDir = directory.absolutePath();
    QStringList extracted;
    if (!zip.goToFirstFile()) {
        return QStringList();
    }
    do {
        QString name = zip.getCurrentFileName();
        QString absFilePath = directory.absoluteFilePath(name);
        // Path traversal validation is done inside extractFile
        auto [success, skipped] = extractFile(&zip, QLatin1String(""), absFilePath, absCleanDir);
        if (!success) {
            removeFile(extracted);
            return QStringList();
        }
        if (!skipped) {
            extracted.append(absFilePath);
        }
    } while (zip.goToNextFile());

    // Close zip
    zip.close();
    if(zip.getZipError()!=0) {
        removeFile(extracted);
        return QStringList();
    }

    return extracted;
}

QStringList JlCompress::getFileList(QString fileCompressed) {
    // Open zip
    QuaZip* zip = new QuaZip(QFileInfo(fileCompressed).absoluteFilePath());
    return getFileList(zip);
}

QStringList JlCompress::getFileList(QuaZip *zip)
{
    if(!zip->open(QuaZip::mdUnzip)) {
        delete zip;
        return QStringList();
    }

    // Extract file names
    QStringList lst;
    QuaZipFileInfo64 info;
    for(bool more=zip->goToFirstFile(); more; more=zip->goToNextFile()) {
      if(!zip->getCurrentFileInfo(&info)) {
          delete zip;
          return QStringList();
      }
      lst << info.name;
      //info.name.toLocal8Bit().constData()
    }

    // Close zip
    zip->close();
    if(zip->getZipError()!=0) {
        delete zip;
        return QStringList();
    }
    delete zip;
    return lst;
}

QStringList JlCompress::extractDir(QIODevice* ioDevice, QuazipTextCodec* fileNameCodec, QString dir)
{
    QuaZip zip(ioDevice);
    if (fileNameCodec)
        zip.setFileNameCodec(fileNameCodec);
    return extractDir(zip, dir);
}

QStringList JlCompress::extractDir(QIODevice *ioDevice, QString dir)
{
    return extractDir(ioDevice, nullptr, dir);
}

QStringList JlCompress::getFileList(QIODevice *ioDevice)
{
    QuaZip *zip = new QuaZip(ioDevice);
    return getFileList(zip);
}

QString JlCompress::extractFile(QIODevice *ioDevice, QString fileName, QString fileDest)
{
    QuaZip zip(ioDevice);
    return extractFile(zip, fileName, fileDest);
}

QStringList JlCompress::extractFiles(QIODevice *ioDevice, QStringList files, QString dir)
{
    QuaZip zip(ioDevice);
    return extractFiles(zip, files, dir);
} 

// Extract with password support
QString JlCompress::extractFile(QString fileCompressed, QString fileName, QString fileDest, const QByteArray& password) {
    QuaZip zip(fileCompressed);
    if(!zip.open(QuaZip::mdUnzip)) {
        return QString();
    }
    if (fileDest.isEmpty())
        fileDest = fileName;
    auto [success, skipped] = extractFile(&zip, fileName, fileDest, password);
    if (!success) {
        return QString();
    }
    zip.close();
    if(zip.getZipError()!=0) {
        removeFile(QStringList(fileDest));
        return QString();
    }
    return QFileInfo(fileDest).absoluteFilePath();
}

QStringList JlCompress::extractFiles(QString fileCompressed, QStringList files, QString dir, const QByteArray& password) {
    QuaZip zip(fileCompressed);
    if(!zip.open(QuaZip::mdUnzip)) {
        return QStringList();
    }

    // Prepare base directory for path traversal protection
    QString cleanDir = QDir::cleanPath(dir);
    QDir directory(cleanDir);
    QString absCleanDir = directory.absolutePath();

    QStringList extracted;
    for (int i=0; i<files.count(); i++) {
        QString absPath = QDir(dir).absoluteFilePath(files.at(i));
        auto [success, skipped] = extractFile(&zip, files.at(i), absPath, password, absCleanDir);
        if (!success) {
            removeFile(extracted);
            return QStringList();
        }
        if (!skipped) {
            extracted.append(absPath);
        }
    }
    zip.close();
    if(zip.getZipError()!=0) {
        removeFile(extracted);
        return QStringList();
    }
    return extracted;
}

QStringList JlCompress::extractDir(QString fileCompressed, QString dir, const QByteArray& password) {
    QuaZip zip(fileCompressed);
    if(!zip.open(QuaZip::mdUnzip)) {
        return QStringList();
    }
    QString cleanDir = QDir::cleanPath(dir);
    QDir directory(cleanDir);
    QString absCleanDir = directory.absolutePath();
    QStringList extracted;
    if (!zip.goToFirstFile()) {
        return QStringList();
    }
    do {
        QString name = zip.getCurrentFileName();
        QString absFilePath = directory.absoluteFilePath(name);
        // Path traversal validation is done inside extractFile
        auto [success, skipped] = extractFile(&zip, "", absFilePath, password, absCleanDir);
        if (!success) {
            removeFile(extracted);
            return QStringList();
        }
        if (!skipped) {
            extracted.append(absFilePath);
        }
    } while (zip.goToNextFile());
    zip.close();
    if(zip.getZipError()!=0) {
        removeFile(extracted);
        return QStringList();
    }
    return extracted;
}
