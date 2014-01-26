#include "testquagzipfile.h"
#include <zlib.h>
#include <QDir>
#include <quazip/quagzipfile.h>
#include <QtTest/QtTest>

void TestQuaGzipFile::read()
{
    QDir curDir;
    curDir.mkpath("tmp");
    gzFile file = gzopen("tmp/test.gz", "wb");
    gzwrite(file, "test", 4);
    gzclose(file);
    QuaGzipFile testFile("tmp/test.gz");
    QVERIFY(testFile.open(QIODevice::ReadOnly));
    char buf[5];
    buf[4] = '\0';
    QCOMPARE(testFile.read(buf, 5), static_cast<qint64>(4));
    testFile.close();
    QVERIFY(!testFile.isOpen());
    QCOMPARE(static_cast<const char*>(buf), "test");
    curDir.remove("tmp/test.gz");
    curDir.rmdir("tmp");
}

void TestQuaGzipFile::write()
{
    QDir curDir;
    curDir.mkpath("tmp");
    QuaGzipFile testFile;
    testFile.setFileName("tmp/test.gz");
    QCOMPARE(testFile.getFileName(), QString::fromLatin1("tmp/test.gz"));
    QVERIFY(testFile.open(QIODevice::WriteOnly));
    QCOMPARE(testFile.write("test", 4), static_cast<qint64>(4));
    QVERIFY(testFile.flush());
    testFile.close();
    QVERIFY(!testFile.isOpen());
    gzFile file = gzopen("tmp/test.gz", "rb");
    char buf[5];
    buf[4] = '\0';
    QCOMPARE(gzread(file, buf, 5), 4);
    gzclose(file);
    QCOMPARE(static_cast<const char*>(buf), "test");
    curDir.remove("tmp/test.gz");
    curDir.rmdir("tmp");
}

void TestQuaGzipFile::constructorDestructor()
{
    QuaGzipFile *f1 = new QuaGzipFile();
    delete f1; // D0 destructor
    QObject parent;
    QuaGzipFile f2(&parent);
    QuaGzipFile f3("test.gz", &parent);
}
