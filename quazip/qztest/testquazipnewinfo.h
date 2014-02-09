#ifndef TESTQUAZIPNEWINFO_H
#define TESTQUAZIPNEWINFO_H

#include <QObject>

class TestQuaZipNewInfo : public QObject
{
    Q_OBJECT
public:
    explicit TestQuaZipNewInfo(QObject *parent = 0);
private slots:
    void testSetNTFSTimes();
};

#endif // TESTQUAZIPNEWINFO_H
