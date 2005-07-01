#include <QFile>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

/* A simple test program. Requires "test.zip" and writable "out"
 * directory to be present in the current directory.
 *
 * To test unicode-aware case sensitivity, see testCase() function.
 */

// test reading archive
bool testRead()
{
  QuaZip zip("test.zip");
  if(!zip.open(QuaZip::mdUnzip)) {
    qWarning("testRead(): zip.open(): %d", zip.getZipError());
    return false;
  }
  zip.setFileNameCodec("IBM866");
  printf("%d entries\n", zip.getEntriesCount());
  printf("Global comment: %s\n", zip.getComment().constData());
  QuaZipFileInfo info;
  printf("name\tcver\tnver\tflags\tmethod\tctime\tCRC\tcsize\tusize\tdisknum\tIA\tEA\tcomment\textra\n");
  QuaZipFile file(&zip);
  QFile out;
  QString name;
  char c;
  for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
    if(!zip.getCurrentFileInfo(&info)) {
      qWarning("testRead(): getCurrentFileInfo(): %d\n", zip.getZipError());
      return false;
    }
    printf("%s\t%hu\t%hu\t%hu\t%hu\t%s\t%u\t%u\t%u\t%hu\t%hu\t%u\t%s\t%s\n",
        info.name.toLocal8Bit().constData(),
        info.versionCreated, info.versionNeeded, info.flags, info.method,
        info.dateTime.toString(Qt::ISODate).toLocal8Bit().constData(),
        info.crc, info.compressedSize, info.uncompressedSize, info.diskNumberStart,
        info.internalAttr, info.externalAttr,
        info.comment.constData(), info.extra.constData());
    if(!file.open(QIODevice::ReadOnly)) {
      qWarning("testRead(): file.open(): %d", file.getZipError());
      return false;
    }
    name=file.getActualFileName();
    if(file.getZipError()!=UNZ_OK) {
      qWarning("testRead(): file.getFileName(): %d", file.getZipError());
      return false;
    }
    out.setFileName("out/"+name);
    // this will fail if "name" contains subdirectories, but we don't mind that
    out.open(QIODevice::WriteOnly);
    // Slow like hell (on GNU/Linux at least), but it is not my fault.
    // Not ZIP/UNZIP package's fault either.
    // The slowest thing here is out.putChar(c).
    while(file.getChar(&c)) out.putChar(c);
    out.close();
    if(file.getZipError()!=UNZ_OK) {
      qWarning("testRead(): file.getFileName(): %d", file.getZipError());
      return false;
    }
    if(!file.atEnd()) {
      qWarning("testRead(): read all but not EOF");
      return false;
    }
    file.close();
    if(file.getZipError()!=UNZ_OK) {
      qWarning("testRead(): file.close(): %d", file.getZipError());
      return false;
    }
  }
  zip.close();
  if(zip.getZipError()!=UNZ_OK) {
    qWarning("testRead(): zip.close(): %d", zip.getZipError());
    return false;
  }
  return true;
}

// test unicode-aware case sensitivity
// change the name below before compiling
bool testCase()
{
  QString name=QString::fromUtf8("01_КАФЕ НА ТРОТУАРЕ.OGG");
  QuaZip zip("test.zip");
  if(!zip.open(QuaZip::mdUnzip)) {
    qWarning("testCase(): zip.open(): %d", zip.getZipError());
    return false;
  }
  zip.setFileNameCodec("IBM866");
  if(!zip.setCurrentFile(name, QuaZip::csInsensitive)) {
    if(zip.getZipError()==UNZ_OK)
      qWarning("testCase(): setCurrentFile(): check the file name");
    else
      qWarning("testCase(): setCurrentFile(): %d", zip.getZipError());
    return false;
  }
  if(zip.setCurrentFile(name, QuaZip::csSensitive)) {
    qWarning("testCase(): setCurrentFile(): sets even if the case is wrong");
    return false;
  }
  zip.close();
  return true;
}

int main()
{
  if(!testRead()) return 1;
  if(!testCase()) return 1;
  return 0;
}
