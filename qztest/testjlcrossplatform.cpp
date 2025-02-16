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

Original ZIP package is copyrighted by Gilles Vollant and contributors,
see quazip/(un)zip.h files for details. Basically it's the zlib license.
*/

#include "testjlcrossplatform.h"

#include "qztest.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaType>
#include <quazip_qt_compat.h>

#include <QtTest/QtTest>

#include <JlCompress.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

Q_DECLARE_METATYPE(JlCompress::Options::CompressionStrategy)

/*
 * The purpose of this tests is to create bundle archives of most common options
 * on each supported OS platform, NOT delete them after run, upload them to object storage
 * then perform extraction and validation of resulting archives on non-native platforms.
 * For example: produce archives on Windows and Linux, extract and verify on Mac.
 * Repeat for each combination.
 */
void TestJlCrossPlatform::compressFileOptions_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<JlCompress::Options::CompressionStrategy>("strategy");
    QTest::newRow("simple") << "jlsimplefile.zip"
                            << "test0.txt"
                            << JlCompress::Options::Default;
    QTest::newRow("simple-storage") << "jlsimplefile-storage.zip"
                                    << "test0.txt"
                                    << JlCompress::Options::Storage;
    QTest::newRow("simple-best") << "jlsimplefile-best.zip"
                                 << "test0.txt"
                                 << JlCompress::Options::Best;
}

void TestJlCrossPlatform::compressFileOptions()
{
    QFETCH(QString, zipName);
    QFETCH(QString, fileName);
    QFETCH(JlCompress::Options::CompressionStrategy, strategy);
    QDir curDir;
    if (curDir.exists(zipName)) {
      if (!curDir.remove(zipName))
          QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(QStringList() << fileName)) {
        QFAIL("Can't create test file");
    }

    const JlCompress::Options options(strategy);
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + fileName, options));

    removeTestFiles(QStringList() << fileName);

    archivesToBundle << zipName;
}

void TestJlCrossPlatform::createBundle()
{
  QVERIFY(JlCompress::compressFiles("cp.zip", archivesToBundle));
}
