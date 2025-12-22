/*
Copyright (C) 2025 cen1

This file is part of QuaZip test suite.

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

#include "testutf8_decompress.h"

#include "qztest.h"

#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtTest/QTest>

#include <JlCompress.h>

/*
 * The purpose of this test is to extract UTF-8 archives created in a UTF-8 locale
 * but extracted in a C locale environment. We expect the filenames to be mangled
 * (not properly decoded as UTF-8) since we're in a C locale.
 */
void TestUtf8Decompress::decompressUtf8Files()
{
    qDebug() << "Performing UTF-8 decompress tests in " << QDir::currentPath();
    qDebug() << "Current locale: LC_ALL=" << qgetenv("LC_ALL") << "LANG=" << qgetenv("LANG");

    // Find artifact directories containing UTF-8 archives
    QDirIterator it(QDir::currentPath(), QStringList() << "utf8_archives_*", QDir::Dirs, QDirIterator::Subdirectories);

    QVERIFY(it.hasNext());

    while (it.hasNext()) {
        QFileInfo artifactDir(it.next());
        qDebug() << "====== Found artifact:" << artifactDir.fileName() << "======";

        QDir dir(artifactDir.absoluteFilePath());
        QStringList zipFiles = dir.entryList(QStringList() << "utf8_*.zip", QDir::Files);

        QVERIFY(!zipFiles.isEmpty());

        for (const QString &archiveFile : zipFiles) {
            QString zipPath = dir.absoluteFilePath(archiveFile);
            qDebug() << "Processing archive:" << archiveFile;

            // Extract to a temporary directory
            QString extractDir = QString("tmp/utf8_extract_%1").arg(archiveFile.left(archiveFile.length() - 4));
            QDir().mkpath(extractDir);

            QStringList extracted = JlCompress::extractDir(zipPath, extractDir);

            // We expect extraction to succeed, but filenames will be mangled in C locale
            QVERIFY(!extracted.isEmpty());

            qDebug() << "Extracted files:" << extracted;

            // Verify files exist (even with mangled names)
            QDir extractedDir(extractDir);
            QStringList extractedFiles = extractedDir.entryList(QDir::Files);
            QVERIFY(!extractedFiles.isEmpty());

            // In C locale, UTF-8 filenames should be mangled (not properly decoded)
            // We just verify that extraction doesn't crash and produces some output
            for (const QString &extractedFile : extractedFiles) {
                QString filePath = extractedDir.absoluteFilePath(extractedFile);
                QVERIFY(QFile::exists(filePath));
                qDebug() << "Found extracted file (possibly mangled):" << extractedFile;
            }

            // Cleanup
            extractedDir.removeRecursively();
        }
    }

    qDebug() << "UTF-8 decompression tests completed successfully";
}
