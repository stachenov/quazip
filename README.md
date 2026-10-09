# Quazip

[![CI](https://github.com/stachenov/quazip/actions/workflows/ci.yml/badge.svg?branch=master)](https://github.com/stachenov/quazip/actions/workflows/ci.yml)
[![CodeQL](https://github.com/stachenov/quazip/actions/workflows/github-code-scanning/codeql/badge.svg)](https://github.com/stachenov/quazip/actions/workflows/github-code-scanning/codeql)
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

> [!IMPORTANT]
> This project is looking for additional maintainers and code reviewers. While the library is mostly feature complete, there are a couple of big ticket issues that need work.
> Code review is welcome for all pull requests. If you are well versed in Qt and C/C++ please start helping around and check [#185](https://github.com/stachenov/quazip/issues/185). 

# Build

## Dependencies
You need at least the following dependencies:
- zlib
- Qt6 or Qt5 (searched in that order)

## Linux
```
sudo apt-get install zlib1g-dev libbz2-dev
cmake -B build
cmake --build build
```

## Windows

When using Qt online installer on Windows with MSVC, make sure to select the box for `MSVC 20XY 64-bit` and under additional libraries, select `Qt 5 Compatibility Module`.
Finally, add `C:\Qt\6.8.2\msvc20XY_64` to your PATH.

If you don't use a package manager you will have to add library and include directories to your PATH or specify them with `CMAKE_PREFIX_PATH`.
Qt is not installed as a dependency of either vcpkg or conan.

### x64
Using vcpkg
```
cmake --preset vcpkg
cmake --build build --config Release
```

Using conan v2
```
conan install . -of build -s build_type=Release -o *:shared=False --build=missing
cmake --preset conan
cmake --build build --config Release
```

### x86
Only Qt5 is tested on x86.

Using vcpkg
```
cmake --preset vcpkg_x86
cmake --build build --config Release
```

Using conan v2
```
conan install . -of build -s build_type=Release -s:h arch=x86 -o *:shared=False --build=missing
cmake --preset conan_x86
cmake --build build --config Release
```

## Additional build options
If you built Qt from source and installed it, you might need to tell CMake where to find it, for example: `-DCMAKE_PREFIX_PATH="/usr/local/Qt-6.8.2"`.  
Alternatively, if you did not install the source build it might look something like: `-DCMAKE_PREFIX_PATH="/home/you/qt-everywhere-src-6.8.2/qtbase/lib/cmake"`.  
Replace `qtbase` if you used a custom prefix at `configure` step.

Qt installed through Linux distribution packages or official Qt online installer should be detected automatically.

CMake is used to configure and build the project. A typical configure, build, install and clean is shown below.

```
cmake -B build -DQUAZIP_QT_MAJOR_VERSION=6 -DBUILD_SHARED_LIBS=ON -DQUAZIP_ENABLE_TESTS=ON
cmake --build build --config Release
sudo cmake --install build
cd build
ctest --verbose -C Release
cmake --build . --target clean
```

CMake options

| Option                      | Description                                                                                                                                                   | Default |
|-----------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| `QUAZIP_QT_MAJOR_VERSION`   | Specifies which major Qt version should be searched for (6 or 5). By default it tries to find the most recent.                                                |         |
| `BUILD_SHARED_LIBS`         | Usual choice for a fresh build: `ON` builds shared, `OFF` builds static. Overridden by the two options below.                                                 | `ON` standalone, `OFF` as a subproject |
| `QUAZIP_BUILD_SHARED`       | Explicitly enable/disable the shared library. Forced off on Emscripten.                                                                                       | Initial value of `BUILD_SHARED_LIBS` |
| `QUAZIP_BUILD_STATIC`       | Explicitly enable/disable the static library. Enable both options to build both variants. Forced on on Emscripten.                                            | Opposite of initial `BUILD_SHARED_LIBS` |
| `QUAZIP_REUSE_OBJECTS`      | Reuse shared-library objects for static output when building both. See visibility tradeoff below; ignored for single-variant builds.                          | `OFF` |
| `QUAZIP_LIB_FILE_NAME`      | Specifies output libname.                                                                                                                                     | `quazip<quazip-version>-qt<qt-version>` |
| `QUAZIP_INSTALL`            | Enable installation                                                                                                                                           | `ON` for standalone builds, `OFF` as a subproject |
| `QUAZIP_USE_QT_ZLIB`        | Use Qt's bundled zlib instead of system zlib (**not recommended**). Qt must be built with `-qt-zlib` and `-static`. Incompatible with `QUAZIP_BUILD_SHARED=ON`. | `OFF`   |
| `QUAZIP_ENABLE_TESTS`       | Build QuaZip tests                                                                                                                                            | `OFF`   |
| `QUAZIP_BZIP2`              | Enable BZIP2 compression                                                                                                                                      | `ON`    |
| `QUAZIP_BZIP2_STDIO`        | Output BZIP2 errors to stdio when BZIP2 compression is enabled                                                                                                | `ON`    |
| `QUAZIP_ENABLE_QTEXTCODEC`  | Set to OFF to explicitely disable the use of QTextCodec on Qt6 even if Core5Compat is available. This uses [QStringConverter](https://doc.qt.io/qt-6/qstringconverter.html) in the background with less supported encodings. | `ON`    |

### Optional combined shared/static builds

QuaZip builds one variant by default. To opt into both:

```sh
cmake -S . -B build-both -DQUAZIP_BUILD_SHARED=ON -DQUAZIP_BUILD_STATIC=ON
cmake --build build-both
cmake --install build-both --prefix /path/to/quazip
```

Each variant compiles separately unless you also set `QUAZIP_REUSE_OBJECTS=ON`.
Reuse compiles once but retains shared export annotations and PIC in the static
archive. Embedding it in another shared library can leak QuaZip symbols, even
with hidden visibility, causing symbol collisions or interposition. On Windows,
objects pulled from the archive retain `__declspec(dllexport)`, exporting their
annotated symbols from consuming DLLs **and executables**; visibility settings
do not remove these annotations. Enable reuse only if you accept this tradeoff.

Targets exposed by `find_package` and `add_subdirectory`:

| Variants built | `QuaZip::QuaZip` | `QuaZip::QuaZip_static` |
|----------------|-----------------|------------------------|
| Shared only | Shared | Unavailable |
| Static only | Static | Unavailable |
| Both | Shared | Static |

Consumer-side `BUILD_SHARED_LIBS` does not change these mappings. To select static
when available, falling back to shared for shared-only packages:

```cmake
find_package(QuaZip-Qt6 CONFIG REQUIRED)
if(TARGET QuaZip::QuaZip_static)
    target_link_libraries(myapp PRIVATE QuaZip::QuaZip_static)
else()
    target_link_libraries(myapp PRIVATE QuaZip::QuaZip)
endif()
```

Exporting a dependency on `QuaZip::QuaZip_static`
requires downstream packages to provide that target too; static-only packages
do not. Packagers should account for these target and visibility tradeoffs,
which downstream users inherit without opting in themselves.

See also [qztest/examples](qztest/examples/README.md).

# Usage

See [EXAMPLES.md](EXAMPLES.md) for code examples showing how to use QuaZip's three API levels: low-level QuaZip/QuaZipFile, JlCompress utility functions, and QuaCompress fluent interface.

# UTF-8 Handling

QuaZip supports setting the UTF-8 flag ZIP file via `JlCompress::Options::setUtf8Enabled()` or `QuaZip::setUtf8Enabled()`. This flag tells extractors that filenames are UTF-8 encoded.

## Behavior Summary

| Platform | Qt Version | UTF-8 Flag | Locale | Result                             | Example: `файл.txt` |
|----------|-----------|-----------|--------|------------------------------------|---------------------|
| **Windows** | Any | ✓ Set | Any | ✓ Works                            | `файл.txt` |
| **Windows** | Any | ✗ Not set | Any | ✗ Mangled (double-encoding or ???) | `Ñ„Ð°Ð¹Ð».txt` |
| **Linux** | Qt 6 | Any | Any | ✓ Works (Qt forces UTF-8)          | `файл.txt` |
| **Linux** | Qt 5 | Any | UTF-8 | ✓ Works                            | `файл.txt` |
| **Linux** | Qt 5 | Any | non-UTF-8 | ✗ Truncated at null bytes          | `.txt` |
