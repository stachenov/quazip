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
#include <QtTest/QTest>

#include <JlCompress.h>

/*
 * The purpose of this test is to create archives with UTF-8 filenames
 * in a UTF-8 locale environment. These archives will be uploaded as artifacts
 * and then extracted in a C locale environment to verify proper handling
 * of UTF-8 filenames.
 */
void TestUtf8Compress::compressUtf8Files()
{
    // Test filenames with various UTF-8 characters
    QStringList testFiles;
    testFiles << QString::fromUtf8("файл.txt");        // Cyrillic
    testFiles << QString::fromUtf8("ありがとう.txt");    // Japanese
    testFiles << QString::fromUtf8("你好.txt");         // Chinese
    testFiles << QString::fromUtf8("Привет.txt");      // Cyrillic
    testFiles << QString::fromUtf8("测试.txt");         // Chinese

    // Check if we're in C locale to demonstrate filename mangling
    bool isCompressBad = qgetenv("TEST_UTF8_COMPRESS_BAD") == "true";

    if (isCompressBad) {
        qDebug() << "\n========================================";
        qDebug() << "Compressing UTF-8 filenames WITHOUT UTF-8 flag";
        qDebug() << "Locale:" << (qgetenv("LC_ALL").isEmpty() ? qgetenv("LANG") : qgetenv("LC_ALL"));
        qDebug() << "On non-UTF-8 systems, filenames will be mangled";
        qDebug() << "========================================";
    }

    // Create test files
    QVERIFY(createTestFiles(testFiles));

    // Compress each file individually
    int fileIndex = 0;
    for (const QString &fileName : testFiles) {
        QString zipName = QString("utf8_%1.zip").arg(fileIndex++);
        QString filePath = "tmp/" + fileName;

        JlCompress::Options options;
        options.setUtf8Enabled(!isCompressBad);  // Disable UTF-8 for "bad" test

        QVERIFY(JlCompress::compressFile(zipName, filePath, options));
        QVERIFY(QFile::exists(zipName));

        // Get the filename as stored in archive
        QStringList fileList = JlCompress::getFileList(zipName);
        QCOMPARE(fileList.size(), 1);

        if (isCompressBad) {
            // In C locale without UTF-8 flag, show what actually happens
            qDebug() << "Original filename:" << fileName;
            qDebug() << "Filename in archive:" << fileList[0];
            qDebug() << "Raw bytes:" << fileList[0].toLocal8Bit().toHex();
            qDebug() << "---";
            // Note: On Qt6 Linux, Qt forces UTF-8 so filenames may look correct
            // On Qt5 or Windows with non-UTF-8 locale, we'll see actual mangling
        } else {
            // With UTF-8 flag, filename should match
            QCOMPARE(fileList[0], fileName);
        }
    }

    // Cleanup test files
    removeTestFiles(testFiles);

    qDebug() << "UTF-8 compression tests completed successfully";
}
