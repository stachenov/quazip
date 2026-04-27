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
#include <QuaExtract.h>

Q_DECLARE_METATYPE(QuaCompress::CompressionStrategy)

// ==================== TestQuaCompress ====================

// Test data for compressFile - comprehensive matrix of options
void TestQuaCompress::compressFile_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<bool>("utf8");
    QTest::addColumn<QuaCompress::CompressionStrategy>("compression");
    QTest::addColumn<QByteArray>("password");

    // Basic tests with different compression levels
    QTest::newRow("simple") << "quacompress_simple.zip"
                            << "test0.txt"
                            << false
                            << QuaCompress::Default
                            << QByteArray();
    QTest::newRow("storage") << "quacompress_storage.zip"
                             << "test0.txt"
                             << false
                             << QuaCompress::Storage
                             << QByteArray();
    QTest::newRow("fastest") << "quacompress_fastest.zip"
                             << "test0.txt"
                             << false
                             << QuaCompress::Fastest
                             << QByteArray();
    QTest::newRow("best") << "quacompress_best.zip"
                          << "test0.txt"
                          << false
                          << QuaCompress::Best
                          << QByteArray();

    // UTF-8 tests with different scripts and compression levels
    QTest::newRow("utf8-japanese-best") << "quacompress_utf8_ja.zip"
                                        << QString::fromUtf8("ありがとう.txt")
                                        << true
                                        << QuaCompress::Best
                          << QByteArray();
    QTest::newRow("utf8-cyrillic-default") << "quacompress_utf8_ru.zip"
                                           << QString::fromUtf8("Привет.txt")
                                           << true
                                           << QuaCompress::Default
                                           << QByteArray();
    QTest::newRow("utf8-chinese-fastest") << "quacompress_utf8_zh.zip"
                                          << QString::fromUtf8("你好.txt")
                                          << true
                                          << QuaCompress::Fastest
                             << QByteArray();

    // Password tests
    QTest::newRow("password-simple") << "quacompress_password.zip"
                                     << "test0.txt"
                                     << false
                                     << QuaCompress::Default
                                     << QByteArray("password123");
    QTest::newRow("password-utf8-best") << "quacompress_password_utf8.zip"
                                        << QString::fromUtf8("тест.txt")
                                        << true
                                        << QuaCompress::Best
                                        << QByteArray("секрет");
}

// Test compressFile with various options
void TestQuaCompress::compressFile()
{
    QFETCH(QString, zipName);
    QFETCH(QString, fileName);
    QFETCH(bool, utf8);
    QFETCH(QuaCompress::CompressionStrategy, compression);
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
        .withStrategy(compression)
        .withPassword(password)
        .compressFile(zipName, "tmp/" + fileName);
    QVERIFY(result);
    QVERIFY(QFile::exists(zipName));

    // Verify file list (works without password)
    QStringList fileList = QuaExtract().getFileList(zipName);
    QCOMPARE(fileList.count(), 1);
    QCOMPARE(fileList[0], fileName);

    // Verify archive can be extracted
    QStringList extracted;
    if (!password.isEmpty()) {
        // Test fluent API extraction
        extracted = QuaExtract()
            .withPassword(password)
            .extractDir(zipName, "tmp/quacompress_extracted");
        QCOMPARE(extracted.size(), 1);
        QVERIFY(QFile::exists("tmp/quacompress_extracted/" + fileName));

        // Test wrong password fails
        QStringList wrongExtract = QuaExtract()
            .withPassword(QByteArray("wrong"))
            .extractDir(zipName, "tmp/quacompress_wrong");
        QCOMPARE(wrongExtract.size(), 0);
        QDir("tmp/quacompress_wrong").removeRecursively();
    } else {
        // No password
        extracted = QuaExtract().extractDir(zipName, "tmp/quacompress_extracted");
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
    QStringList list = QuaExtract().getFileList(zipName);
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
    QStringList list = QuaExtract().getFileList(zipName);
    QVERIFY(list.size() >= expectedMinFiles);

    // Cleanup
    removeTestFiles(fileNames);
    curDir.remove(zipName);
}

// Test data for addFile
void TestQuaCompress::addFile_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("initialFile");
    QTest::addColumn<QString>("addedFile");
    QTest::addColumn<bool>("utf8");

    QTest::newRow("simple") << "quacompress_addfile.zip"
                            << "test0.txt"
                            << "test1.txt"
                            << false;
    QTest::newRow("utf8") << "quacompress_addfile_utf8.zip"
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

    // Create initial archive with one file
    bool result = QuaCompress()
        .withUtf8Enabled(utf8)
        .compressFile(zipName, "tmp/" + initialFile);
    QVERIFY(result);

    // Verify initial archive has 1 file
    QStringList list = QuaExtract().getFileList(zipName);
    QCOMPARE(list.size(), 1);
    QVERIFY(list.contains(initialFile));

    // Add another file to the archive
    result = QuaCompress()
        .withUtf8Enabled(utf8)
        .addFile(zipName, "tmp/" + addedFile);
    QVERIFY(result);

    // Verify archive now has 2 files
    list = QuaExtract().getFileList(zipName);
    QCOMPARE(list.size(), 2);
    QVERIFY(list.contains(initialFile));
    QVERIFY(list.contains(addedFile));

    // Cleanup
    removeTestFiles(QStringList() << initialFile << addedFile);
    curDir.remove(zipName);
}

// Test data for addFiles
void TestQuaCompress::addFiles_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("initialFiles");
    QTest::addColumn<QStringList>("addedFiles");
    QTest::addColumn<bool>("utf8");

    QTest::newRow("simple") << "quacompress_addfiles.zip"
                            << (QStringList() << "test0.txt")
                            << (QStringList() << "test1.txt" << "test2.txt")
                            << false;
    QTest::newRow("utf8") << "quacompress_addfiles_utf8.zip"
                          << (QStringList() << "test0.txt")
                          << (QStringList() << QString::fromUtf8("файл1.txt") << QString::fromUtf8("файл2.txt"))
                          << true;
}

// Test addFiles to existing archive
void TestQuaCompress::addFiles()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, initialFiles);
    QFETCH(QStringList, addedFiles);
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

    QStringList allFiles = initialFiles + addedFiles;
    if (!createTestFiles(allFiles)) {
        QFAIL("Can't create test files");
    }

    // Create initial archive
    QStringList initialPaths;
    foreach (QString fileName, initialFiles) {
        initialPaths << "tmp/" + fileName;
    }
    bool result = QuaCompress()
        .withUtf8Enabled(utf8)
        .compressFiles(zipName, initialPaths);
    QVERIFY(result);

    // Add more files
    QStringList addedPaths;
    foreach (QString fileName, addedFiles) {
        addedPaths << "tmp/" + fileName;
    }
    result = QuaCompress()
        .withUtf8Enabled(utf8)
        .addFiles(zipName, addedPaths);
    QVERIFY(result);

    // Verify all files are in archive
    QStringList list = QuaExtract().getFileList(zipName);
    QCOMPARE(list.size(), allFiles.size());
    foreach (QString fileName, allFiles) {
        QVERIFY(list.contains(fileName));
    }

    // Cleanup
    removeTestFiles(allFiles);
    curDir.remove(zipName);
}

// Test data for addDir
void TestQuaCompress::addDir_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("initialFile");
    QTest::addColumn<QStringList>("dirFiles");
    QTest::addColumn<int>("expectedTotalFiles");
    QTest::addColumn<bool>("utf8");

    QTest::newRow("simple") << "quacompress_adddir.zip"
                            << "test0.txt"
                            << (QStringList() << "testdir1/test1.txt" << "testdir1/test2.txt")
                            << 3  // 1 initial + 2 added
                            << false;
    QTest::newRow("utf8") << "quacompress_adddir_utf8.zip"
                          << "test0.txt"
                          << (QStringList() << "testdir1/test1.txt")
                          << 2  // 1 initial + 1 added
                          << true;
}

// Test addDir to existing archive
void TestQuaCompress::addDir()
{
    QFETCH(QString, zipName);
    QFETCH(QString, initialFile);
    QFETCH(QStringList, dirFiles);
    QFETCH(int, expectedTotalFiles);
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

    QStringList allFiles = QStringList() << initialFile;
    allFiles << dirFiles;
    if (!createTestFiles(allFiles)) {
        QFAIL("Can't create test files");
    }

    // Create initial archive with one file
    bool result = QuaCompress()
        .withUtf8Enabled(utf8)
        .compressFile(zipName, "tmp/" + initialFile);
    QVERIFY(result);

    // Add a directory to the archive
    result = QuaCompress()
        .withUtf8Enabled(utf8)
        .addDir(zipName, "tmp/testdir1", true);
    QVERIFY(result);

    // Verify archive has all files
    QStringList list = QuaExtract().getFileList(zipName);
    QVERIFY(list.size() >= expectedTotalFiles);

    // Cleanup
    removeTestFiles(allFiles);
    curDir.remove(zipName);
}

// ==================== TestQuaExtract ====================

// Test data for extractFile
void TestQuaExtract::extractFile_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QString>("extractPath");
    QTest::addColumn<bool>("usePassword");
    QTest::addColumn<QByteArray>("password");

    QTest::newRow("simple") << "quaextract_file.zip"
                            << "test0.txt"
                            << "tmp/quaextract_out.txt"
                            << false
                            << QByteArray();
    QTest::newRow("with-password") << "quaextract_file_pwd.zip"
                                   << "test1.txt"
                                   << "tmp/quaextract_pwd_out.txt"
                                   << true
                                   << QByteArray("secret123");
}

// Test extractFile
void TestQuaExtract::extractFile()
{
    QFETCH(QString, zipName);
    QFETCH(QString, fileName);
    QFETCH(QString, extractPath);
    QFETCH(bool, usePassword);
    QFETCH(QByteArray, password);

    QDir curDir;
    if (curDir.exists(zipName)) {
        if (!curDir.remove(zipName))
            QFAIL("Can't remove zip file");
    }
    if (QFile::exists(extractPath)) {
        if (!QFile::remove(extractPath))
            QFAIL("Can't remove extracted file");
    }
    if (!createTestFiles(QStringList() << fileName)) {
        QFAIL("Can't create test file");
    }

    // Create archive
    QuaCompress compressor;
    compressor.withUtf8Enabled();
    if (usePassword) {
        compressor.withPassword(password);
    }
    bool result = compressor.compressFile(zipName, "tmp/" + fileName);
    QVERIFY(result);

    // Extract single file
    QString extracted;
    if (usePassword) {
        extracted = QuaExtract()
            .withPassword(password)
            .extractFile(zipName, fileName, extractPath);
    } else {
        extracted = QuaExtract()
            .extractFile(zipName, fileName, extractPath);
    }

    QVERIFY(!extracted.isEmpty());
    QVERIFY(QFile::exists(extractPath));

    // Test wrong password fails
    if (usePassword) {
        QString wrongExtract = QuaExtract()
            .withPassword(QByteArray("wrong"))
            .extractFile(zipName, fileName, "tmp/wrong.txt");
        QVERIFY(wrongExtract.isEmpty());
    }

    // Cleanup
    removeTestFiles(QStringList() << fileName);
    curDir.remove(zipName);
    QFile::remove(extractPath);
    QFile::remove("tmp/wrong.txt");
}

// Test data for extractFiles
void TestQuaExtract::extractFiles_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<QStringList>("filesToExtract");
    QTest::addColumn<QString>("extractDir");

    QTest::newRow("extract-some") << "quaextract_files_some.zip"
                                  << (QStringList() << "test0.txt" << "test1.txt" << "test2.txt")
                                  << (QStringList() << "test0.txt" << "test2.txt")  // Only extract 2 of 3
                                  << "tmp/quaextract_some/";
    QTest::newRow("extract-single") << "quaextract_files_single.zip"
                                    << (QStringList() << "test0.txt" << "test1.txt")
                                    << (QStringList() << "test0.txt")  // Only extract 1 of 2
                                    << "tmp/quaextract_single/";
}

// Test extractFiles
void TestQuaExtract::extractFiles()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(QStringList, filesToExtract);
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

    // Create archive with all files
    bool result = QuaCompress().compressFiles(zipName, fullPaths);
    QVERIFY(result);

    // Extract specific files
    QStringList extracted = QuaExtract().extractFiles(zipName, filesToExtract, extractDir);

    // Verify extraction
    QCOMPARE(extracted.size(), filesToExtract.size());

    // Verify extracted files exist
    foreach (QString fileName, filesToExtract) {
        QVERIFY(QFile::exists(extractDir + fileName));
    }

    // Verify non-extracted files don't exist
    foreach (QString fileName, fileNames) {
        if (!filesToExtract.contains(fileName)) {
            QVERIFY(!QFile::exists(extractDir + fileName));
        }
    }

    // Cleanup
    removeTestFiles(fileNames);
    curDir.remove(zipName);
    QDir(extractDir).removeRecursively();
}

// Test data for extractDir
void TestQuaExtract::extractDir_data()
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
void TestQuaExtract::extractDir()
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
    QStringList extracted = QuaExtract().extractDir(zipName, extractDir);
    QCOMPARE(extracted.size(), fileNames.size());
    foreach (QString fileName, fileNames) {
        QVERIFY(QFile::exists(extractDir + fileName));
    }

    // Cleanup
    removeTestFiles(fileNames);
    curDir.remove(zipName);
    QDir(extractDir).removeRecursively();
}

// Test data for getFileList
void TestQuaExtract::getFileList_data()
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
void TestQuaExtract::getFileList()
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
    QStringList list = QuaExtract().getFileList(zipName);

    QCOMPARE(list, shortNames);

    // Cleanup
    removeTestFiles(fileNames);
    curDir.remove(zipName);
}
