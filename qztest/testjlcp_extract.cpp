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

#include "testjlcp_extract.h"

#include "qztest.h"

#include <QSet>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaType>

#include <QtTest/QtTest>

#include <JlCompress.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

Q_DECLARE_METATYPE(JlCompress::Options::CompressionStrategy)

/* Inside the CI env, current directory contains archives like
 * macos-13_qt5.15.2_sharedOFF_cp.zip
 * ubuntu-22.04_qt5.15.2_sharedOFF_cp.zip
 * We extract all, check content. This verifies an archive created on different
 * platform can be unpacked.
 */
void TestJlCpExtract::extract()
{
	QSet<QString> zipNames = {
	  "jlsimplefile.zip",
	  "jlsimplefile-storage.zip",
	  "jlsimplefile-best.zip"
	};

    QSet<QString> fileNames = {
	  "test0.txt"
	};

    qDebug() << "Performing CP extract tests in " << QDir::currentPath();

    // If artifact is downloaded from GHA iz has a .zip but inside actions it does not..
	QDirIterator itZipBundles(QDir::currentPath(), QStringList() << "*_cp.zip", QDir::Files, QDirIterator::Subdirectories);
    QDirIterator itDirBundles(QDir::currentPath(), QStringList() << "*_cp", QDir::Files, QDirIterator::Subdirectories);

	auto ext = [&](QDirIterator &it) {
        QFileInfo bundle(it.next());

		qDebug() << "Found CP BUNDLE:" << bundle.fileName();
        QDir target(bundle.completeBaseName()); // removes .zip
        if (target.exists()) target.removeRecursively();

        // macos-13_qt6.8.2_sharedON_cp
        JlCompress::extractDir(bundle.absoluteFilePath(), target.absolutePath());
        QFileInfo cpZip(target.absolutePath(), "cp.zip");
        QVERIFY(cpZip.exists());

        QDir target2(target.absolutePath()+"/cp");
        // macos-13_qt6.8.2_sharedON_cp/cp/
        JlCompress::extractDir(cpZip.absoluteFilePath(), target2.absolutePath());

        QList<QString> extractedZipList = target2.entryList(QStringList() << "*.zip", QDir::Files);
        QSet<QString> extractedZipSet(extractedZipList.begin(), extractedZipList.end());

        extractedZipSet = extractedZipSet.subtract(zipNames);
        QVERIFY(extractedZipSet.isEmpty());

        for (const QString &zipFile : zipNames) {
        	qDebug() << "Found ZIP:" << zipFile;
            QFileInfo zip(target2, zipFile);
			QDir target3(target2.absolutePath()+"/"+zip.completeBaseName());
            // macos-13_qt6.8.2_sharedON_cp/cp/jlsimplefile/
			JlCompress::extractDir(zip.absoluteFilePath(), target3.absolutePath());

			QList<QString> extractedFileList = target3.entryList(QDir::Files);
			QSet<QString> extractedFileSet(extractedFileList.begin(), extractedFileList.end());

            extractedFileSet = extractedFileSet.subtract(fileNames);
			QVERIFY(extractedFileSet.isEmpty());

            // Check file content
            for (const QString &fileName : extractedFileList) {
                qDebug() << "Found File:" << fileName;

                QFile file(target3.absolutePath()+"/"+fileName);
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                  QFAIL("Failed to open file");
                }

                QTextStream in(&file);
                QString fileContent = in.readAll();
                file.close();

                QString expectedContent("");
                QTextStream ss(&expectedContent);
                ss << "This is a test file named " << fileName << Qt::endl;

                QVERIFY(fileContent == expectedContent);
            }
        }
	};

    while (itZipBundles.hasNext()) {
    	ext(itZipBundles);
    }
    while (itDirBundles.hasNext()) {
        ext(itDirBundles);
    }
}
