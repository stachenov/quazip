# Quazip

[![CI](https://github.com/stachenov/quazip/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/stachenov/quazip/actions/workflows/ci.yml)
[![Conan Center](https://shields.io/conan/v/quazip)](https://conan.io/center/recipes/quazip)

QuaZip is the C++ wrapper for Gilles Vollant's ZIP/UNZIP package
(AKA Minizip) using Qt library.

If you need to write files to a ZIP archive or read files from one
using QIODevice API, QuaZip is exactly the kind of tool you need.

See [the documentation](https://stachenov.github.io/quazip/) for details.

Want to report a bug or ask for a feature? Open an [issue](https://github.com/stachenov/quazip/issues).

Want to fix a bug or implement a new feature? See [CONTRIBUTING.md](CONTRIBUTING.md).

You're a package maintainer and want to update to QuaZip 1.0? Read [the migration guide](https://github.com/stachenov/quazip/blob/master/QuaZip-1.x-migration.md).

Copyright notice:

Copyright (C) 2005-2020 Sergey A. Tachenov and contributors

Distributed under LGPL, full details in the COPYING file.

Original ZIP package is copyrighted by Gilles Vollant, see
quazip/(un)zip.h files for details, but basically it's the zlib license.

## Build

You need at least the following dependencies:
- zlib
- Qt6, Qt5 or Qt4 (searched in that order)

If you built Qt from source and installed it, you might need to tell CMake where to find it, for example: `-DCMAKE_PREFIX_PATH="/usr/local/Qt-6.6.2"`.  
Alternatively, if you did not install the source build it might look something like: `-DCMAKE_PREFIX_PATH="/home/you/qt-everywhere-src-6.6.2/qtbase/lib/cmake"`.  
Replace `qtbase` if you used a custom prefix at `configure` step.

Qt installed through Linux distribution packages or official Qt online installer should be detected automatically.

CMake is used to configure and build the project.

```
cmake -B build
cmake --build build --config Release
sudo cmake --install build
cmake --build build --target clean
```

CMake options

| Option                   | Description                                                                                                       | Default |
|--------------------------|-------------------------------------------------------------------------------------------------------------------|---------|
| `QUAZIP_QT_MAJOR_VERSION`| Specifies which major Qt version should be searched for (6, 5 or 4). By default it tries to find the most recent. |         |
| `BUILD_SHARED_LIBS`      | Build QuaZip as a shared library                                                                                  | `ON`    |
| `QUAZIP_INSTALL`         | Enable installation                                                                                               | `ON`    |
| `QUAZIP_USE_QT_ZLIB`     | Use Qt's bundled zlib instead of system zlib (**not recommended**). Qt must be built with `-qt-zlib`.             | `OFF`   |
| `QUAZIP_ENABLE_TESTS`    | Build QuaZip tests                                                                                                | `OFF`   |
| `QUAZIP_BZIP2`           | Enable BZIP2 compression                                                                                          | `ON`    |
| `QUAZIP_BZIP2_STDIO`     | Output BZIP2 errors to stdio when BZIP2 compression is enabled                                                    | `ON`    |

