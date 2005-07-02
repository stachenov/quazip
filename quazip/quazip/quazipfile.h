#ifndef QUA_ZIPFILE_H
#define QUA_ZIPFILE_H

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

#include <QIODevice>
#include "quazip.h"

/// A file inside ZIP archive.
/** \class QuaZipFile quazipfile.h <quazip/quazipfile.h>
 * This is the most interesting class. Not only it provides C++
 * interface to the ZIP/UNZIP package, but also integrates it with Qt by
 * subclassing QIODevice. This makes possible to access files inside ZIP
 * archive using QTextStream or QDataStream, for example. Actually, this
 * is the main purpose of the whole QuaZIP library.
 *
 * You can either use existing QuaZip instance to create instance of
 * this class or pass ZIP archive file name to this class, in which case
 * it will create internal QuaZip object. See constructors' descriptions
 * for details.
 *
 * \section quazipfile-sequential Sequential or random-access?
 *
 * At the first thought, QuaZipFile has fixed size, the start and the
 * end and should be therefore considered random-access device. But
 * there is one major obstacle to making it random-access: ZIP/UNZIP API
 * does not support seek() operation and the only way to implement it is
 * through reopening the file and re-reading to the required position,
 * but this is prohibitely slow.
 *
 * Therefore, QuaZipFile is considered to be a sequential device. This
 * has advantage of availability of the ungetChar() operation (QIODevice
 * does not implement it properly for non-sequential devices unless they
 * support seek()). Disadvantage is a somewhat strange behaviour of the
 * size() and pos() functions. This should be kept in mind while using
 * this class.
 *
 **/
class QuaZipFile: public QIODevice {
  Q_OBJECT
  private:
    QuaZip *zip;
    QString fileName;
    QuaZip::CaseSensitivity caseSensitivity;
    bool raw;
    bool internal;
    int zipError;
    // these are not supported nor implemented
    QuaZipFile(const QuaZipFile& that);
    QuaZipFile& operator=(const QuaZipFile& that);
    void resetZipError()const {setZipError(UNZ_OK);}
    // const, but sets zipError!
    void setZipError(int zipError)const;
  protected:
    /// Implementation of the QIODevice::readData().
    qint64 readData(char *data, qint64 maxSize);
    /// Implementation of the QIODevice::writeData().
    /** Currenlty always returns -1 (because ZIP is not supported yet).
     **/
    qint64 writeData(const char *data, qint64 maxSize);
  public:
    /// Constructs a QuaZipFile instance.
    /** You should use setZipName() and setFileName() before trying to
     * call open() on the constructed object.
     **/
    QuaZipFile();
    /// Constructs a QuaZipFile instance.
    /** \a parent argument specifies this object's parent object.
     *
     * You should use setZipName() and setFileName() before trying to
     * call open() on the constructed object.
     **/
    QuaZipFile(QObject *parent);
    /// Constructs a QuaZipFile instance.
    /** \a parent argument specifies this object's parent object and \a
     * zipName specifies ZIP archive file name.
     *
     * You should use setFileName() before trying to call open() on the
     * constructed object.
     **/
    QuaZipFile(const QString& zipName, QObject *parent =NULL);
    /// Constructs a QuaZipFile instance.
    /** \a parent argument specifies this object's parent object, \a
     * zipName specifies ZIP archive file name and \a fileName and \a cs
     * specify a name of the file to open inside archive.
     *
     * \sa QuaZip::setCurrentFile()
     **/
    QuaZipFile(const QString& zipName, const QString& fileName,
        QuaZip::CaseSensitivity cs =QuaZip::csDefault, QObject *parent =NULL);
    /// Constructs a QuaZipFile instance.
    /** \a parent argument specifies this object's parent object.
     *
     * \a zip is the pointer to the existing QuaZip object. This
     * QuaZipFile object then can be used to access current file in the
     * \a zip.
     *
     * \warning Using this constructor can be tricky. Let's take the
     * following example:
     * \code
     * QuaZip zip("archive.zip");
     * zip.open(QuaZip::mdUnzip);
     * zip.setCurrentFile("file-in-archive");
     * QuaZipFile file(&zip);
     * file.open(QIODevice::ReadOnly);
     * // ok, now we can read from the file
     * file.read(somewhere, some);
     * zip.setCurrentFile("another-file-in-archive"); // oops...
     * QuaZipFile anotherFile(&zip);
     * anotherFile.open(QIODevice::ReadOnly);
     * anotherFile.read(somewhere, some); // this is still ok...
     * file.read(somewhere, some); // and this is NOT
     * \endcode
     * So, what exactly happens here? When we change current file in the
     * \c zip archive, \c file that references it becomes invalid
     * (actually, as far as I understand ZIP/UNZIP sources, it becomes
     * closed, but QuaZipFile has no means to detect it).
     *
     * Summary: do not close \c zip object or change its current file as
     * long as QuaZipFile is open. Even better - use another constructors
     * which create internal QuaZip instances, one per object, and
     * therefore do not cause unnecessary trouble. This constructor may
     * be useful, though, if you already have a QuaZip instance and do
     * not want to access several files at once. Good example:
     * \code
     * QuaZip zip("archive.zip");
     * zip.open(QuaZip::mdUnzip);
     * // first, we need some information about archive itself
     * QByteArray comment=zip.getComment();
     * // and now we are going to access files inside it
     * QuaZipFile file(&zip);
     * for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
     *   file.open(QIODevice::ReadOnly);
     *   // do something cool with file here
     *   file.close(); // do not forget to close!
     * }
     * zip.close();
     * \endcode
     **/
    QuaZipFile(QuaZip *zip, QObject *parent =NULL);
    /// Destroys a QuaZipFile instance.
    /** Closes file if open, destructs internal QuaZip object (if it
     * exists and \em is internal, of course).
     **/
    virtual ~QuaZipFile();
    /// Returns the ZIP archive file name.
    /** If this object was created by passing QuaZip pointer to the
     * constructor, this function will return that QuaZip's file name
     * (or null string if that object does not have file name yet).
     *
     * Otherwise, returns associated ZIP archive file name or null
     * string if there are no name set yet.
     *
     * \sa setZipName() getFileName()
     **/
    QString getZipName()const;
    /// Returns a pointer to the associated QuaZip object.
    /** Returns \c NULL if there is no associated QuaZip or it is
     * internal (so you will not mess with it).
     **/
    QuaZip* getZip()const;
    /// Returns file name.
    /** This function returns file name you passed to this object either
     * by using
     * QuaZipFile(const QString&,const QString&,QuaZip::CaseSensitivity,QObject*)
     * or by calling setFileName(). Real name of the file may differ in
     * case if you used case-insensitivity.
     *
     * Returns null string if there is no file name set yet. This is the
     * case when this QuaZipFile operates on the existing QuaZip object
     * (constructor QuaZipFile(QuaZip*,QObject*) or setZip() was used).
     * 
     * \sa getActualFileName
     **/
    QString getFileName()const {return fileName;}
    /// Returns case sensitivity of the file name.
    /** This function returns case sensitivity argument you passed to
     * this object either by using
     * QuaZipFile(const QString&,const QString&,QuaZip::CaseSensitivity,QObject*)
     * or by calling setFileName().
     *
     * Returns unpredictable value if getFileName() returns null string
     * (this is the case when you did not used setFileName() or
     * constructor above).
     *
     * \sa getFileName
     **/
    QuaZip::CaseSensitivity getCaseSensitivity()const {return caseSensitivity;}
    /// Returns the actual file name in the archive.
    /** This is \em not a ZIP archive file name, but a name of file inside
     * archive. It is not necessary the same name that you have passed
     * to the
     * QuaZipFile(const QString&,const QString&,QuaZip::CaseSensitivity,QObject*),
     * setFileName() or QuaZip::setCurrentFile() - this is the real file
     * name inside archive, so it may differ in case if the file name
     * search was case-insensitive.
     *
     * Equivalent to calling getCurrentFileName() on the associated
     * QuaZip object. Returns null string if there is no associated
     * QuaZip object or if it does not have a current file yet. And this
     * is the case if you called setFileName() but did not open the
     * file yet. So this is perfectly fine:
     * \code
     * QuaZipFile file("somezip.zip");
     * file.setFileName("somefile");
     * QString name=file.getName(); // name=="somefile"
     * QString actual=file.getActualFileName(); // actual is null string
     * file.open(QIODevice::ReadOnly);
     * QString actual=file.getActualFileName(); // actual can be "SoMeFiLe" on Windows
     * \endcode
     *
     * \sa getZipName(), getFileName(), QuaZip::CaseSensitivity
     **/
    QString getActualFileName()const;
    /// Sets the ZIP archive file name.
    /** Automatically creates internal QuaZip object and destroys
     * previously created internal QuaZip object, if any.
     *
     * Will do nothing if this file is already open. You must close() it
     * first.
     **/
    void setZipName(const QString& zipName);
    /// Returns \c true if the file was opened in raw mode.
    /** If the file is not open, the returned value is undefined.
     *
     * \sa open(OpenMode,int*,int*,bool,const char*)
     **/
    bool isRaw()const {return raw;}
    /// Binds to the existing QuaZip instance.
    /** This function destroys internal QuaZip object, if any, and makes
     * this QuaZipFile to use current file in the \a zip object for any
     * further operations. See QuaZipFile(QuaZip*,QObject*) for the
     * possible pitfalls.
     *
     * Will do nothing if the file is currently open. You must close()
     * it first.
     **/
    void setZip(QuaZip *zip);
    /// Sets the file name.
    /** Will do nothing if at least one of the following conditions is
     * met:
     * - ZIP name has not been set yet (getZipName() returns null
     *   string).
     * - This QuaZipFile is associated with external QuaZip. In this
     *   case you should call that QuaZip's setCurrentFile() function
     *   instead!
     * - File is already open so setting the name is meaningless.
     *
     * \sa QuaZip::setCurrentFile
     **/
    void setFileName(const QString& fileName, QuaZip::CaseSensitivity cs =QuaZip::csDefault);
    /// Opens a file.
    /** Returns \c true on success, \c false otherwise.
     * Call getZipError() to get error code.
     *
     * \note Since ZIP/UNZIP API provides buffered reading only,
     * QuaZipFile does not support unbuffered reading. So do not pass
     * QIODevice::Unbuffered flag in \a mode, or open will fail.
     **/
    virtual bool open(OpenMode mode);
    /// Opens a file.
    /** \overload
     * Argument \a password specifies a password to decrypt the file. If
     * it is NULL then this function behaves just like open(OpenMode).
     **/
    bool open(OpenMode mode, const char *password)
    {return open(mode, NULL, NULL, false, password);}
    /// Opens a file.
    /** \overload
     * Argument \a password specifies a password to decrypt the file.
     *
     * An integers pointed by \a method and \a level will receive codes
     * of the compression method and level used. See unzip.h.
     *
     * If raw is \c true then no decompression is performed.
     *
     * \a method should not be \c NULL. \a level can be \c NULL if you
     * don't want to know the compression level.
     **/
    bool open(OpenMode mode, int *method, int *level, bool raw, const char *password =NULL);
    /// Returns \c true, but \ref quazipfile-sequential "beware"!
    virtual bool isSequential()const;
    /// Returns current position in the file.
    /** Implementation of the QIODevice::pos(). This function is a
     * wrapper to the ZIP/UNZIP unztell(), therefore it is unable to
     * keep track of the ungetChar() calls (which is non-virtual and
     * therefore is dangerous to reimplement). So if you are using
     * ungetChar() feature of the QIODevice, this function reports
     * incorrect value until you get back characters which you ungot.
     *
     * \note Although
     * \ref quazipfile-sequential "QuaZipFile is a sequential device"
     * and therefore pos() should always return zero, it does not,
     * because it would be misguiding. Keep this in mind.
     *
     * This function returns -1 if the file or archive is not open.
     *
     * Error code returned by getZipError() is not affected by this
     * function call.
     **/
    virtual qint64 pos()const;
    /// Returns \c true if the end of file was reached.
    /** This function returns \c false in the case of error. This means
     * that you called this function on either not open file, or a file
     * in the not open archive or even on a QuaZipFile instance that
     * does not even have QuaZip instance associated. Do not do that
     * because there is no means to determine whether \c false is
     * returned because of error or because end of file was reached.
     * Well, on the other side you may interpret \c false return value
     * as "there is no file open to check for end of file and there is
     * no end of file therefore".
     *
     * Error code returned by getZipError() is not affected by this
     * function call.
     **/
    virtual bool atEnd()const;
    /// Returns file size.
    /** This function returns csize() if the file is open in raw mode
     * and usize() if it is open in normal mode. If the file is not open
     * at all, the behaviour is undefined. Do not do that.
     *
     * Returns -1 on error, call getZipError() to get error code.
     *
     * \note This function returns file size despite that
     * \ref quazipfile-sequential "QuaZipFile is considered to be sequential device",
     * for which size() should return bytesAvailable() instead. But its
     * name would be very misguiding otherwise, so just keep in mind
     * this inconsistence.
     **/
    virtual qint64 size()const;
    /// Returns compressed file size.
    /** Equivalent to calling getFileInfo() and then getting
     * compressedSize field, but more convenient and faster. See
     * getFileInfo() for a warning.
     *
     * Returns -1 on error, call getZipError() to get error code.
     **/
    virtual qint64 csize()const;
    /// Returns uncompressed file size.
    /** Equivalent to calling getFileInfo() and then getting
     * uncompressedSize field, but more convenient and faster. See
     * getFileInfo() for a warning.
     *
     * Returns -1 on error, call getZipError() to get error code.
     **/
    virtual qint64 usize()const;
    /// Gets information about current file.
    /** This function does the same thing as calling
     * QuaZip::getCurrentFileInfo() on the associated QuaZip object
     * except that you can not call getCurrentFileInfo() if the
     * associated QuaZip is internal (because you do not have access to
     * it), but you still can call this function in that case.
     *
     * Returns \c false in the case of an error. This includes the case
     * when QuaZip is internal and you did not open this QuaZipFile yet.
     * To avoid potential bugs, it is better to not use this function at
     * all if the file is not open.
     **/
    bool getFileInfo(QuaZipFileInfo *info);
    /// Closes the file.
    /** Clears the error on success. Otherwise, getError() call will
     * return either QuaZipFile::errWrongMode (if attempting to close
     * not open file or a file in not open ZIP archive) or
     * QuaZipFile::errZip (if ZIP/UNZIP API call returned).
     **/
    virtual void close();
    /// Returns the error code returned by the last ZIP/UNZIP API call.
    int getZipError()const {return zipError;}
};

#endif
