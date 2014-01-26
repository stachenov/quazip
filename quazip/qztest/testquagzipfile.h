#ifndef QUAZIP_TEST_QUAGZIPFILE_H
#define QUAZIP_TEST_QUAGZIPFILE_H

#include <QObject>

class TestQuaGzipFile: public QObject {
    Q_OBJECT
private slots:
    void read();
    void write();
    void constructorDestructor();
};

#endif // QUAZIP_TEST_QUAGZIPFILE_H
