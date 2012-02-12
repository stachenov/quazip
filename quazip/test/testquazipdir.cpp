#include "testquazipdir.h"
#include "qztest.h"
#include <QtTest/QtTest>
#include <quazip/quazip.h>
#include <quazip/quazipdir.h>

void TestQuaZipDir::entryList_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<QString>("dirName");
    // QDir::Filters type breaks Qt meta type system on MSVC
    QTest::addColumn<int>("filter");
    QTest::addColumn<QStringList>("entries");
    QTest::newRow("simple") << "simple.zip" << (
            QStringList() << "test0.txt" << "testdir1/test1.txt"
            << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
            << "testdir2" << static_cast<int>(QDir::NoFilter) <<
            (QStringList() << "test2.txt" << "subdir");
    QTest::newRow("separate dir") << "sepdir.zip" << (
            QStringList() << "laj/" << "laj/lajfile.txt")
            << "" << static_cast<int>(QDir::NoFilter)
            << (QStringList() << "laj");
    QTest::newRow("dirs only") << "dirsonly.zip" << (
            QStringList() << "file" << "dir/")
            << "" << static_cast<int>(QDir::Dirs)
            << (QStringList() << "dir");
    QTest::newRow("files only") << "filesonly.zip" << (
            QStringList() << "file1" << "parent/dir/" << "parent/file2")
            << "parent" << static_cast<int>(QDir::Files)
            << (QStringList() << "file2");
}

void TestQuaZipDir::entryList()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(QString, dirName);
    QFETCH(int, filter);
    QDir::Filters filters = static_cast<QDir::Filters>(filter);
    QFETCH(QStringList, entries);
    QDir curDir;
    if (!curDir.mkpath("jlext/jldir")) {
        QFAIL("Couldn't mkpath jlext/jldir");
    }
    if (!createTestFiles(fileNames)) {
        QFAIL("Couldn't create test files");
    }
    if (!createTestArchive(zipName, fileNames)) {
        QFAIL("Couldn't create test archive");
    }
    QuaZip zip(zipName);
    QVERIFY(zip.open(QuaZip::mdUnzip));
    QuaZipDir dir(&zip, dirName);
    QCOMPARE(dir.entryList(filters), entries);
    zip.close();
    curDir.rmpath("jlext/jldir");
    removeTestFiles(fileNames);
    curDir.remove(zipName);
}
