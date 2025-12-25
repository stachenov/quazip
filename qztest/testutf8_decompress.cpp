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
#include <QtCore/QRegularExpression>
#include <QtTest/QTest>

#include <JlCompress.h>

/*
 * The purpose of this test is to extract UTF-8 archives and verify behavior
 * based on archive type (with/without UTF-8 flag), platform, Qt version, and locale.
 *
 * Expected behavior:
 * - Archives WITH UTF-8 flag: Should always work (flag tells us to decode as UTF-8)
 * - Archives WITHOUT UTF-8 flag:
 *   - Qt 6: Works (Qt forces UTF-8)
 *   - Qt 5 + UTF-8 locale: Works (locale encoding matches file bytes)
 *   - Qt 5 + non-UTF-8 locale: Fails with mangling (encoding mismatch)
 *     - Windows: '?' replacement
 *     - Linux: null truncation
 */
void TestUtf8Decompress::decompressUtf8Files()
{
    qDebug() << "Performing UTF-8 decompress tests in " << QDir::currentPath();
    qDebug() << "Current locale: LC_ALL=" << qgetenv("LC_ALL") << "LANG=" << qgetenv("LANG");

    // Expected UTF-8 filenames (must match testutf8_compress.cpp)
    QStringList expectedUtf8Files;
    expectedUtf8Files << QString::fromUtf8("файл.txt");        // Cyrillic
    expectedUtf8Files << QString::fromUtf8("ありがとう.txt");    // Japanese
    expectedUtf8Files << QString::fromUtf8("你好.txt");         // Chinese
    expectedUtf8Files << QString::fromUtf8("Привет.txt");      // Cyrillic
    expectedUtf8Files << QString::fromUtf8("测试.txt");         // Chinese

    // Determine archive type being tested
    QByteArray archiveTypeEnv = qgetenv("TEST_UTF8_ARCHIVE_TYPE");
    QString archiveType = archiveTypeEnv.isEmpty() ? "with_flag" : QString::fromLatin1(archiveTypeEnv);
    qDebug() << "Testing archive type:" << archiveType;

    // Determine expected behavior based on archive type, Qt version, platform, and locale
    bool shouldWork = false;

#ifdef Q_OS_WIN
    // Windows: Has codepage issues regardless of Qt version
    // UTF-8 filenames only work if the UTF-8 flag is set in the archive
    if (archiveType == "with_flag") {
        shouldWork = true;
        qDebug() << "Expected: UTF-8 filenames should work (Windows + UTF-8 flag set)";
    } else {
        shouldWork = false;
        qDebug() << "Expected: UTF-8 filenames will be mangled (Windows + no UTF-8 flag)";
        qDebug() << "  Windows: double-encoding or codepage issues";
    }
#else
    // Linux/Unix: Qt 6 forces UTF-8, Qt 5 depends on locale
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // Qt 6 forces UTF-8 encoding regardless of flag or locale
    shouldWork = true;
    qDebug() << "Expected: UTF-8 filenames should work (Qt 6 forces UTF-8)";
#else
    // Qt 5: depends on locale, even with UTF-8 flag
    // The UTF-8 flag in the archive is not sufficient on Qt 5 if locale is non-UTF-8
    QByteArray locale = qgetenv("LC_ALL");
    if (locale.isEmpty()) locale = qgetenv("LANG");
    shouldWork = locale.contains("UTF-8") || locale.contains("utf8");

    if (shouldWork) {
        qDebug() << "Expected: UTF-8 filenames should work (Qt 5 + UTF-8 locale)";
    } else {
        qDebug() << "Expected: UTF-8 filenames will be mangled (Qt 5 + non-UTF-8 locale)";
        if (archiveType == "with_flag") {
            qDebug() << "  Note: UTF-8 flag is set but Qt 5 still requires UTF-8 locale";
        }
        qDebug() << "  Linux: expecting null truncation (.txt)";
    }
#endif
#endif

    // Find artifact directories containing UTF-8 archives
    QString pattern = QString("utf8_%1_qt*").arg(archiveType);
    QDirIterator it(QDir::currentPath(), QStringList() << pattern, QDir::Dirs, QDirIterator::Subdirectories);

    QVERIFY2(it.hasNext(), qPrintable(QString("No UTF-8 archive artifacts found matching: %1").arg(pattern)));

    int totalArchivesProcessed = 0;

    while (it.hasNext()) {
        QFileInfo artifactDir(it.next());
        qDebug() << "====== Found artifact:" << artifactDir.fileName() << "======";

        QDir dir(artifactDir.absoluteFilePath());
        // Look for archives matching the type we're testing
        QString zipPattern = QString("utf8_%1_*.zip").arg(archiveType);
        QStringList zipFiles = dir.entryList(QStringList() << zipPattern, QDir::Files);

        QVERIFY2(!zipFiles.isEmpty(), qPrintable(QString("No UTF-8 zip files found in artifact directory: %1").arg(artifactDir.fileName())));

        for (const QString &archiveFile : zipFiles) {
            QString zipPath = dir.absoluteFilePath(archiveFile);
            qDebug() << "Processing archive:" << archiveFile;

            // Extract to a temporary directory
            QString extractDir = QString("tmp/utf8_extract_%1").arg(archiveFile.left(archiveFile.length() - 4));
            QDir().mkpath(extractDir);

            QStringList extracted = JlCompress::extractDir(zipPath, extractDir);

            // We expect extraction to succeed, but filenames will be mangled in C locale
            QVERIFY2(!extracted.isEmpty(), qPrintable(QString("Failed to extract any files from: %1").arg(archiveFile)));

            qDebug() << "Extracted files:" << extracted;

            // Verify files exist (even with mangled names)
            // Note: We include QDir::Hidden because UTF-8 filenames can get so mangled in C locale
            // that only the extension remains (e.g., "файл.txt" -> ".txt"), which is a hidden file
            QDir extractedDir(extractDir);
            QStringList extractedFiles = extractedDir.entryList(QDir::Files | QDir::Hidden);
            QVERIFY2(!extractedFiles.isEmpty(), qPrintable(QString("No files found after extraction from: %1").arg(archiveFile)));

            // Verify extracted filenames based on expected behavior
            QCOMPARE(extractedFiles.size(), 1);  // Each archive contains exactly one file
            QString extractedFile = extractedFiles.first();
            QString filePath = extractedDir.absoluteFilePath(extractedFile);
            QVERIFY2(QFile::exists(filePath), qPrintable(QString("Extracted file does not exist: %1").arg(filePath)));
            qDebug() << "Extracted file:" << extractedFile << "(" << extractedFile.toUtf8().toHex() << ")";

            if (shouldWork) {
                // Filenames should be correctly decoded as UTF-8
                // Verify extracted filename matches one of the expected UTF-8 filenames
                QVERIFY2(expectedUtf8Files.contains(extractedFile),
                         qPrintable(QString("Expected one of the UTF-8 filenames, got: %1").arg(extractedFile)));
                qDebug() << "  UTF-8 filename correctly decoded:" << extractedFile;
            } else {
                // Filenames should be mangled
#ifdef Q_OS_WIN
                // Windows: behavior varies - may produce '?' replacement, double-encoding, or other mangling
                // Just verify it doesn't match the expected UTF-8 filenames
                QVERIFY2(!expectedUtf8Files.contains(extractedFile),
                         qPrintable(QString("Expected mangled filename on Windows, but got valid UTF-8: %1").arg(extractedFile)));
                qDebug() << "  Windows: filename mangled as expected (got:" << extractedFile << ")";
#else
                // Linux Qt 5 + non-UTF-8 locale: expect null truncation
                QCOMPARE(extractedFile, QString(".txt"));
                qDebug() << "  Linux: null truncation as expected";
#endif
            }

            // Cleanup
            extractedDir.removeRecursively();

            totalArchivesProcessed++;
        }
    }

    QVERIFY2(totalArchivesProcessed > 0, "No UTF-8 archives were processed");
    qDebug() << "UTF-8 decompression tests completed successfully. Processed" << totalArchivesProcessed << "archives.";
}
