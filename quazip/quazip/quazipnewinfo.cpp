#include <QFileInfo>

#include "quazipnewinfo.h"


QuaZipNewInfo::QuaZipNewInfo(const QString& name):
  name(name), dateTime(QDateTime::currentDateTime()), internalAttr(0), externalAttr(0)
{
}

QuaZipNewInfo::QuaZipNewInfo(const QString& name, const QString& file):
  name(name), internalAttr(0), externalAttr(0)
{
  QFileInfo info(file);
  if(!info.exists()) dateTime=QDateTime::currentDateTime();
  else dateTime=info.lastModified();
}

void QuaZipNewInfo::setFileDateTime(const QString& file)
{
  QFileInfo info(file);
  if(info.exists()) dateTime=info.lastModified();
}
