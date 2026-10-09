# Installed-package consumer examples

These are two standalone CMake projects. Neither adds the QuaZip source tree as
a subdirectory; both use `find_package` to consume an installed package, including
its exported include paths, dependencies, and `QUAZIP_STATIC` definition.

- `default` always links `QuaZip::QuaZip`.
- `combined` prefers `QuaZip::QuaZip_static` when it exists, otherwise falls back
  to `QuaZip::QuaZip`. With a combined package, a second executable uses the
  canonical target so both variants are exercised.

The projects share a small application and wrapper under `common`. Each
application compresses a file through its shared wrapper library, then extracts
it with `JlCompress` and checks the content. On Linux, CTest also inspects the wrapper's
defined dynamic symbols with `nm`, checking that QuaZip symbols are absent or
present as expected. Imported symbols do not count as leaks.

## Build and run

From the repository root, build and install the usual shared-only package:

```sh
cmake -S . -B build-package -DQUAZIP_QT_MAJOR_VERSION=6 \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PWD/install-example" \
  -DCMAKE_POSITION_INDEPENDENT_CODE=ON \
  -DCMAKE_C_VISIBILITY_PRESET=hidden \
  -DCMAKE_CXX_VISIBILITY_PRESET=hidden \
  -DCMAKE_VISIBILITY_INLINES_HIDDEN=ON
cmake --build build-package --parallel 2
cmake --install build-package

cmake -S qztest/examples/default -B build-example-default \
  -DCMAKE_PREFIX_PATH="$PWD/install-example" -DQUAZIP_QT_MAJOR_VERSION=6
cmake --build build-example-default --parallel 2
ctest --test-dir build-example-default --verbose

cmake -S qztest/examples/combined -B build-example-combined \
  -DCMAKE_PREFIX_PATH="$PWD/install-example" -DQUAZIP_QT_MAJOR_VERSION=6
cmake --build build-example-combined --parallel 2
ctest --test-dir build-example-combined --verbose
```

Use fresh build directories and install prefixes for each package mode. Add the
following options to the **QuaZip** configure command to try the alternatives:

| Package mode | QuaZip configure options | Combined consumer's selected target | Expected wrapper leaks |
|--------------|--------------------------|-------------------------------------|------------------------|
| Default shared | None | `QuaZip::QuaZip` (shared fallback) | No |
| Static only | `-DBUILD_SHARED_LIBS=OFF` | `QuaZip::QuaZip` (static fallback) | No |
| Both, separately compiled | `-DQUAZIP_BUILD_SHARED=ON -DQUAZIP_BUILD_STATIC=ON` | `QuaZip::QuaZip_static` | No |
| Both, reused objects | `-DQUAZIP_BUILD_SHARED=ON -DQUAZIP_BUILD_STATIC=ON -DQUAZIP_REUSE_OBJECTS=ON` | `QuaZip::QuaZip_static` | Yes |

For the last row, also pass `-DEXAMPLE_EXPECT_SYMBOL_LEAKS=ON` when configuring
the **combined consumer**. This test requires observing leakage; it does not
merely tolerate it. Leave it off for the other package modes. The default example always expects
no leakage, even with a combined package, because its canonical target is shared.

PIC allows a static archive to be embedded in the wrapper. Hidden visibility
must be set when building QuaZip itself: setting it only on the consumer cannot
hide symbols already compiled into the archive. These test flags establish a
baseline in which symbols would stay private without shared export annotations;
they do not change QuaZip's normal build defaults.

The [consumers workflow](../../../.github/workflows/consumers.yml) runs both
projects for every row with Qt 6 on Linux and verifies that installed
targets remain visible outside the directory calling `find_package`.
The symbol inspector tests ELF exports only, not Windows DLL exports. Other
platforms may also need their usual runtime library search path or DLL deployment
setup to run the applications.
