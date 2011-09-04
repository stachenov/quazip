#include "testquazipfile.h"
#include "testquachecksum32.h"
#include "testjlcompress.h"

#include <quazip/jlcompress.h>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>

#include <QtTest/QtTest>

bool createTestFiles(const QStringList &fileNames)
{
    QDir curDir;
    foreach (QString fileName, fileNames) {
        QDir testDir = QFileInfo("tmp/" + fileName).dir();
        if (!testDir.exists()) {
            if (!curDir.mkpath(testDir.path())) {
                qWarning("Couldn't mkpath %s",
                        testDir.path().toUtf8().constData());
                return false;
            }
        }
        QFile testFile("tmp/" + fileName);
        if (!testFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning("Couldn't create %s",
                    fileName.toUtf8().constData());
            return false;
        }
        QTextStream testStream(&testFile);
        testStream << "This is a test file named " << fileName << endl;
    }
    return true;
}

void removeTestFiles(const QStringList &fileNames)
{
    QDir curDir;
    foreach (QString fileName, fileNames) {
        curDir.remove("tmp/" + fileName);
    }
    foreach (QString fileName, fileNames) {
        QDir fileDir = QFileInfo("tmp/" + fileName).dir();
        if (fileDir.exists()) {
            // Non-empty dirs won't get removed, and that's good.
            !curDir.rmpath(fileDir.path());
        }
    }
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    int err = 0;
    TestQuaZipFile testQuaZipFile;
    err = qMax(err, QTest::qExec(&testQuaZipFile, app.arguments()));
    TestQuaChecksum32 testQuaChecksum32;
    err = qMax(err, QTest::qExec(&testQuaChecksum32, app.arguments()));
    TestJlCompress testJlCompress;
    err = qMax(err, QTest::qExec(&testJlCompress, app.arguments()));
    return err;
}

