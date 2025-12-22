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

#include "testquacompress.h"

#include "qztest.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtTest/QTest>

#include <QuaCompress.h>

Q_DECLARE_METATYPE(JlCompress::Options::CompressionStrategy)

// Test data for fluent API chaining
void TestQuaCompress::fluentApiChaining_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("fileName");

    QTest::newRow("simple") << "quacompress_chain.zip" << "test0.txt";
}

// Test that fluent API chaining works correctly
void TestQuaCompress::fluentApiChaining()
{
    QFETCH(QString, zipName);
    QFETCH(QString, fileName);

    // Create test file
    QVERIFY(createTestFiles(QStringList() << fileName));

    // Test method chaining returns reference to same object
    QuaCompress compressor;
    QuaCompress& ref1 = compressor.withUtf8Enabled(true);
    QCOMPARE(&ref1, &compressor);

    QuaCompress& ref2 = compressor.withCompression(JlCompress::Options::Better);
    QCOMPARE(&ref2, &compressor);

    QuaCompress& ref3 = compressor.withDateTime(QDateTime::currentDateTime());
    QCOMPARE(&ref3, &compressor);

    // Test that chained configuration works end-to-end
    bool result = QuaCompress()
        .withUtf8Enabled()
        .withCompression(JlCompress::Options::Best)
        .compressFile(zipName, "tmp/" + fileName);

    QVERIFY(result);
    QVERIFY(QFile::exists(zipName));

    // Cleanup
    removeTestFiles(QStringList() << fileName);
    QFile::remove(zipName);
}

// Test data for compressFile - comprehensive matrix of options
void TestQuaCompress::compressFile_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("utf8");
    QTest::addColumn<JlCompress::Options::CompressionStrategy>("compression");
    QTest::addColumn<QByteArray>("password");

    // Basic tests with different compression levels
    QTest::newRow("simple") << "quacompress_simple.zip"
                            << "test0.txt"
                            << false
                            << JlCompress::Options::Default
                            << QByteArray();
    QTest::newRow("storage") << "quacompress_storage.zip"
                             << "test0.txt"
                             << false
                             << JlCompress::Options::Storage
                             << QByteArray();
    QTest::newRow("fastest") << "quacompress_fastest.zip"
                             << "test0.txt"
                             << false
                             << JlCompress::Options::Fastest
                             << QByteArray();
    QTest::newRow("best") << "quacompress_best.zip"
                          << "test0.txt"
                          << false
                          << JlCompress::Options::Best
                          << QByteArray();

    // UTF-8 tests with different scripts and compression levels
    QTest::newRow("utf8-japanese-best") << "quacompress_utf8_ja.zip"
                                        << QString::fromUtf8("ありがとう.txt")
                                        << true
                                        << JlCompress::Options::Best
                          << QByteArray();
    QTest::newRow("utf8-cyrillic-default") << "quacompress_utf8_ru.zip"
                                           << QString::fromUtf8("Привет.txt")
                                           << true
                                           << JlCompress::Options::Default
                                           << QByteArray();
    QTest::newRow("utf8-chinese-fastest") << "quacompress_utf8_zh.zip"
                                          << QString::fromUtf8("你好.txt")
                                          << true
                                          << JlCompress::Options::Fastest
                             << QByteArray();

    // Password tests
    QTest::newRow("password-simple") << "quacompress_password.zip"
                                     << "test0.txt"
                                     << false
                                     << JlCompress::Options::Default
                                     << QByteArray("password123");
    QTest::newRow("password-utf8-best") << "quacompress_password_utf8.zip"
                                        << QString::fromUtf8("тест.txt")
                                        << true
                                        << JlCompress::Options::Best
                                        << QByteArray("секрет");
}

// Test compressFile with various options
void TestQuaCompress::compressFile()
{
    QFETCH(QString, zipName);
    QFETCH(QString, fileName);
    QFETCH(bool, utf8);
    QFETCH(JlCompress::Options::CompressionStrategy, compression);
    QFETCH(QByteArray, password);

    // Skip UTF-8 tests if platform doesn't support UTF-8
    if (utf8 && !isPlatformUtf8()) {
        QSKIP("Skipping UTF-8 test on non-UTF-8 platform");
    }

    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(QStringList() << fileName)) {
        QFAIL("Can't create test file");
    }

    // Compress file (fluent API)
    bool result = QuaCompress()
        .withUtf8Enabled(utf8)
        .withCompression(compression)
        .withPassword(password)
        .compressFile(zipName, "tmp/" + fileName);
    QVERIFY(result);
    QVERIFY(QFile::exists(zipName));

    // Verify file list (works without password)
    QStringList fileList = QuaCompress().getFileList(zipName);
    QCOMPARE(fileList.count(), 1);
    QCOMPARE(fileList[0], fileName);

    // Verify archive can be extracted
    QStringList extracted;
    if (!password.isEmpty()) {
        // Test fluent API extraction
        extracted = QuaCompress()
            .withPassword(password)
            .extractDir(zipName, "tmp/quacompress_extracted");
        QCOMPARE(extracted.size(), 1);
        QVERIFY(QFile::exists("tmp/quacompress_extracted/" + fileName));

        // Also test parameter-based extraction
        QDir("tmp/quacompress_extracted").removeRecursively();
        extracted = QuaCompress().extractDir(zipName, "tmp/quacompress_extracted", password);
        QCOMPARE(extracted.size(), 1);

        // Test wrong password fails
        QStringList wrongExtract = QuaCompress().extractDir(zipName, "tmp/quacompress_wrong", QByteArray("wrong"));
        QCOMPARE(wrongExtract.size(), 0);
        QDir("tmp/quacompress_wrong").removeRecursively();
    } else {
        // No password
        extracted = QuaCompress().extractDir(zipName, "tmp/quacompress_extracted");
        QCOMPARE(extracted.size(), 1);
        QVERIFY(QFile::exists("tmp/quacompress_extracted/" + fileName));
    }

    // Cleanup
    removeTestFiles(QStringList() << fileName);
    curDir.remove(zipName);
    QDir("tmp/quacompress_extracted").removeRecursively();
}

// Test data for compressFiles
void TestQuaCompress::compressFiles_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<bool>("utf8");

    QTest::newRow("simple") << "quacompress_files.zip"
                            << (QStringList() << "test0.txt" << "test1.txt")
                            << false;
    QTest::newRow("utf8") << "quacompress_files_utf8.zip"
                          << (QStringList() << "test0.txt" << QString::fromUtf8("тест.txt"))
                          << true;
    QTest::newRow("subdirs") << "quacompress_subdirs.zip"
                             << (QStringList() << "subdir1/test1.txt" << "subdir2/test2.txt")
                             << false;
}

// Test compressFiles with multiple files
void TestQuaCompress::compressFiles()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(bool, utf8);

    // Skip UTF-8 tests if platform doesn't support UTF-8
    if (utf8 && !isPlatformUtf8()) {
        QSKIP("Skipping UTF-8 test on non-UTF-8 platform");
    }

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

    // Compress files
    bool result = QuaCompress()
        .withUtf8Enabled(utf8)
        .compressFiles(zipName, realNamesList);
    QVERIFY(result);

    // Verify file list
    QStringList list = QuaCompress().getFileList(zipName);
    QCOMPARE(list, shortNamesList);

    // Cleanup
    removeTestFiles(fileNames);
    curDir.remove(zipName);
}

// Test data for compressDir
void TestQuaCompress::compressDir_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<int>("expectedMinFiles");
    QTest::addColumn<bool>("recursive");
    QTest::addColumn<bool>("utf8");

    QTest::newRow("simple-recursive") << "quacompress_dir.zip"
        << (QStringList() << "test0.txt" << "testdir1/test1.txt"
            << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
        << 4  // At least 4 files
        << true
        << false;
    QTest::newRow("recursive-utf8") << "quacompress_dir_utf8.zip"
        << (QStringList() << "test0.txt" << "testdir1/test1.txt")
        << 2  // At least 2 files
        << true
        << true;
    QTest::newRow("non-recursive") << "quacompress_dir_nonrec.zip"
        << (QStringList() << "test0.txt" << "testdir1/test1.txt")
        << 1  // Only test0.txt (non-recursive)
        << false
        << false;
}

// Test compressDir with recursive/non-recursive options
void TestQuaCompress::compressDir()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(int, expectedMinFiles);
    QFETCH(bool, recursive);
    QFETCH(bool, utf8);

    // Skip UTF-8 tests if platform doesn't support UTF-8
    if (utf8 && !isPlatformUtf8()) {
        QSKIP("Skipping UTF-8 test on non-UTF-8 platform");
    }

    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(fileNames)) {
        QFAIL("Can't create test files");
    }

    // Compress directory
    bool result = QuaCompress()
        .withUtf8Enabled(utf8)
        .compressDir(zipName, "tmp", recursive);
    QVERIFY(result);

    // Verify file list has at least the expected number of files
    QStringList list = QuaCompress().getFileList(zipName);
    QVERIFY(list.size() >= expectedMinFiles);

    // Cleanup
    removeTestFiles(fileNames);
    curDir.remove(zipName);
}

// Test data for extractDir
void TestQuaCompress::extractDir_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<QString>("extractDir");

    QTest::newRow("simple") << "quacompress_extract.zip"
                            << (QStringList() << "test0.txt" << "test1.txt")
                            << "tmp/quacompress_ext/";
    QTest::newRow("3files") << "quacompress_extract3.zip"
                            << (QStringList() << "test0.txt" << "test1.txt" << "test2.txt")
                            << "tmp/quacompress_ext3/";
}

// Test extractDir
void TestQuaCompress::extractDir()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(QString, extractDir);

    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(fileNames)) {
        QFAIL("Can't create test files");
    }

    QStringList fullPaths;
    foreach (QString fileName, fileNames) {
        fullPaths << "tmp/" + fileName;
    }

    // Create archive
    QuaCompress().compressFiles(zipName, fullPaths);

    // Extract
    QStringList extracted = QuaCompress().extractDir(zipName, extractDir);
    QCOMPARE(extracted.size(), fileNames.size());
    foreach (QString fileName, fileNames) {
        QVERIFY(QFile::exists(extractDir + fileName));
    }

    // Cleanup
    removeTestFiles(fileNames);
    curDir.remove(zipName);
    QDir(extractDir).removeRecursively();
}

// Test data for addFile
void TestQuaCompress::addFile_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("initialFile");
    QTest::addColumn<QString>("addedFile");
    QTest::addColumn<bool>("utf8");

    QTest::newRow("simple") << "quacompress_add.zip"
                            << "test0.txt"
                            << "test1.txt"
                            << false;
    QTest::newRow("utf8") << "quacompress_add_utf8.zip"
                          << "test0.txt"
                          << QString::fromUtf8("тест.txt")
                          << true;
}

// Test addFile to existing archive
void TestQuaCompress::addFile()
{
    QFETCH(QString, zipName);
    QFETCH(QString, initialFile);
    QFETCH(QString, addedFile);
    QFETCH(bool, utf8);

    // Skip UTF-8 tests if platform doesn't support UTF-8
    if (utf8 && !isPlatformUtf8()) {
        QSKIP("Skipping UTF-8 test on non-UTF-8 platform");
    }

    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (!createTestFiles(QStringList() << initialFile << addedFile)) {
        QFAIL("Can't create test files");
    }

    // Create initial archive
    QuaCompress().withUtf8Enabled(utf8).compressFile(zipName, "tmp/" + initialFile);

    // Add file
    bool result = QuaCompress()
        .withUtf8Enabled(utf8)
        .addFile(zipName, "tmp/" + addedFile);
    QVERIFY(result);

    // Verify both files are in archive
    QStringList list = QuaCompress().getFileList(zipName);
    QCOMPARE(list.size(), 2);
    QVERIFY(list.contains(initialFile));
    QVERIFY(list.contains(addedFile));

    // Cleanup
    removeTestFiles(QStringList() << initialFile << addedFile);
    curDir.remove(zipName);
}

// Test data for getFileList
void TestQuaCompress::getFileList_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");

    QTest::newRow("2files") << "quacompress_list2.zip"
                            << (QStringList() << "test0.txt" << "test1.txt");
    QTest::newRow("5files") << "quacompress_list5.zip"
                            << (QStringList() << "test0.txt" << "test1.txt" << "test2.txt"
                                              << "test3.txt" << "test4.txt");
}

// Test getFileList
void TestQuaCompress::getFileList()
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

    QStringList fullPaths;
    QStringList shortNames;
    foreach (QString fileName, fileNames) {
        fullPaths << "tmp/" + fileName;
        shortNames << fileName;
    }

    // Create archive
    QuaCompress().compressFiles(zipName, fullPaths);

    // Get file list
    QStringList list = QuaCompress().getFileList(zipName);

    QCOMPARE(list, shortNames);

    // Cleanup
    removeTestFiles(fileNames);
    curDir.remove(zipName);
}
