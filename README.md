QuaZIP is the C++ wrapper for Gilles Vollant's ZIP/UNZIP package
(AKA Minizip) using Trolltech's Qt library.

If you need to write files to a ZIP archive or read files from one
using QIODevice API, QuaZIP is exactly the kind of tool you need.

You can get started by installing QuaZip:

```shell
git clone https://github.com/stachenov/quazip.git
cd quazip
mkdir build && cd build
cmake ..
make
sudo make install
```

then you can add the following to your project's `CMakeLists.txt`:

```cmake
find_package(QuaZip5 CONFIG REQUIRED)
include_directories(${QUAZIP_INCLUDE_DIRS})
list(APPEND YOUR_PROJECT_LIBRARIES ${QUAZIP_LIBRARIES})
```

See [the documentation](https://stachenov.github.io/quazip/) for details.

Want to report a bug or ask for a feature? Open an [issue](https://github.com/stachenov/quazip/issues).

Want to fix a bug or implement a new feature? See [CONTRIBUTING.md](CONTRIBUTING.md).

Copyright notice:

Copyright (C) 2005-2018 Sergey A. Tachenov

Distributed under LGPL, full details in the COPYING file.

Original ZIP package is copyrighted by Gilles Vollant, see
quazip/(un)zip.h files for details, but basically it's the zlib license.
