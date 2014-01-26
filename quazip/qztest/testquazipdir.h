#ifndef QUAZIP_TEST_QUAZIPDIR_H
#define QUAZIP_TEST_QUAZIPDIR_H

#include <QObject>

class TestQuaZipDir: public QObject {
    Q_OBJECT
private slots:
    void entryList_data();
    void entryList();
    void cd_data();
    void cd();
    void entryInfoList();
    void operators();
    void filePath();
};

#endif // QUAZIP_TEST_QUAZIPDIR_H
