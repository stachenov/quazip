QuaZIP is the C++ wrapper for Gilles Vollant's ZIP/UNZIP package
(AKA minizip) using Trolltech's Qt library.

It uses existing ZIP/UNZIP package C code and therefore depends on
the zlib library.

Also, it depends on Qt 4/5.

To compile it on UNIX dialect:

```
$ cd quazip
$ qmake
$ make
```

You must make sure that:
* You have Qt 4/5 properly and fully installed (including tools and
  headers, not just library).
* "qmake" command runs the right qmake, not some older version installed in parallel
  (you'll have to type full path to qmake otherwise).

To install compiled shared library, just type:

```
$ make install
```

By default, it installs in /usr/local, but you may change it using

```
$ qmake PREFIX=/wherever/you/want/to/install
```

You do not have to compile and install QuaZIP to use it. You can just
(and sometimes it may be the best way) add QuaZIP's source files to your
project and use them.

See doc/html or, if you do not have a browser, quazip/\*.h and
quazip/doc/\* files for the more detailed documentation.

For Windows, it's essentially the same, but you may have to adjust
settings for different environments.

If linking statically (either a static lib or just using the source code
directly in your project), then QUAZIP_STATIC should be defined. This is
done automatically when you build QuaZIP as a static library with `CONFIG += staticlib`. However,
when _using_ a static lib (or source code, for that matter) you must
also define QUAZIP_STATIC in your project (that uses QuaZIP) to tell
quazip_global.h that you use a static version because otherwise the
compiler wouldn't know that and will mark QuaZIP symbols as dllimported.
Linking problems among the lines of “undefined reference” are usually
caused by this.

Copyright notice:

Copyright (C) 2005-2018 Sergey A. Tachenov

Distributed under LGPL, full details in the COPYING file.

Original ZIP package is copyrighted by Gilles Vollant, see
quazip/(un)zip.h files for details, but basically it's the zlib license.
