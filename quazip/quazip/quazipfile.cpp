/*
 * QuaZIP - a Qt/C++ wrapper for the ZIP/UNZIP package
 * Copyright (C) 2005 Sergey A. Tachenov
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 **/

#include "quazipfile.h"

using namespace std;

QuaZipFile::QuaZipFile():
  zip(NULL), internal(true), zipError(UNZ_OK)
{
}

QuaZipFile::QuaZipFile(QObject *parent):
  QIODevice(parent), zip(NULL), internal(true), zipError(UNZ_OK)
{
}

QuaZipFile::QuaZipFile(const QString& zipName, QObject *parent):
  QIODevice(parent), internal(true), zipError(UNZ_OK)
{
  zip=new QuaZip(zipName);
  Q_CHECK_PTR(zip);
}

QuaZipFile::QuaZipFile(const QString& zipName, const QString& fileName,
    QuaZip::CaseSensitivity cs, QObject *parent):
  QIODevice(parent), internal(true), zipError(UNZ_OK)
{
  zip=new QuaZip(zipName);
  Q_CHECK_PTR(zip);
  this->fileName=fileName;
  this->caseSensitivity=cs;
}

QuaZipFile::QuaZipFile(QuaZip *zip, QObject *parent):
  QIODevice(parent),
  zip(zip), internal(false),
  zipError(UNZ_OK)
{
}

QuaZipFile::~QuaZipFile()
{
  if(isOpen()) close();
  if(internal) delete zip;
}

QString QuaZipFile::getZipName()const
{
  return zip==NULL?QString():zip->getZipName();
}

QString QuaZipFile::getActualFileName()const
{
  setZipError(UNZ_OK);
  if(zip==NULL) return QString();
  QString name=zip->getCurrentFileName();
  if(name.isNull())
    setZipError(zip->getZipError());
  return name;
}

void QuaZipFile::setZipName(const QString& zipName)
{
  if(isOpen()) {
    qWarning("QuaZipFile::setZipName(): file is already open - can not set ZIP name");
    return;
  }
  if(zip!=NULL&&internal) delete zip;
  zip=new QuaZip(zipName);
  Q_CHECK_PTR(zip);
  internal=true;
}

void QuaZipFile::setZip(QuaZip *zip)
{
  if(isOpen()) {
    qWarning("QuaZipFile::setZip(): file is already open - can not set ZIP");
    return;
  }
  if(this->zip!=NULL&&internal) delete this->zip;
  this->zip=zip;
  this->fileName=QString();
  internal=false;
}

void QuaZipFile::setFileName(const QString& fileName, QuaZip::CaseSensitivity cs)
{
  if(zip==NULL) {
    qWarning("QuaZipFile::setFileName(): call setZipName() first");
    return;
  }
  if(!internal) {
    qWarning("QuaZipFile::setFileName(): should not be used when not using internal QuaZip");
    return;
  }
  if(isOpen()) {
    qWarning("QuaZipFile::setFileName(): can not set file name for already opened file");
    return;
  }
  this->fileName=fileName;
  this->caseSensitivity=cs;
}

void QuaZipFile::setZipError(int zipError)const
{
  QuaZipFile *fakeThis=(QuaZipFile*)this; // non-const
  fakeThis->zipError=zipError;
  if(zipError==UNZ_OK)
    fakeThis->setErrorString(QString());
  else
    fakeThis->setErrorString(tr("ZIP/UNZIP API error %1").arg(zipError));
}

bool QuaZipFile::open(OpenMode mode)
{
  return open(mode, NULL);
}

bool QuaZipFile::open(OpenMode mode, int *method, int *level, bool raw, const char *password)
{
  resetZipError();
  if(isOpen()) {
    qWarning("QuaZipFile::open(): already opened");
    return false;
  }
  if((mode&ReadOnly)&&!(mode&WriteOnly)) {
    if(internal) {
      if(!zip->open(QuaZip::mdUnzip)) {
        setZipError(zip->getZipError());
        return false;
      }
      if(!zip->setCurrentFile(fileName, caseSensitivity)) {
        setZipError(zip->getZipError());
        zip->close();
        return false;
      }
    } else if(zip->getMode()!=QuaZip::mdUnzip) {
      qWarning("QuaZipFile::open(): file open mode %d incompatible with ZIP open mode %d",
          (int)mode, (int)zip->getMode());
      return false;
    }
    setZipError(unzOpenCurrentFile3(zip->getUnzFile(), method, level, raw, password));
    if(zipError==UNZ_OK) {
      setOpenMode(mode);
      return true;
    } else
      return false;
  }
  qWarning("QuaZipFile::open(): open mode %d not supported", (int)mode);
  return false;
}

qint64 QuaZipFile::pos()const
{
  if(zip==NULL) {
    qWarning("QuaZipFile::pos(): call setZipName() or setZip() first");
    return -1;
  }
  return unztell(zip->getUnzFile());
}

bool QuaZipFile::atEnd()const
{
  if(zip==NULL) {
    qWarning("QuaZipFile::atEnd(): call setZipName() or setZip() first");
    return false;
  }
  return unzeof(zip->getUnzFile())==1;
}

void QuaZipFile::close()
{
  resetZipError();
  if(zip->getMode()!=QuaZip::mdUnzip) return;
  if(!isOpen()) {
    qWarning("QuaZipFile::close(): file isn't open");
    return;
  }
  setZipError(unzCloseCurrentFile(zip->getUnzFile()));
  if(zipError==UNZ_OK) setOpenMode(QIODevice::NotOpen);
  else return;
  if(internal) {
    zip->close();
    setZipError(zip->getZipError());
  }
}

qint64 QuaZipFile::readData(char *data, qint64 maxSize)
{
  return unzReadCurrentFile(zip->getUnzFile(), data, (unsigned)maxSize);
}

qint64 QuaZipFile::writeData(const char*, qint64)
{
  qWarning("Write mode not supported");
  return -1;
}
