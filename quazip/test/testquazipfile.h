#ifndef QUAZIP_TEST_QUAZIPFILE_H
#define QUAZIP_TEST_QUAZIPFILE_H

#include <QObject>
#include <QStringList>

class TestQuaZipFile: public QObject {
    Q_OBJECT
private slots:
    void zipUnzip_data();
    void zipUnzip();
};

#endif // QUAZIP_TEST_QUAZIPFILE_H
