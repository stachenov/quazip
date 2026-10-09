#pragma once

#include <QString>

#ifdef EXAMPLE_BUILD_WRAPPER
#define EXAMPLE_EXPORT Q_DECL_EXPORT
#else
#define EXAMPLE_EXPORT Q_DECL_IMPORT
#endif

extern "C" EXAMPLE_EXPORT bool example_compress(const QString &archive, const QString &input);
