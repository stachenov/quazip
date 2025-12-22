#ifndef QUAZIP_TEST_UTF8_DECOMPRESS_H
#define QUAZIP_TEST_UTF8_DECOMPRESS_H

/*
Copyright (C) 2025 cen1

This file is part of QuaZip test suite.

QuaZip is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

QuaZip is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with QuaZip.  If not, see <http://www.gnu.org/licenses/>.

See COPYING file for the full LGPL text.
*/

#include <QtCore/QObject>

class TestUtf8Decompress: public QObject {
    Q_OBJECT
private slots:
    void decompressUtf8Files();
};

#endif // QUAZIP_TEST_UTF8_DECOMPRESS_H
