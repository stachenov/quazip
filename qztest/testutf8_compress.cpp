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

    // Create test files
    QVERIFY(createTestFiles(testFiles));

    // Compress each file individually with UTF-8 enabled
    int fileIndex = 0;
    for (const QString &fileName : testFiles) {
        QString zipName = QString("utf8_%1.zip").arg(fileIndex++);
        QString filePath = "tmp/" + fileName;

        qDebug() << "Creating UTF-8 archive:" << zipName << "with file:" << fileName;

        JlCompress::Options options;
        options.setUtf8Enabled(true);

        QVERIFY(JlCompress::compressFile(zipName, filePath, options));
        QVERIFY(QFile::exists(zipName));

        // Verify the archive contains the correct filename
        QStringList fileList = JlCompress::getFileList(zipName);
        QCOMPARE(fileList.size(), 1);
        QCOMPARE(fileList[0], fileName);
    }

    // Cleanup test files
    removeTestFiles(testFiles);

    qDebug() << "UTF-8 compression tests completed successfully";
}
