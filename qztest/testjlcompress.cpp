/*
Copyright (C) 2005-2014 Sergey A. Tachenov

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

#include "testjlcompress.h"

#include "qztest.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaType>
#include <QtCore/QTimeZone>
#include <QtCore/QCryptographicHash>
#include <quazip_qt_compat.h>

#include <QtTest/QTest>

#include <JlCompress.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

Q_DECLARE_METATYPE(JlCompress::Options::CompressionStrategy)

void TestJlCompress::compressFile_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("fileName");
    QTest::newRow("simple") << "jlsimplefile.zip" << "test0.txt";
}

void TestJlCompress::compressFile()
{
    QFETCH(QString, zipName);
    QFETCH(QString, fileName);
    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(QStringList() << fileName)) {
        QFAIL("Can't create test file");
    }
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + fileName));
    // get the file list and check it
    QStringList fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList.count(), 1);
    QVERIFY(fileList[0] == fileName);
    // now test the QIODevice* overload of getFileList()
    QFile _zipFile(zipName);
    QVERIFY(_zipFile.open(QIODevice::ReadOnly));
    fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList.count(), 1);
    QVERIFY(fileList[0] == fileName);
    _zipFile.close();
    removeTestFiles(QStringList() << fileName);
    curDir.remove(zipName);
}

void TestJlCompress::compressFileOptions_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QDateTime>("dateTime");
    QTest::addColumn<JlCompress::Options::CompressionStrategy>("strategy");
    QTest::addColumn<QString>("sha256sum_unix"); // Due to extra data archives are not identical
    QTest::addColumn<QString>("sha256sum_unix_ng"); // zlib-ng
    QTest::addColumn<QString>("sha256sum_win");
    QTest::newRow("simple") << "jlsimplefile.zip"
                            << "test0.txt"
                            << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                            << JlCompress::Options::Default
                            << "5eedd83aee92cf3381155d167fee54a4ef6e43b8bc7a979c903611d9aa28610a"
                            << "752db50b15db1a19e091f9c1b43ec22b279867b20d43c76bc9a01d7bc0d7ae4f"
                            << "cb1847dff1a5c33a805efde2558fc74024ad4c64c8607f8b12903e4d92385955";
    QTest::newRow("simple-storage") << "jlsimplefile-storage.zip"
                                    << "test0.txt"
                                    << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                    << JlCompress::Options::Storage
                                    << ""
                                    << ""
                                    << "";
    QTest::newRow("simple-fastest") << "jlsimplefile-fastest.zip"
                                    << "test0.txt"
                                    << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                    << JlCompress::Options::Fastest
                                    << ""
                                    << ""
                                    << "";
    QTest::newRow("simple-faster") << "jlsimplefile-faster.zip"
                                   << "test0.txt"
                                   << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                   << JlCompress::Options::Faster
                                   << ""
                                   << ""
                                   << "";
    QTest::newRow("simple-standard") << "jlsimplefile-standard.zip"
                                     << "test0.txt"
                                     << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                     << JlCompress::Options::Standard
                                     << "5eedd83aee92cf3381155d167fee54a4ef6e43b8bc7a979c903611d9aa28610a"
                                     << "752db50b15db1a19e091f9c1b43ec22b279867b20d43c76bc9a01d7bc0d7ae4f"
                                     << "cb1847dff1a5c33a805efde2558fc74024ad4c64c8607f8b12903e4d92385955";
    QTest::newRow("simple-better") << "jlsimplefile-better.zip"
                                   << "test0.txt"
                                   << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                   << JlCompress::Options::Better
                                   << ""
                                   << ""
                                   << "";
    QTest::newRow("simple-best") << "jlsimplefile-best.zip"
                                 << "test0.txt"
                                 << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                 << JlCompress::Options::Best
                                 << ""
                                 << ""
                                 << "";
}

void TestJlCompress::compressFileOptions()
{
    QFETCH(QString, zipName);
    QFETCH(QString, fileName);
    QFETCH(QDateTime, dateTime);
    QFETCH(JlCompress::Options::CompressionStrategy, strategy);
    QFETCH(QString, sha256sum_unix);
    QFETCH(QString, sha256sum_unix_ng);
    QFETCH(QString, sha256sum_win);
    QDir curDir;
    if (curDir.exists(zipName)) {
      if (!curDir.remove(zipName))
          QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(QStringList() << fileName)) {
        QFAIL("Can't create test file");
    }

    const JlCompress::Options options(dateTime, strategy);
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + fileName, options));
    // get the file list and check it
    QStringList fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList.count(), 1);
    QVERIFY(fileList[0] == fileName);
    // now test the QIODevice* overload of getFileList()
    QFile _zipFile(zipName);
    QVERIFY(_zipFile.open(QIODevice::ReadOnly));
    fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList.count(), 1);
    QVERIFY(fileList[0] == fileName);
    // Hash is computed on the resulting file externally, then hardcoded in the test data
    // This should help detecting any library breakage since we compare against a well-known stable result
    QString hash = QCryptographicHash::hash(_zipFile.readAll(), QCryptographicHash::Sha256).toHex();
#if defined Q_OS_WIN
    if (!sha256sum_win.isEmpty()) QCOMPARE(hash, sha256sum_win);
#elif defined ZLIBNG_VERSION
    if (!sha256sum_unix_ng.isEmpty()) QCOMPARE(hash, sha256sum_unix_ng);
#else
    if (!sha256sum_unix.isEmpty()) QCOMPARE(hash, sha256sum_unix);
#endif
    _zipFile.close();
    removeTestFiles(QStringList() << fileName);
    curDir.remove(zipName);
}

void TestJlCompress::compressFiles_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::newRow("simple") << "jlsimplefiles.zip" <<
        (QStringList() << "test0.txt" << "test00.txt");
    QTest::newRow("different subdirs") << "jlsubdirfiles.zip" <<
        (QStringList() << "subdir1/test1.txt" << "subdir2/test2.txt");
}

void TestJlCompress::compressFiles()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(fileNames)) {
        QFAIL("Can't create test files");
    }
    QStringList realNamesList, shortNamesList;
    foreach (QString fileName, fileNames) {
        QString realName = "tmp/" + fileName;
        realNamesList += realName;
        shortNamesList += QFileInfo(realName).fileName();
    }
    QVERIFY(JlCompress::compressFiles(zipName, realNamesList));
    // get the file list and check it
    QStringList fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList, shortNamesList);
    removeTestFiles(fileNames);
    curDir.remove(zipName);
}

void TestJlCompress::compressDir_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<QStringList>("expected");
    QTest::newRow("simple") << "jldir.zip"
        << (QStringList() << "test0.txt" << "testdir1/test1.txt"
            << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
		<< (QStringList() << "test0.txt"
			<< "testdir1/" << "testdir1/test1.txt"
            << "testdir2/" << "testdir2/test2.txt"
			<< "testdir2/subdir/" << "testdir2/subdir/test2sub.txt");
    QTest::newRow("empty dirs") << "jldir_empty.zip"
		<< (QStringList() << "testdir1/" << "testdir2/testdir3/")
        << (QStringList() << "testdir1/" << "testdir2/"
            << "testdir2/testdir3/");
    QTest::newRow("hidden files") << "jldir_hidden.zip"
        << (QStringList() << ".test0.txt" << "test1.txt")
        << (QStringList() << ".test0.txt" << "test1.txt");
}

void TestJlCompress::compressDir()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(QStringList, expected);
    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(fileNames, -1, "compressDir_tmp")) {
        QFAIL("Can't create test files");
    }
#ifdef Q_OS_WIN
    for (int i = 0; i < fileNames.size(); ++i) {
        if (fileNames.at(i).startsWith(".")) {
            QString fn = "compressDir_tmp\\" + fileNames.at(i);
            SetFileAttributesW(reinterpret_cast<LPCWSTR>(fn.utf16()),
                              FILE_ATTRIBUTE_HIDDEN);
        }
    }
#endif
    QVERIFY(JlCompress::compressDir(zipName, "compressDir_tmp", true, QDir::Hidden));
    // get the file list and check it
    QStringList fileList = JlCompress::getFileList(zipName);
    fileList.sort();
    expected.sort();
    QCOMPARE(fileList, expected);
    removeTestFiles(fileNames, "compressDir_tmp");
    curDir.remove(zipName);
}

void TestJlCompress::compressDirOptions_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<QStringList>("expected");
    QTest::addColumn<QDateTime>("dateTime");
    QTest::addColumn<JlCompress::Options::CompressionStrategy>("strategy");
    QTest::addColumn<QString>("sha256sum_unix");
    QTest::addColumn<QString>("sha256sum_unix_ng");
    QTest::addColumn<QString>("sha256sum_win");
    QTest::newRow("simple") << "jldir.zip"
                            << (QStringList() << "test0.txt" << "testdir1/test1.txt"
                                              << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
                            << (QStringList() << "test0.txt"
                                              << "testdir1/" << "testdir1/test1.txt"
                                              << "testdir2/" << "testdir2/test2.txt"
                                              << "testdir2/subdir/" << "testdir2/subdir/test2sub.txt")
                            << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                            << JlCompress::Options::Default
                            << "ed0d5921b2fc11b6b4cb214b3e43ea3ea28987d6ff8080faab54c4756de30af6"
                            << "299cd566069754a4ca1deb025e279be3cca80e454132b51fa2a22e41c8ef1299"
                            << "1eba110a33718c07a4ddf3fa515d1b4c6e3f4fc912b2e29e5e32783e2cddf852";
    QTest::newRow("simple-storage") << "jldir-storage.zip"
                                    << (QStringList() << "test0.txt" << "testdir1/test1.txt"
                                                      << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
                                    << (QStringList() << "test0.txt"
                                                      << "testdir1/" << "testdir1/test1.txt"
                                                      << "testdir2/" << "testdir2/test2.txt"
                                                      << "testdir2/subdir/" << "testdir2/subdir/test2sub.txt")
                                    << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                    << JlCompress::Options::Storage
                                    << ""
                                    << ""
                                    << "";
    QTest::newRow("simple-fastest") << "jldir-fastest.zip"
                                    << (QStringList() << "test0.txt" << "testdir1/test1.txt"
                                                      << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
                                    << (QStringList() << "test0.txt"
                                                      << "testdir1/" << "testdir1/test1.txt"
                                                      << "testdir2/" << "testdir2/test2.txt"
                                                      << "testdir2/subdir/" << "testdir2/subdir/test2sub.txt")
                                    << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                    << JlCompress::Options::Fastest
                                    << ""
                                    << ""
                                    << "";
    QTest::newRow("simple-faster") << "jldir-faster.zip"
                                    << (QStringList() << "test0.txt" << "testdir1/test1.txt"
                                                      << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
                                    << (QStringList() << "test0.txt"
                                                      << "testdir1/" << "testdir1/test1.txt"
                                                      << "testdir2/" << "testdir2/test2.txt"
                                                      << "testdir2/subdir/" << "testdir2/subdir/test2sub.txt")
                                    << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                    << JlCompress::Options::Faster
                                    << ""
                                    << ""
                                    << "";
    QTest::newRow("simple-standard") << "jldir-standard.zip"
                                    << (QStringList() << "test0.txt" << "testdir1/test1.txt"
                                                      << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
                                    << (QStringList() << "test0.txt"
                                                      << "testdir1/" << "testdir1/test1.txt"
                                                      << "testdir2/" << "testdir2/test2.txt"
                                                      << "testdir2/subdir/" << "testdir2/subdir/test2sub.txt")
                                    << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                    << JlCompress::Options::Standard
                                    << "ed0d5921b2fc11b6b4cb214b3e43ea3ea28987d6ff8080faab54c4756de30af6"
                                    << "299cd566069754a4ca1deb025e279be3cca80e454132b51fa2a22e41c8ef1299"
                                    << "1eba110a33718c07a4ddf3fa515d1b4c6e3f4fc912b2e29e5e32783e2cddf852";
    QTest::newRow("simple-better") << "jldir-better.zip"
                                    << (QStringList() << "test0.txt" << "testdir1/test1.txt"
                                                      << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
                                    << (QStringList() << "test0.txt"
                                                      << "testdir1/" << "testdir1/test1.txt"
                                                      << "testdir2/" << "testdir2/test2.txt"
                                                      << "testdir2/subdir/" << "testdir2/subdir/test2sub.txt")
                                    << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                    << JlCompress::Options::Better
                                    << ""
                                    << ""
                                    << "";
    QTest::newRow("simple-best") << "jldir-best.zip"
                                    << (QStringList() << "test0.txt" << "testdir1/test1.txt"
                                                      << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
                                    << (QStringList() << "test0.txt"
                                                      << "testdir1/" << "testdir1/test1.txt"
                                                      << "testdir2/" << "testdir2/test2.txt"
                                                      << "testdir2/subdir/" << "testdir2/subdir/test2sub.txt")
                                    << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                                    << JlCompress::Options::Best
                                    << ""
                                    << ""
                                    << "";
}

void TestJlCompress::compressDirOptions()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(QStringList, expected);
    QFETCH(QDateTime, dateTime);
    QFETCH(JlCompress::Options::CompressionStrategy, strategy);
    QFETCH(QString, sha256sum_unix);
    QFETCH(QString, sha256sum_unix_ng);
    QFETCH(QString, sha256sum_win);
    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(fileNames, -1, "compressDir_tmp")) {
        QFAIL("Can't create test files");
    }
#ifdef Q_OS_WIN
    for (int i = 0; i < fileNames.size(); ++i) {
        if (fileNames.at(i).startsWith(".")) {
            QString fn = "compressDir_tmp\\" + fileNames.at(i);
            SetFileAttributesW(reinterpret_cast<LPCWSTR>(fn.utf16()),
                               FILE_ATTRIBUTE_HIDDEN);
        }
    }
#endif
    const JlCompress::Options options(dateTime, strategy);
    QVERIFY(JlCompress::compressDir(zipName, "compressDir_tmp", true, QDir::Hidden, options));
    // get the file list and check it
    QStringList fileList = JlCompress::getFileList(zipName);
    fileList.sort();
    expected.sort();
    QCOMPARE(fileList, expected);
    QFile _zipFile(curDir.absoluteFilePath(zipName));
    if (!_zipFile.open(QIODevice::ReadOnly)) {
        QFAIL("Can't read output zip file");
    }
    QString hash = QCryptographicHash::hash(_zipFile.readAll(), QCryptographicHash::Sha256).toHex();
#ifdef Q_OS_WIN
    if (!sha256sum_win.isEmpty()) QCOMPARE(hash, sha256sum_win);
#elif defined ZLIBNG_VERSION
    if (!sha256sum_unix_ng.isEmpty()) QCOMPARE(hash, sha256sum_unix_ng);
#else
    if (!sha256sum_unix.isEmpty()) QCOMPARE(hash, sha256sum_unix);
#endif
    _zipFile.close();
    removeTestFiles(fileNames, "compressDir_tmp");
    curDir.remove(zipName);
}

void TestJlCompress::extractFile_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<QString>("fileToExtract");
    QTest::addColumn<QString>("destName");
    QTest::addColumn<QByteArray>("encoding");
    QTest::newRow("simple") << "jlextfile.zip" << (
            QStringList() << "test0.txt" << "testdir1/test1.txt"
            << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
        << "testdir2/test2.txt" << "test2.txt" << QByteArray();
    QTest::newRow("russian") << "jlextfilerus.zip" << (
            QStringList() << "test0.txt" << "testdir1/test1.txt"
            << QString::fromUtf8("testdir2/тест2.txt")
            << "testdir2/subdir/test2sub.txt")
        << QString::fromUtf8("testdir2/тест2.txt")
        << QString::fromUtf8("тест2.txt") << QByteArray("IBM866");
    QTest::newRow("extract dir") << "jlextdir.zip" << (
            QStringList() << "testdir1/")
        << "testdir1/" << "testdir1/" << QByteArray();
}

void TestJlCompress::extractFile()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(QString, fileToExtract);
    QFETCH(QString, destName);
    QFETCH(QByteArray, encoding);
    QDir curDir;
    if (!curDir.mkpath("jlext/jlfile")) {
        QFAIL("Couldn't mkpath jlext/jlfile");
    }
    if (!createTestFiles(fileNames)) {
        QFAIL("Couldn't create test files");
    }
    QFile srcFile("tmp/" + fileToExtract);
    QFile::Permissions srcPerm = srcFile.permissions();
    // Invert the "write other" flag so permissions
    // are NOT default any more. Otherwise it's impossible
    // to figure out whether the permissions were set correctly
    // or JlCompress failed to set them completely,
    // thus leaving them at the default setting.
    srcPerm ^= QFile::WriteOther;
    QVERIFY(srcFile.setPermissions(srcPerm));
    if (!createTestArchive(zipName, fileNames,
                           QuazipTextCodec::codecForName(encoding))) {
        QFAIL("Can't create test archive");
    }
    QuaZip::setDefaultFileNameCodec(encoding);
    QVERIFY(!JlCompress::extractFile(zipName, fileToExtract,
                "jlext/jlfile/" + destName).isEmpty());
    QFileInfo destInfo("jlext/jlfile/" + destName), srcInfo("tmp/" +
            fileToExtract);
    QCOMPARE(destInfo.size(), srcInfo.size());
    QCOMPARE(destInfo.permissions(), srcInfo.permissions());
    curDir.remove("jlext/jlfile/" + destName);
    // now test the QIODevice* overload
    QFile _zipFile(zipName);
    QVERIFY(_zipFile.open(QIODevice::ReadOnly));
    QVERIFY(!JlCompress::extractFile(&_zipFile, fileToExtract,
                "jlext/jlfile/" + destName).isEmpty());
    destInfo = QFileInfo("jlext/jlfile/" + destName);
    QCOMPARE(destInfo.size(), srcInfo.size());
    QCOMPARE(destInfo.permissions(), srcInfo.permissions());
    curDir.remove("jlext/jlfile/" + destName);
    if (!fileToExtract.endsWith("/")) {
        // If we aren't extracting a directory, we need to check
        // that extractFile() fails if there is a directory
        // with the same name as the file being extracted.
        curDir.mkdir("jlext/jlfile/" + destName);
        QVERIFY(JlCompress::extractFile(zipName, fileToExtract,
                    "jlext/jlfile/" + destName).isEmpty());
    }
    _zipFile.close();
    // Here we either delete the target dir or the dir created in the
    // test above.
    curDir.rmpath("jlext/jlfile/" + destName);
    removeTestFiles(fileNames);
    curDir.remove(zipName);
}

void TestJlCompress::extractFiles_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<QStringList>("filesToExtract");
    QTest::newRow("simple") << "jlextfiles.zip" << (
            QStringList() << "test0.txt" << "testdir1/test1.txt"
            << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
        << (QStringList() << "testdir2/test2.txt" << "testdir1/test1.txt");
}

void TestJlCompress::extractFiles()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(QStringList, filesToExtract);
    QDir curDir;
    if (!curDir.mkpath("jlext/jlfiles")) {
        QFAIL("Couldn't mkpath jlext/jlfiles");
    }
    if (!createTestFiles(fileNames)) {
        QFAIL("Couldn't create test files");
    }
    if (!JlCompress::compressDir(zipName, "tmp")) {
        QFAIL("Couldn't create test archive");
    }
    QVERIFY(!JlCompress::extractFiles(zipName, filesToExtract,
                "jlext/jlfiles").isEmpty());
    foreach (QString fileName, filesToExtract) {
        QFileInfo fileInfo("jlext/jlfiles/" + fileName);
        QFileInfo extInfo("tmp/" + fileName);
        QCOMPARE(fileInfo.size(), extInfo.size());
        QCOMPARE(fileInfo.permissions(), extInfo.permissions());
        curDir.remove("jlext/jlfiles/" + fileName);
        QString dirPath = fileInfo.dir().path();
        if (!dirPath.isEmpty()) {
            curDir.rmpath(dirPath);
        }
    }
    // now test the QIODevice* overload
    QFile _zipFile(zipName);
    QVERIFY(_zipFile.open(QIODevice::ReadOnly));
    QVERIFY(!JlCompress::extractFiles(&_zipFile, filesToExtract,
                "jlext/jlfiles").isEmpty());
    foreach (QString fileName, filesToExtract) {
        QFileInfo fileInfo("jlext/jlfiles/" + fileName);
        QFileInfo extInfo("tmp/" + fileName);
        QCOMPARE(fileInfo.size(), extInfo.size());
        QCOMPARE(fileInfo.permissions(), extInfo.permissions());
        curDir.remove("jlext/jlfiles/" + fileName);
        QString dirPath = fileInfo.dir().path();
        if (!dirPath.isEmpty()) {
            curDir.rmpath(dirPath);
        }
    }
    _zipFile.close();
    curDir.rmpath("jlext/jlfiles");
    removeTestFiles(fileNames);
    curDir.remove(zipName);
}

void TestJlCompress::extractDir_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<QStringList>("expectedExtracted");
    QTest::addColumn<QString>("extDir");
    QTest::addColumn<QByteArray>("fileNameCodecName");
    QTest::newRow("simple") << "jlextdir.zip"
        << (QStringList() << "test0.txt" << "testdir1/test1.txt"
            << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
        << (QStringList() << "test0.txt" << "testdir1/test1.txt"
            << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
        << "tmp/jlext/jldir/"
        << QByteArray();
    QTest::newRow("separate dir") << "sepdir.zip"
        << (QStringList() << "laj/" << "laj/lajfile.txt")
        << (QStringList() << "laj/" << "laj/lajfile.txt")
        << "tmp/jlext/jldir/"
        << QByteArray();
    QTest::newRow("Zip Slip") << "zipslip.zip"
        << (QStringList() << "test0.txt" << "../zipslip.txt")
        << (QStringList() << "test0.txt")
        << "tmp/jlext/jldir/"
        << QByteArray();
    QTest::newRow("Cyrillic") << "cyrillic.zip"
        << (QStringList() << QString::fromUtf8("Ще не вмерла Україна"))
        << (QStringList() << QString::fromUtf8("Ще не вмерла Україна"))
        << "tmp/jlext/jldir/"
        << QByteArray("KOI8-U");
    QTest::newRow("Japaneses") << "japanese.zip"
        << (QStringList() << QString::fromUtf8("日本"))
        << (QStringList() << QString::fromUtf8("日本"))
        << "tmp/jlext/jldir/"
        << QByteArray("UTF-8");
#ifdef QUAZIP_EXTRACT_TO_ROOT_TEST
    QTest::newRow("Exract to root") << "tmp/quazip-root-test.zip"
        << (QStringList() << "tmp/quazip-root-test/test.txt")
        << (QStringList() << "tmp/quazip-root-test/test.txt")
        << QString()
        << QByteArray("UTF-8");
#endif
}

class TemporarilyChangeToRoot {
    QString previousDir;
    bool success{false};
public:
    ~TemporarilyChangeToRoot() {
        if (success) {
            QDir::setCurrent(previousDir); // Let's HOPE it succeeds.
        }
    }
    void changeToRoot() {
        previousDir = QDir::currentPath();
        success = QDir::setCurrent("/");
    }
    inline bool isSuccess() const { return success; }
};

void TestJlCompress::extractDir()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(QStringList, expectedExtracted);
    QFETCH(QString, extDir);
    QFETCH(QByteArray, fileNameCodecName);
    TemporarilyChangeToRoot changeToRoot;
    if (zipName == "tmp/quazip-root-test.zip") {
        changeToRoot.changeToRoot();
        if (!changeToRoot.isSuccess()) {
            QFAIL("Couldn't change to /");
        }
    }
    QuazipTextCodec *fileNameCodec = NULL;
    if (!fileNameCodecName.isEmpty())
        fileNameCodec = QuazipTextCodec::codecForName(fileNameCodecName);
    QDir curDir;
    if (!extDir.isEmpty() && !curDir.mkpath(extDir)) {
        QFAIL("Couldn't mkpath extDir");
    }
    if (!createTestFiles(fileNames)) {
        QFAIL("Couldn't create test files");
    }
    if (!createTestArchive(zipName, fileNames, fileNameCodec)) {
        QFAIL("Couldn't create test archive");
    }
    QStringList extracted;
    if (fileNameCodec == NULL)
        extracted = JlCompress::extractDir(zipName, extDir);
    else // test both overloads here
        extracted = JlCompress::extractDir(zipName, fileNameCodec, extDir);
    QCOMPARE(extracted.count(), expectedExtracted.count());
    const QString dir = extDir;
    foreach (QString fileName, expectedExtracted) {
        QString fullName = dir + fileName;
        QFileInfo fileInfo(fullName);
        QFileInfo extInfo("tmp/" + fileName);
        if (!fileInfo.isDir())
            QCOMPARE(fileInfo.size(), extInfo.size());
        QCOMPARE(fileInfo.permissions(), extInfo.permissions());
        curDir.remove(fullName);
        QString dirPath = fileInfo.dir().path();
        if (!dirPath.isEmpty()) {
            curDir.rmpath(dirPath);
        }
        QString absolutePath = QDir(dir).absoluteFilePath(fileName);
        if (fileInfo.isDir() && !absolutePath.endsWith('/'))
            absolutePath += '/';
        QVERIFY(extracted.contains(absolutePath));
    }
    // now test the QIODevice* overload
    QFile _zipFile(zipName);
    QVERIFY(_zipFile.open(QIODevice::ReadOnly));
    if (fileNameCodec == NULL)
        extracted = JlCompress::extractDir(&_zipFile, extDir);
    else // test both overloads here
        extracted = JlCompress::extractDir(&_zipFile, fileNameCodec, extDir);
    QCOMPARE(extracted.count(), expectedExtracted.count());
    foreach (QString fileName, expectedExtracted) {
        QString fullName = dir + fileName;
        QFileInfo fileInfo(fullName);
        QFileInfo extInfo("tmp/" + fileName);
        if (!fileInfo.isDir())
            QCOMPARE(fileInfo.size(), extInfo.size());
        QCOMPARE(fileInfo.permissions(), extInfo.permissions());
        curDir.remove(fullName);
        QString dirPath = fileInfo.dir().path();
        if (!dirPath.isEmpty()) {
            curDir.rmpath(dirPath);
        }
        QString absolutePath = QDir(dir).absoluteFilePath(fileName);
        if (fileInfo.isDir() && !absolutePath.endsWith('/'))
            absolutePath += '/';
        QVERIFY(extracted.contains(absolutePath));
    }
    _zipFile.close();
    if (!extDir.isEmpty()) {
        curDir.rmpath(extDir);
    }
    removeTestFiles(fileNames);
    //curDir.remove(zipName);
}

void TestJlCompress::zeroPermissions()
{
    QuaZip zipCreator("zero.zip");
    QVERIFY(zipCreator.open(QuaZip::mdCreate));
    QuaZipFile zeroFile(&zipCreator);
    QuaZipNewInfo newInfo("zero.txt");
    newInfo.externalAttr = 0; // should be zero anyway, but just in case
    QVERIFY(zeroFile.open(QIODevice::WriteOnly, newInfo));
    zeroFile.close();
    zipCreator.close();
    QVERIFY(!JlCompress::extractFile("zero.zip", "zero.txt").isEmpty());
    QVERIFY(QFile("zero.txt").permissions() != 0);
    QDir curDir;
    curDir.remove("zero.zip");
    curDir.remove("zero.txt");
}

#ifdef QUAZIP_SYMLINK_TEST

void TestJlCompress::symlinkHandling()
{
    QStringList fileNames { "file.txt" };
    if (!createTestFiles(fileNames)) {
        QFAIL("Couldn't create test files");
    }
    QVERIFY(QFile::link("file.txt", "tmp/link.txt"));
    fileNames << "link.txt";
    QVERIFY(JlCompress::compressDir("symlink.zip", "tmp"));
    QDir curDir;
    QVERIFY(curDir.mkpath("extsymlink"));
    QVERIFY(!JlCompress::extractDir("symlink.zip", "extsymlink").isEmpty());
    QFileInfo linkInfo("extsymlink/link.txt");
    QVERIFY(quazip_is_symlink(linkInfo));
    removeTestFiles(fileNames, "extsymlink");
    removeTestFiles(fileNames, "tmp");
    curDir.remove("symlink.zip");
}

#endif

#ifdef QUAZIP_SYMLINK_EXTRACTION_ON_WINDOWS_TEST

void TestJlCompress::symlinkExtractionOnWindows()
{
    QuaZip zipWithSymlinks("withSymlinks.zip");
    QVERIFY(zipWithSymlinks.open(QuaZip::mdCreate));
    QuaZipFile file(&zipWithSymlinks);
    QVERIFY(file.open(QIODevice::WriteOnly, QuaZipNewInfo("file.txt")));
    file.write("contents");
    file.close();
    QuaZipNewInfo symlinkInfo("symlink.txt");
    symlinkInfo.externalAttr |= 0120000 << 16; // symlink attr
    QuaZipFile symlink(&zipWithSymlinks);
    QVERIFY(symlink.open(QIODevice::WriteOnly, symlinkInfo));
    symlink.write("file.txt"); // link target goes into contents
    symlink.close();
    zipWithSymlinks.close();
    QCOMPARE(zipWithSymlinks.getZipError(), ZIP_OK);
    // The best we can do here is to test that extraction works at all,
    // because it's hard to say what should be the "correct" result when
    // trying to extract symbolic links on Windows.
    QVERIFY(!JlCompress::extractDir("withSymlinks.zip", "symlinksOnWindows").isEmpty());
    QDir curDir;
    curDir.remove("withSymlinks.zip");
    removeTestFiles(QStringList() << "file.txt" << "symlink.txt", "symlinksOnWindows");
}

#endif

void TestJlCompress::addFile_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("initialFile");
    QTest::addColumn<QString>("fileToAdd");
    QTest::newRow("simple") << "jladdfile.zip" << "test0.txt" << "test1.txt";
}

void TestJlCompress::addFile()
{
    QFETCH(QString, zipName);
    QFETCH(QString, initialFile);
    QFETCH(QString, fileToAdd);
    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(QStringList() << initialFile << fileToAdd)) {
        QFAIL("Can't create test files");
    }
    // Create initial archive with one file
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + initialFile));
    // Verify initial archive has 1 file
    QStringList fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList.count(), 1);
    QVERIFY(fileList[0] == initialFile);

    // Add another file to existing archive
    QVERIFY(JlCompress::addFile(zipName, "tmp/" + fileToAdd));

    // Verify archive now has 2 files
    fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList.count(), 2);
    QVERIFY(fileList.contains(initialFile));
    QVERIFY(fileList.contains(fileToAdd));

    removeTestFiles(QStringList() << initialFile << fileToAdd);
    curDir.remove(zipName);
}

void TestJlCompress::addFileOptions_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("initialFile");
    QTest::addColumn<QString>("fileToAdd");
    QTest::addColumn<QDateTime>("dateTime");
    QTest::addColumn<JlCompress::Options::CompressionStrategy>("strategy");
    QTest::newRow("simple") << "jladdfileoptions.zip"
                            << "test0.txt"
                            << "test1.txt"
                            << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                            << JlCompress::Options::Default;
}

void TestJlCompress::addFileOptions()
{
    QFETCH(QString, zipName);
    QFETCH(QString, initialFile);
    QFETCH(QString, fileToAdd);
    QFETCH(QDateTime, dateTime);
    QFETCH(JlCompress::Options::CompressionStrategy, strategy);
    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(QStringList() << initialFile << fileToAdd)) {
        QFAIL("Can't create test files");
    }

    // Create initial archive with one file
    const JlCompress::Options options(dateTime, strategy);
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + initialFile, options));

    // Add another file with options
    QVERIFY(JlCompress::addFile(zipName, "tmp/" + fileToAdd, options));

    // Verify archive has 2 files
    QStringList fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList.count(), 2);
    QVERIFY(fileList.contains(initialFile));
    QVERIFY(fileList.contains(fileToAdd));

    removeTestFiles(QStringList() << initialFile << fileToAdd);
    curDir.remove(zipName);
}

void TestJlCompress::addFiles_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("initialFile");
    QTest::addColumn<QStringList>("filesToAdd");
    QTest::newRow("simple") << "jladdfiles.zip"
                            << "test0.txt"
                            << (QStringList() << "test1.txt" << "test2.txt");
    QTest::newRow("subdirs") << "jladdfiles-subdirs.zip"
                             << "test0.txt"
                             << (QStringList() << "subdir1/test1.txt" << "subdir2/test2.txt");
}

void TestJlCompress::addFiles()
{
    QFETCH(QString, zipName);
    QFETCH(QString, initialFile);
    QFETCH(QStringList, filesToAdd);
    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    QStringList allFiles = QStringList() << initialFile;
    allFiles.append(filesToAdd);
    if (!createTestFiles(allFiles)) {
        QFAIL("Can't create test files");
    }

    // Create initial archive with one file
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + initialFile));

    // Prepare full paths for files to add
    QStringList realNamesList;
    QStringList shortNamesList;
    foreach (QString fileName, filesToAdd) {
        QString realName = "tmp/" + fileName;
        realNamesList += realName;
        shortNamesList += QFileInfo(realName).fileName();
    }

    // Add files to existing archive
    QVERIFY(JlCompress::addFiles(zipName, realNamesList));

    // Verify archive has all files
    QStringList fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList.count(), 1 + shortNamesList.count());
    QVERIFY(fileList.contains(initialFile));
    foreach (QString fileName, shortNamesList) {
        QVERIFY(fileList.contains(fileName));
    }

    removeTestFiles(allFiles);
    curDir.remove(zipName);
}

void TestJlCompress::addFilesOptions_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::newRow("multiple batches") << "jladdfilesoptions.zip";
}

void TestJlCompress::addFilesOptions()
{
    QFETCH(QString, zipName);
    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }

    QStringList allFiles;
    allFiles << "test0.txt" << "test1.txt" << "test2.txt" << "test3.txt" << "test4.txt" << "test5.txt";
    if (!createTestFiles(allFiles)) {
        QFAIL("Can't create test files");
    }

    // Create initial archive with first file using Default compression
    const JlCompress::Options defaultOptions(QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ),
                                              JlCompress::Options::Default);
    QVERIFY(JlCompress::compressFile(zipName, "tmp/test0.txt", defaultOptions));

    // Add batch 1 (2 files) with Storage compression
    const JlCompress::Options storageOptions(QDateTime(QDate(2024, 9, 20), QTime(12, 0, 0), COMPAT_UTC_TZ),
                                              JlCompress::Options::Storage);
    QStringList batch1;
    batch1 << "tmp/test1.txt" << "tmp/test2.txt";
    QVERIFY(JlCompress::addFiles(zipName, batch1, storageOptions));

    // Add batch 2 (2 files) with Best compression
    const JlCompress::Options bestOptions(QDateTime(QDate(2024, 9, 21), QTime(15, 30, 0), COMPAT_UTC_TZ),
                                           JlCompress::Options::Best);
    QStringList batch2;
    batch2 << "tmp/test3.txt" << "tmp/test4.txt";
    QVERIFY(JlCompress::addFiles(zipName, batch2, bestOptions));

    // Add batch 3 (1 file) with Fastest compression
    const JlCompress::Options fastestOptions(QDateTime(QDate(2024, 9, 22), QTime(18, 0, 0), COMPAT_UTC_TZ),
                                              JlCompress::Options::Fastest);
    QStringList batch3;
    batch3 << "tmp/test5.txt";
    QVERIFY(JlCompress::addFiles(zipName, batch3, fastestOptions));

    // Verify archive has all 6 files
    QStringList fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList.count(), 6);
    foreach (QString fileName, allFiles) {
        QVERIFY(fileList.contains(fileName));
    }

    // Extract all files and verify they match originals
    curDir.mkpath("jlext/addFilesOptions");
    QStringList extracted = JlCompress::extractDir(zipName, "jlext/addFilesOptions");
    QCOMPARE(extracted.count(), 6);

    // Verify each extracted file matches the original
    foreach (QString fileName, allFiles) {
        QFile original("tmp/" + fileName);
        QFile extractedFile(curDir.absoluteFilePath("jlext/addFilesOptions/" + fileName));
        QVERIFY(original.open(QIODevice::ReadOnly));
        QVERIFY(extractedFile.open(QIODevice::ReadOnly));
        QCOMPARE(extractedFile.readAll(), original.readAll());
        original.close();
        extractedFile.close();
    }

    removeTestFiles(allFiles);
    removeTestFiles(allFiles, "jlext/addFilesOptions");
    curDir.rmpath("jlext/addFilesOptions");
    curDir.remove(zipName);
}

void TestJlCompress::addDir_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("initialFile");
    QTest::addColumn<QStringList>("dirFiles");
    QTest::addColumn<bool>("recursive");
    QTest::addColumn<QStringList>("expectedAdded");
    QTest::addColumn<QStringList>("expectedNotAdded");

    // Non-recursive: should only add top-level files, NOT subdirectories
    QTest::newRow("non-recursive") << "jladddir-nonrecursive.zip"
                                   << "test0.txt"
                                   << (QStringList() << "toplevel1.txt" << "toplevel2.txt"
                                       << "subdir1/nested1.txt" << "subdir2/nested2.txt")
                                   << false  // recursive = false
                                   << (QStringList() << "toplevel1.txt" << "toplevel2.txt")
                                   << (QStringList() << "subdir1/" << "subdir1/nested1.txt"
                                       << "subdir2/" << "subdir2/nested2.txt");

    // Recursive: should add ALL files and directories
    QTest::newRow("recursive") << "jladddir-recursive.zip"
                               << "test0.txt"
                               << (QStringList() << "toplevel.txt" << "dir1/file1.txt"
                                   << "dir1/subdir/file2.txt" << "dir2/file3.txt")
                               << true  // recursive = true
                               << (QStringList() << "toplevel.txt"
                                   << "dir1/" << "dir1/file1.txt"
                                   << "dir1/subdir/" << "dir1/subdir/file2.txt"
                                   << "dir2/" << "dir2/file3.txt")
                               << (QStringList());  // Nothing should be excluded
}

void TestJlCompress::addDir()
{
    QFETCH(QString, zipName);
    QFETCH(QString, initialFile);
    QFETCH(QStringList, dirFiles);
    QFETCH(bool, recursive);
    QFETCH(QStringList, expectedAdded);
    QFETCH(QStringList, expectedNotAdded);

    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(QStringList() << initialFile)) {
        QFAIL("Can't create initial test file");
    }
    if (!createTestFiles(dirFiles, -1, "addDir_tmp")) {
        QFAIL("Can't create test directory files");
    }

    // Create initial archive with one file
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + initialFile));

    // Add directory to existing archive with specified recursive flag
    QVERIFY(JlCompress::addDir(zipName, "addDir_tmp", recursive));

    // Verify archive has initial file plus all expected added files
    QStringList fileList = JlCompress::getFileList(zipName);
    QVERIFY(fileList.contains(initialFile));

    foreach (QString fileName, expectedAdded) {
        if (!fileList.contains(fileName)) {
            qDebug() << "Missing expected file:" << fileName;
        }
        QVERIFY(fileList.contains(fileName));
    }

    // Verify that files that should NOT be added are indeed not present
    foreach (QString fileName, expectedNotAdded) {
        if (fileList.contains(fileName)) {
            qDebug() << "Unexpectedly found file:" << fileName;
        }
        QVERIFY(!fileList.contains(fileName));
    }

    removeTestFiles(QStringList() << initialFile);
    removeTestFiles(dirFiles, "addDir_tmp");
    curDir.remove(zipName);
}

void TestJlCompress::addDirOptions_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("initialFile");
    QTest::addColumn<QStringList>("dirFiles");
    QTest::addColumn<QStringList>("expectedAdded");
    QTest::addColumn<QDateTime>("dateTime");
    QTest::addColumn<JlCompress::Options::CompressionStrategy>("strategy");
    QTest::newRow("simple") << "jladddir-options.zip"
                            << "test0.txt"
                            << (QStringList() << "addtest1.txt" << "adddir1/addtest2.txt")
                            << (QStringList() << "addtest1.txt"
                                << "adddir1/" << "adddir1/addtest2.txt")
                            << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                            << JlCompress::Options::Default;
    QTest::newRow("empty dir") << "jladddir-empty.zip"
                               << "test0.txt"
                               << (QStringList() << "empty1/" << "parent/empty2/")
                               << (QStringList() << "empty1/" << "parent/"
                                   << "parent/empty2/")
                               << QDateTime(QDate(2024, 9, 19), QTime(21, 0, 0), COMPAT_UTC_TZ)
                               << JlCompress::Options::Default;
}

void TestJlCompress::addDirOptions()
{
    QFETCH(QString, zipName);
    QFETCH(QString, initialFile);
    QFETCH(QStringList, dirFiles);
    QFETCH(QStringList, expectedAdded);
    QFETCH(QDateTime, dateTime);
    QFETCH(JlCompress::Options::CompressionStrategy, strategy);
    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(QStringList() << initialFile)) {
        QFAIL("Can't create initial test file");
    }
    if (!createTestFiles(dirFiles, -1, "addDir_tmp")) {
        QFAIL("Can't create test directory files");
    }

    // Create initial archive with one file
    const JlCompress::Options options(dateTime, strategy);
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + initialFile, options));

    // Add directory with options
    QVERIFY(JlCompress::addDir(zipName, "addDir_tmp", true, QDir::Filters(), options));

    // Verify archive has initial file plus all added files
    QStringList fileList = JlCompress::getFileList(zipName);
    QVERIFY(fileList.contains(initialFile));
    foreach (QString fileName, expectedAdded) {
        QVERIFY(fileList.contains(fileName));
    }

    // Extract and verify
    curDir.mkpath("jlext/addDirOptions");
    QStringList extracted = JlCompress::extractDir(zipName, "jlext/addDirOptions");
    QVERIFY(!extracted.isEmpty());

    // Verify initial file was extracted
    QVERIFY(QFile::exists("jlext/addDirOptions/" + initialFile));

    // Verify all expected files/directories were extracted
    foreach (QString fileName, expectedAdded) {
        QString extractedPath = "jlext/addDirOptions/" + fileName;
        QFileInfo extractedInfo(extractedPath);

        if (fileName.endsWith("/")) {
            // It's a directory - verify it exists as a directory
            QVERIFY2(extractedInfo.exists(), qPrintable("Directory not found: " + extractedPath));
            QVERIFY2(extractedInfo.isDir(), qPrintable("Not a directory: " + extractedPath));

            // If directory name contains "empty", verify it's actually empty
            if (fileName.contains("empty", Qt::CaseInsensitive)) {
                QDir extractedDir(extractedPath);
                QStringList entries = extractedDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
                QVERIFY2(entries.isEmpty(),
                        qPrintable("Directory should be empty but contains: " + entries.join(", ") + " in " + extractedPath));
            }
        } else {
            // It's a file - verify it exists and content matches
            QVERIFY2(extractedInfo.exists(), qPrintable("File not found: " + extractedPath));
            QVERIFY2(extractedInfo.isFile(), qPrintable("Not a file: " + extractedPath));
        }
    }

    removeTestFiles(QStringList() << initialFile);
    removeTestFiles(dirFiles, "addDir_tmp");
    removeTestFiles(QStringList() << initialFile, "jlext/addDirOptions");
    removeTestFiles(dirFiles, "jlext/addDirOptions");
    curDir.rmpath("jlext/addDirOptions");
    curDir.remove(zipName);
}

void TestJlCompress::addFileNonExistingArchive()
{
    QDir curDir;
    QString zipName = "jladdfile-nonexist.zip";
    QString testFile = "test0.txt";

    // Make sure archive doesn't exist
    if (curDir.exists(zipName)) {
        curDir.remove(zipName);
    }

    // Create test file
    if (!createTestFiles(QStringList() << testFile)) {
        QFAIL("Can't create test file");
    }

    // Try to add file to non-existing archive - should fail
    QVERIFY(!JlCompress::addFile(zipName, "tmp/" + testFile));

    // Verify archive was NOT created
    QVERIFY(!QFile::exists(zipName));

    removeTestFiles(QStringList() << testFile);
}

void TestJlCompress::addFilesCollision()
{
    QDir curDir;
    QString zipName = "jladdfiles-collision.zip";
    QString initialFile = "test0.txt";

    // Clean up
    if (curDir.exists(zipName)) {
        curDir.remove(zipName);
    }

    // Create test files with same basename but different content
    curDir.mkdir("tmp");
    QFile f1("tmp/" + initialFile);
    if (!f1.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QFAIL("Can't create tmp test file");
    }
    f1.write("CONTENT FROM TMP\n");
    f1.close();

    curDir.mkdir("subdir1");
    QFile f2("subdir1/" + initialFile);
    if (!f2.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QFAIL("Can't create subdir1 test file");
    }
    f2.write("CONTENT FROM SUBDIR1\n");
    f2.close();

    curDir.mkdir("subdir2");
    QFile f3("subdir2/" + initialFile);
    if (!f3.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QFAIL("Can't create subdir2 test file");
    }
    f3.write("CONTENT FROM SUBDIR2\n");
    f3.close();

    // Create initial archive
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + initialFile));

    // Add files with same basename from different directories
    // ZIP format allows duplicate filenames - they don't overwrite
    QStringList filesToAdd;
    filesToAdd << "subdir1/" + initialFile << "subdir2/" + initialFile;
    QVERIFY(JlCompress::addFiles(zipName, filesToAdd));

    // Verify archive contains 3 duplicate entries with the same name
    // ZIP format permits multiple files with identical names
    QStringList fileList = JlCompress::getFileList(zipName);
    QCOMPARE(fileList.count(), 3);
    // All three should have the same name
    QVERIFY(fileList[0] == initialFile);
    QVERIFY(fileList[1] == initialFile);
    QVERIFY(fileList[2] == initialFile);

    // Extract and verify behavior with duplicate filenames
    QString extractDir = "extracted_collision";
    curDir.mkdir(extractDir);
    QStringList extracted = JlCompress::extractDir(zipName, extractDir);

    // extractDir returns all 3 paths (with duplicates)
    QCOMPARE(extracted.count(), 3);

    // But only 1 file exists on disk (overwrites happened)
    QStringList extractedFiles = curDir.entryList(QDir::Files, QDir::Name);
    int filesInExtractDir = QDir(extractDir).entryList(QDir::Files).count();
    QCOMPARE(filesInExtractDir, 1);

    // The last entry should have won (subdir2 was added last)
    QFile resultFile(extractDir + "/" + initialFile);
    QVERIFY(resultFile.open(QIODevice::ReadOnly));
    QString content = QString::fromUtf8(resultFile.readAll());
    resultFile.close();
    // Content should be from subdir2 file (the last one added)
    QVERIFY(content.contains("CONTENT FROM SUBDIR2"));

    // Cleanup
    curDir.remove("tmp/" + initialFile);
    curDir.rmdir("tmp");
    curDir.remove("subdir1/" + initialFile);
    curDir.rmdir("subdir1");
    curDir.remove("subdir2/" + initialFile);
    curDir.rmdir("subdir2");
    JlCompress::removeFile(extracted);
    curDir.rmdir(extractDir);
    curDir.remove(zipName);
}

void TestJlCompress::addFilesWithDirectory()
{
    QDir curDir;
    QString zipName = "jladdfiles-withdir.zip";
    QString initialFile = "test0.txt";
    QString testFile = "test1.txt";
    QString testFile2 = "test2.txt";

    // Clean up
    if (curDir.exists(zipName)) {
        curDir.remove(zipName);
    }

    // Create test files and directory
    if (!createTestFiles(QStringList() << initialFile << testFile << testFile2)) {
        QFAIL("Can't create test files");
    }
    curDir.mkpath("tmp/testdir");

    // Case 1: Directory comes FIRST - should fail immediately, no files added
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + initialFile));

    QStringList filesToAdd1;
    filesToAdd1 << "tmp/testdir" << "tmp/" + testFile;
    QVERIFY(!JlCompress::addFiles(zipName, filesToAdd1));

    // Verify testFile was NOT added (clean failure)
    QStringList fileList1 = JlCompress::getFileList(zipName);
    QCOMPARE(fileList1.count(), 1);  // Only initialFile
    QVERIFY(fileList1.contains(initialFile));
    QVERIFY(!fileList1.contains(testFile));

    // Archive should still be valid
    QuaZip zip1(zipName);
    QVERIFY(zip1.open(QuaZip::mdUnzip));
    zip1.close();

    // Case 2: File comes BEFORE directory - file gets added, then fails (partial corruption)
    curDir.remove(zipName);
    QVERIFY(JlCompress::compressFile(zipName, "tmp/" + initialFile));

    QStringList filesToAdd2;
    filesToAdd2 << "tmp/" + testFile << "tmp/testdir" << "tmp/" + testFile2;
    QVERIFY(!JlCompress::addFiles(zipName, filesToAdd2));

    // Verify testFile WAS added but testFile2 was NOT (partial corruption)
    QStringList fileList2 = JlCompress::getFileList(zipName);
    QCOMPARE(fileList2.count(), 2);  // initialFile + testFile
    QVERIFY(fileList2.contains(initialFile));
    QVERIFY(fileList2.contains(testFile));
    QVERIFY(!fileList2.contains(testFile2));  // This was after the directory

    // Archive should still be valid (closed properly)
    QuaZip zip2(zipName);
    QVERIFY(zip2.open(QuaZip::mdUnzip));
    zip2.close();

    removeTestFiles(QStringList() << initialFile << testFile << testFile2);
    curDir.rmpath("tmp/testdir");
    curDir.remove(zipName);
}
