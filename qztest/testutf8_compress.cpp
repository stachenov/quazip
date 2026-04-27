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

#include "testutf8_compress.h"

#include "qztest.h"

#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtTest/QTest>

#include <JlCompress.h>

/*
 * The purpose of this test is to create archives with UTF-8 filenames
 * in a UTF-8 locale environment. This test creates two types of archives:
 * - With UTF-8 flag enabled (utf8_with_flag_*.zip)
 * - Without UTF-8 flag (utf8_no_flag_*.zip)
 *
 * These archives are uploaded as artifacts and extracted on various platforms
 * to test decompression behavior.
 *
 * IMPORTANT: This test should only run on UTF-8 locale systems to ensure
 * files are created correctly. Testing non-UTF-8 locales is done during
 * decompression, not compression.
 */
void TestUtf8Compress::compressUtf8Files()
{
    // Verify we're running on a UTF-8 system
    QByteArray locale = qgetenv("LC_ALL");
    if (locale.isEmpty()) locale = qgetenv("LANG");
    qDebug() << "Current locale:" << locale;

    // Test filenames with various UTF-8 characters
    QStringList testFiles;
    testFiles << QString::fromUtf8("файл.txt");        // Cyrillic (4 chars)
    testFiles << QString::fromUtf8("ありがとう.txt");    // Japanese (5 chars)
    testFiles << QString::fromUtf8("你好.txt");         // Chinese (2 chars)
    testFiles << QString::fromUtf8("Привет.txt");      // Cyrillic (6 chars)
    testFiles << QString::fromUtf8("测试.txt");         // Chinese (2 chars)

    // Determine which type of archive to create
    bool withUtf8Flag = qgetenv("TEST_UTF8_WITH_FLAG") == "true";
    QString prefix = withUtf8Flag ? "utf8_with_flag" : "utf8_no_flag";

    qDebug() << "\n========================================";
    qDebug() << "Creating archives:" << prefix;
    qDebug() << "UTF-8 flag:" << (withUtf8Flag ? "ENABLED" : "DISABLED");
    qDebug() << "========================================";

    // Create test files
    QVERIFY(createTestFiles(testFiles));

    // Compress each file individually
    int fileIndex = 0;
    for (const QString &fileName : testFiles) {
        QString zipName = QString("%1_%2.zip").arg(prefix).arg(fileIndex++);
        QString filePath = "tmp/" + fileName;

        JlCompress::Options options;
        options.setUtf8Enabled(withUtf8Flag);

        qDebug() << "Compressing:" << fileName << "->" << zipName;
        QVERIFY(JlCompress::compressFile(zipName, filePath, options));
        QVERIFY(QFile::exists(zipName));

        // Verify the file was stored correctly
        QStringList fileList = JlCompress::getFileList(zipName);
        QCOMPARE(fileList.size(), 1);

        // On UTF-8 systems, filename should always be preserved in the archive
        // (whether UTF-8 flag is set or not, the bytes are UTF-8)
        QCOMPARE(fileList[0], fileName);

        qDebug() << "  Stored as:" << fileList[0];
        qDebug() << "  Bytes:" << fileList[0].toUtf8().toHex();
    }

    // Cleanup test files
    removeTestFiles(testFiles);

    qDebug() << "========================================";
    qDebug() << "Successfully created" << testFiles.size() << prefix << "archives";
    qDebug() << "========================================";
}
