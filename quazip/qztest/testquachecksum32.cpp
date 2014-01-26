#include "testquachecksum32.h"

#include <quazip/quaadler32.h>
#include <quazip/quacrc32.h>

#include <QtTest/QtTest>

void TestQuaChecksum32::calculate()
{
    QuaCrc32 crc32;
    QCOMPARE(crc32.calculate("Wikipedia"), 0xADAAC02Eu);
    QuaAdler32 adler32;
    QCOMPARE(adler32.calculate("Wikipedia"), 0x11E60398u);
}

void TestQuaChecksum32::update()
{
    QuaCrc32 crc32;
    crc32.update("Wiki");
    crc32.update("pedia");
    QCOMPARE(crc32.value(), 0xADAAC02Eu);
    QuaAdler32 adler32;
    adler32.update("Wiki");
    adler32.update("pedia");
    QCOMPARE(adler32.value(), 0x11E60398u);
}
