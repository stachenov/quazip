#include "wrapper.h"

#include <JlCompress.h>

#include <QCoreApplication>
#include <QFile>
#include <QTemporaryDir>

// Verify the installed target propagates QUAZIP_STATIC to its consumers.
#if defined(EXAMPLE_EXPECT_STATIC) != defined(QUAZIP_STATIC)
#error Incorrect QUAZIP_STATIC definition from the installed QuaZip target
#endif

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    const QByteArray payload("QuaZip");

    QTemporaryDir directory;
    if (!directory.isValid())
        return 1;

    QFile input(directory.filePath("payload.txt"));
    if (!input.open(QIODevice::WriteOnly) || input.write(payload) != payload.size())
        return 1;
    input.close();

    const QString archive = directory.filePath("example.zip");
    if (!example_compress(archive, input.fileName()))
        return 1;
    QFile output(JlCompress::extractFile(archive, "payload.txt",
                                        directory.filePath("extracted.txt")));
    return output.open(QIODevice::ReadOnly) && output.readAll() == payload ? 0 : 1;
}
