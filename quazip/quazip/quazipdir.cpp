#include "quazipdir.h"

#include <QSharedData>

class QuaZipDirPrivate: public QSharedData {
    friend class QuaZipDir;
private:
    QuaZipDirPrivate(QuaZip *zip, const QString &dir = QString()):
        zip(zip), dir(dir), caseSensitivity(QuaZip::csDefault),
        filter(QDir::NoFilter), sorting(QDir::NoSort) {}
    QuaZip *zip;
    QString dir;
    QuaZip::CaseSensitivity caseSensitivity;
    QDir::Filters filter;
    QStringList nameFilters;
    QDir::SortFlags sorting;
    template<typename TFileInfo>
    bool entryInfoList(QStringList nameFilters, QDir::Filters filter,
        QDir::SortFlags sort, QList<TFileInfo> *result) const;
    inline QString simplePath() const {return QDir::cleanPath(dir);}
};

QuaZipDir::QuaZipDir(const QuaZipDir &that):
    d(that.d)
{
}

QuaZipDir::QuaZipDir(QuaZip *zip, const QString &dir):
    d(new QuaZipDirPrivate(zip, dir))
{
}

QuaZipDir::~QuaZipDir()
{
}

bool QuaZipDir::operator==(const QuaZipDir &that)
{
    return d->zip == that.d->zip && d->dir == that.d->dir;
}

QuaZipDir& QuaZipDir::operator=(const QuaZipDir &that)
{
    this->d = that.d;
    return *this;
}

QString QuaZipDir::operator[](int pos) const
{
    return entryList().at(pos);
}

QuaZip::CaseSensitivity QuaZipDir::caseSensitivity() const
{
    return d->caseSensitivity;
}

bool QuaZipDir::cd(const QString &dirName)
{
    if (!exists(dirName))
        return false;
    d->dir = QDir::cleanPath(QDir(d->dir).filePath(dirName));
    return true;
}

bool QuaZipDir::cdUp()
{
    return cd("..");
}

uint QuaZipDir::count() const
{
    return entryList().count();
}

QString QuaZipDir::dirName() const
{
    return QDir(d->dir).dirName();
}

// copied from quazip.cpp

template<typename TFileInfo>
static TFileInfo getFileInfo(QuaZip *zip, bool *ok);

template<>
static QuaZipFileInfo getFileInfo(QuaZip *zip, bool *ok)
{
    QuaZipFileInfo info;
    *ok = zip->getCurrentFileInfo(&info);
    return info;
}

template<>
static QString getFileInfo(QuaZip *zip, bool *ok)
{
    QString name = zip->getCurrentFileName();
    *ok = !name.isEmpty();
    return name;
}

// utility class to restore the current file
class QuaZipDirRestoreCurrent {
public:
    inline QuaZipDirRestoreCurrent(QuaZip *zip):
        zip(zip), currentFile(zip->getCurrentFileName()) {}
    inline ~QuaZipDirRestoreCurrent()
    {
        zip->setCurrentFile(currentFile);
    }
private:
    QuaZip *zip;
    QString currentFile;
};

template<typename TFileInfo>
bool QuaZipDirPrivate::entryInfoList(QStringList nameFilters, 
    QDir::Filters filter, QDir::SortFlags sort, QList<TFileInfo> *result) const
{
    QString basePath = simplePath() + "/";
    int baseLength = basePath.length();
    result->clear();
    QuaZipDirRestoreCurrent saveCurrent(zip);
    if (!zip->goToFirstFile()) {
        return zip->getZipError() == UNZ_OK;
    }
    QDir::Filters fltr = filter;
    if (fltr == QDir::NoFilter)
        fltr = filter;
    if (fltr == QDir::NoFilter)
        fltr = QDir::AllEntries;
    QStringList nmfltr = nameFilters;
    if (nmfltr.isEmpty())
        nmfltr = nameFilters;
    do {
        QString name = zip->getCurrentFileName();
        if (!name.startsWith(basePath))
            continue;
        QString relativeName = name.mid(baseLength);
        if (relativeName.contains('/'))
            continue;
        if ((fltr & QDir::Dirs) == 0 && relativeName.endsWith('/'))
            continue;
        if ((fltr & QDir::Files) == 0 && !relativeName.endsWith('/'))
            continue;
        if (!nmfltr.isEmpty() && QDir::match(nmfltr, relativeName))
            continue;
        bool ok;
        TFileInfo info = getFileInfo<TFileInfo>(zip, &ok);
        if (!ok) {
            return false;
        }
        result->append(info);
    } while (zip->goToNextFile());
    return true;
}

QList<QuaZipFileInfo> QuaZipDir::entryInfoList(const QStringList &nameFilters,
    QDir::Filters filters, QDir::SortFlags sort) const
{
    QList<QuaZipFileInfo> result;
    if (d->entryInfoList(nameFilters, filters, sort, &result))
        return result;
    else
        return QList<QuaZipFileInfo>();
}

QList<QuaZipFileInfo> QuaZipDir::entryInfoList(QDir::Filters filters,
    QDir::SortFlags sort) const
{
    return entryInfoList(QStringList(), filters, sort);
}

QStringList QuaZipDir::entryList(const QStringList &nameFilters,
    QDir::Filters filters, QDir::SortFlags sort) const
{
    QStringList result;
    if (d->entryInfoList(nameFilters, filters, sort, &result))
        return result;
    else
        return QStringList();
}

QStringList QuaZipDir::entryList(QDir::Filters filters,
    QDir::SortFlags sort) const
{
    return entryList(QStringList(), filters, sort);
}

bool QuaZipDir::exists(const QString &fileName) const
{
    QFileInfo fileInfo(filePath(fileName));
    if (fileName == fileInfo.fileName()) {
        return entryList(QDir::AllEntries, QDir::NoSort).contains(fileName);
    } else {
        return QuaZipDir(d->zip, fileInfo.path()).exists(fileInfo.fileName());
    }
}

bool QuaZipDir::exists() const
{
    QDir thisDir(d->dir);
    return QuaZipDir(d->zip, thisDir.filePath("..")).exists(thisDir.dirName());
}

QString QuaZipDir::filePath(const QString &fileName) const
{
    return QDir(d->dir).filePath(fileName);
}

QDir::Filters QuaZipDir::filter()
{
    return d->filter;
}

bool QuaZipDir::isRoot() const
{
    return d->simplePath().isEmpty();
}

QStringList QuaZipDir::nameFilters() const
{
    return d->nameFilters;
}

QString QuaZipDir::path() const
{
    return d->dir;
}

QString QuaZipDir::relativeFilePath(const QString &fileName) const
{
    return QDir(d->dir).relativeFilePath(fileName);
}

void QuaZipDir::setCaseSensitivity(QuaZip::CaseSensitivity caseSensitivity)
{
    d->caseSensitivity = caseSensitivity;
}

void QuaZipDir::setFilter(QDir::Filters filters)
{
    d->filter = filters;
}

void QuaZipDir::setNameFilters(const QStringList &nameFilters)
{
    d->nameFilters = nameFilters;
}

void QuaZipDir::setPath(const QString &path)
{
    d->dir = path;
}

void QuaZipDir::setSorting(QDir::SortFlags sort)
{
    d->sorting = sort;
}

QDir::SortFlags QuaZipDir::sorting() const
{
    return d->sorting;
}
