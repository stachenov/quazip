#ifndef QUAZIP_QUAZIPDIR_H
#define QUAZIP_QUAZIPDIR_H

class QuaZipDirPrivate;

#include "quazip.h"
#include "quazipfileinfo.h"
#include <QDir>
#include <QList>
#include <QSharedDataPointer>

class QUAZIP_EXPORT QuaZipDir {
private:
    QSharedDataPointer<QuaZipDirPrivate> d;
public:
    QuaZipDir(const QuaZipDir &that);
    QuaZipDir(QuaZip *zip, const QString &dir = QString());
    ~QuaZipDir();
    bool operator==(const QuaZipDir &that);
    inline bool operator!=(const QuaZipDir &that) {return !operator==(that);}
    QuaZipDir& operator=(const QuaZipDir &that);
    QString operator[](int pos) const;
    QuaZip::CaseSensitivity caseSensitivity() const;
    bool cd(const QString &dirName);
    bool cdUp();
    uint count() const;
    QString dirName() const;
    QList<QuaZipFileInfo> entryInfoList(const QStringList &nameFilters,
        QDir::Filters filters = QDir::NoFilter,
        QDir::SortFlags sort = QDir::NoSort) const;
    QList<QuaZipFileInfo> entryInfoList(QDir::Filters filters = QDir::NoFilter,
        QDir::SortFlags sort = QDir::NoSort) const;
    QStringList entryList(const QStringList &nameFilters,
        QDir::Filters filters = QDir::NoFilter,
        QDir::SortFlags sort = QDir::NoSort) const;
    QStringList entryList(QDir::Filters filters = QDir::NoFilter,
        QDir::SortFlags sort = QDir::NoSort) const;
    bool exists(const QString &fileName) const;
    bool exists() const;
    QString filePath(const QString &fileName) const;
    QDir::Filters filter();
    bool isRoot() const;
    QStringList nameFilters() const;
    QString path() const;
    QString relativeFilePath(const QString &fileName) const;
    void setCaseSensitivity(QuaZip::CaseSensitivity caseSensitivity);
    void setFilter(QDir::Filters filters);
    void setNameFilters(const QStringList &nameFilters);
    void setPath(const QString &path);
    void setSorting(QDir::SortFlags sort);
    QDir::SortFlags sorting() const;
};

#endif // QUAZIP_QUAZIPDIR_H
