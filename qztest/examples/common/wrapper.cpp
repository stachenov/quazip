#include "wrapper.h"

#include <JlCompress.h>

bool example_compress(const QString &archive, const QString &input)
{
    return JlCompress::compressFile(archive, input);
}
