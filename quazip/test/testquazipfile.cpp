#include "testquazipfile.h"

#include "qztest.h"

#include <quazip/quazipfile.h>
#include <quazip/quazip.h>

#include <QFile>
#include <QString>

#include <QtTest/QtTest>

void TestQuaZipFile::zipUnzip_data()
{
    QTest::addColumn<QString>("zipName");
    QTest::addColumn<QStringList>("fileNames");
    QTest::addColumn<QByteArray>("fileNameCodec");
    QTest::newRow("simple") << "simple.zip" << (
            QStringList() << "test0.txt" << "testdir1/test1.txt"
            << "testdir2/test2.txt" << "testdir2/subdir/test2sub.txt")
        << QByteArray();
    QTest::newRow("Cyrillic") << "cyrillic.zip" << (
            QStringList()
            << QString::fromUtf8("русское имя файла с пробелами.txt"))
        << QByteArray("IBM866");
}

void TestQuaZipFile::zipUnzip()
{
    QFETCH(QString, zipName);
    QFETCH(QStringList, fileNames);
    QFETCH(QByteArray, fileNameCodec);
    QFile testFile(zipName);
    if (testFile.exists()) {
        if (!testFile.remove()) {
            QFAIL("Couldn't remove existing archive to create a new one");
        }
    }
    if (!createTestFiles(fileNames)) {
        QFAIL("Couldn't create test files for zipping");
    }
    QuaZip testZip(&testFile);
    if (!fileNameCodec.isEmpty())
        testZip.setFileNameCodec(fileNameCodec);
    QVERIFY(testZip.open(QuaZip::mdCreate));
    QString comment = "Test comment";
    testZip.setComment(comment);
    foreach (QString fileName, fileNames) {
        QFile inFile("tmp/" + fileName);
        if (!inFile.open(QIODevice::ReadOnly)) {
            qDebug("File name: %s", fileName.toUtf8().constData());
            QFAIL("Couldn't open input file");
        }
        QuaZipFile outFile(&testZip);
        QVERIFY(outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileName,
                        inFile.fileName())));
        for (qint64 pos = 0, len = inFile.size(); pos < len; ) {
            char buf[4096];
            qint64 readSize = qMin(static_cast<qint64>(4096), len - pos);
            qint64 l;
            if ((l = inFile.read(buf, readSize)) != readSize) {
                qDebug("Reading %ld bytes from %s at %ld returned %ld",
                        static_cast<long>(readSize),
                        fileName.toUtf8().constData(),
                        static_cast<long>(pos),
                        static_cast<long>(l));
                QFAIL("Read failure");
            }
            QVERIFY(outFile.write(buf, readSize));
            pos += readSize;
        }
        inFile.close();
        outFile.close();
        QCOMPARE(outFile.getZipError(), ZIP_OK);
    }
    testZip.close();
    QCOMPARE(testZip.getZipError(), ZIP_OK);
    // now test unzip
    QuaZip testUnzip(&testFile);
    if (!fileNameCodec.isEmpty())
        testUnzip.setFileNameCodec(fileNameCodec);
    QVERIFY(testUnzip.open(QuaZip::mdUnzip));
    QCOMPARE(testUnzip.getComment(), comment);
    QVERIFY(testUnzip.goToFirstFile());
    foreach (QString fileName, fileNames) {
        QCOMPARE(testUnzip.getCurrentFileName(), fileName);
        testUnzip.goToNextFile();
    }
    testUnzip.close();
    QCOMPARE(testUnzip.getZipError(), UNZ_OK);
    // clean up
    removeTestFiles(fileNames);
}
