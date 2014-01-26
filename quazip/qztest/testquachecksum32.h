#ifndef QUAZIP_TEST_QUACHECKSUM32_H
#define QUAZIP_TEST_QUACHECKSUM32_H

#include <QObject>

class TestQuaChecksum32: public QObject {
    Q_OBJECT
private slots:
    void calculate();
    void update();
};

#endif // QUAZIP_TEST_QUACHECKSUM32_H
