# Example Usage

QuaZip provides three API levels with increasing abstraction. Choose the one that best fits your needs.

## Low-Level API (QuaZip/QuaZipFile)

Use when you need fine-grained control over ZIP operations or stream-based processing.

### Compress

```cpp
#include <QuaZip.h>
#include <QuaZipFile.h>
#include <QFile>

// Create a ZIP archive and add a file
QuaZip zip("archive.zip");
if (!zip.open(QuaZip::mdCreate)) {
    // Handle error
    return;
}

QFile inFile("document.txt");
if (!inFile.open(QIODevice::ReadOnly)) {
    return;
}

QuaZipFile outFile(&zip);
// QuaZipNewInfo(name, file): name in archive, file to read metadata from
if (!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(inFile.fileName(), inFile.fileName()))) {
    return;
}

// Copy data
outFile.write(inFile.readAll());

outFile.close();
inFile.close();
zip.close();
```

### Extract

```cpp
#include <QuaZip.h>
#include <QuaZipFile.h>
#include <QFile>
#include <QDir>

// Open ZIP archive and extract all files
QuaZip zip("archive.zip");
if (!zip.open(QuaZip::mdUnzip)) {
    return;
}

// Loop through all files in the archive
for (bool more = zip.goToFirstFile(); more; more = zip.goToNextFile()) {
    QuaZipFile inFile(&zip);
    if (!inFile.open(QIODevice::ReadOnly)) {
        return;
    }

    // Get current file info to get the filename
    QuaZipFileInfo64 fileInfo;
    if (!zip.getCurrentFileInfo(&fileInfo)) {
        return;
    }

    // Create output file
    QString outPath = "output/" + fileInfo.name;
    QDir().mkpath(QFileInfo(outPath).absolutePath());  // Create directories if needed

    QFile outFile(outPath);
    if (!outFile.open(QIODevice::WriteOnly)) {
        return;
    }

    // Copy data
    outFile.write(inFile.readAll());

    outFile.close();
    inFile.close();
}

zip.close();
```

## JlCompress API

Use for simple, common operations with static utility functions.

### Compress

```cpp
#include <JlCompress.h>

// Compress a single file
bool ok = JlCompress::compressFile("archive.zip", "document.txt");

// Compress multiple files
QStringList files = {"file1.txt", "file2.txt", "file3.txt"};
ok = JlCompress::compressFiles("archive.zip", files);

// Compress entire directory
ok = JlCompress::compressDir("archive.zip", "my_folder", true);

// Compress with options (compression level, UTF-8, encryption)
JlCompress::Options opts(QDateTime(), JlCompress::Options::Best, true, QByteArray("password123"));
ok = JlCompress::compressFile("archive.zip", "document.txt", opts);
```

### Extract

```cpp
#include <JlCompress.h>

// Extract a single file
QString extracted = JlCompress::extractFile("archive.zip", "document.txt", "output.txt");

// Extract specific files
QStringList files = {"file1.txt", "file2.txt"};
QStringList extracted = JlCompress::extractFiles("archive.zip", files, "output_dir");

// Extract entire archive
QStringList extracted = JlCompress::extractDir("archive.zip", "output_dir");

// Extract encrypted archive
QString extracted = JlCompress::extractFile("archive.zip", "document.txt", "output.txt", QByteArray("password123"));
```

## QuaCompress API

Use for readable, fluent chaining of operations.

### Compress

```cpp
#include <QuaCompress.h>

// Simple compression with fluent API
bool ok = QuaCompress()
    .compressFile("archive.zip", "document.txt");

// Chain multiple options
ok = QuaCompress()
    .withUtf8Enabled(true)
    .withCompression(JlCompress::Options::Best)
    .withPassword(QByteArray("password123"))
    .compressFile("archive.zip", "document.txt");

// Compress directory with options
ok = QuaCompress()
    .withUtf8Enabled(true)
    .withCompression(JlCompress::Options::Better)
    .compressDir("archive.zip", "my_folder", true);
```

### Extract

```cpp
#include <QuaCompress.h>

// Simple extraction
QStringList files = QuaCompress()
    .extractDir("archive.zip", "output_dir");

// Extract with password using fluent API
files = QuaCompress()
    .withPassword(QByteArray("password123"))
    .extractDir("archive.zip", "output_dir");

// Get file list
QStringList fileNames = QuaCompress()
    .getFileList("archive.zip");
```

## Choosing the Right API

- **Low-Level API**: Use when you need streaming I/O, custom error handling, or per-file control.
- **JlCompress**: Use for simple, one-line operations; static utility pattern.
- **QuaCompress**: Use when you want readable code with method chaining and discoverable options.

All three APIs support UTF-8 filenames, password encryption, and multiple compression levels. JlCompress and QuaCompress provide built-in path traversal protection when extracting archives.
