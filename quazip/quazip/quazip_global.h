#ifndef QUAZIP_GLOBAL_H
#define QUAZIP_GLOBAL_H

#include <QtCore/qglobal.h>

/**
 * When building the library with MSVC, QUAZIP_BUILD must be defined.
 * qglobal.h takes care of defining Q_DECL_* correctly for msvc/gcc.
 */
#if defined(QUAZIP_BUILD)
	#define QUAZIP_EXPORT Q_DECL_EXPORT
#else
	#define QUAZIP_EXPORT Q_DECL_IMPORT
#endif

#ifdef __GNUC__
#define UNUSED __attribute__((__unused__))
#else
#define UNUSED
#endif

#endif // QUAZIP_GLOBAL_H
