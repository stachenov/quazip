#include <QDir>
#include <QFile>

#include <quazip/quazip.h>
#include <quazip/quazipfile.h>

/* A simple test program. Creates "test.zip" using the files in the
 * current directory.
 */

// test creating archive
bool testCreate()
{
  QFile zipFile("test.zip");
  QuaZip zip(&zipFile);
  if(!zip.open(QuaZip::mdCreate)) {
    qWarning("testCreate(): zip.open(): %d", zip.getZipError());
    return false;
  }
  zip.setComment("Test comment");
  QFileInfoList files=QDir().entryInfoList();
  QFile inFile;
  QuaZipFile outFile(&zip);
  char c;
  foreach(QFileInfo file, files) {
    if(!file.isFile()||file.fileName()=="test.zip") continue;
    inFile.setFileName(file.fileName());
    if(!inFile.open(QIODevice::ReadOnly)) {
      qWarning("testCreate(): inFile.open(): %s", inFile.errorString().toLocal8Bit().constData());
      return false;
    }
    if(!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(inFile.fileName(), inFile.fileName()))) {
      qWarning("testCreate(): outFile.open(): %d", outFile.getZipError());
      return false;
    }
    qint64 len = file.size();
    qint64 pos = 0;
    while (inFile.getChar(&c)&&outFile.putChar(c)) {
      char buf[4096];
      qint64 l = inFile.read(buf, 4096);
      if (l < 0) {
        qWarning("read(): %s", inFile.errorString().toUtf8().constData());
        break;
      }
      if (l == 0)
        break;
      if (outFile.write(buf, l) != l) {
        qWarning("write(): %d", outFile.getZipError());
        break;
      }
      pos += l;
      if (pos % 1048576 == 0)
        qDebug("%.1f", (float) pos / len * 100.0f);
    }
    if(outFile.getZipError()!=UNZ_OK) {
      qWarning("testCreate(): outFile.putChar(): %d", outFile.getZipError());
      return false;
    }
    outFile.close();
    if(outFile.getZipError()!=UNZ_OK) {
      qWarning("testCreate(): outFile.close(): %d", outFile.getZipError());
      return false;
    }
    inFile.close();
  }
  zip.close();
  if(zip.getZipError()!=0) {
    qWarning("testCreate(): zip.close(): %d", zip.getZipError());
    return false;
  }
  return true;
}

int main()
{
  if(!testCreate()) return 1;
  return 0;
}
