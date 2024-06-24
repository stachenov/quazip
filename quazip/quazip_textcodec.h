#ifndef QUAZIPTEXTCODEC_H
#define QUAZIPTEXTCODEC_H

#include <QByteArray>
#include "quazip_global.h"

#ifdef QUAZIP_CAN_USE_QTEXTCODEC
   #include <QTextCodec>

#else
    #include <QStringConverter>
#endif


#ifdef QUAZIP_CAN_USE_QTEXTCODEC
class QUAZIP_EXPORT QuazipTextCodec: public QTextCodec
#else
class QUAZIP_EXPORT QuazipTextCodec
#endif
{
    
public:
    explicit QuazipTextCodec();

    QByteArray fromUnicode(const QString &str) const;
    QString toUnicode(const QByteArray &a) const;

    static QuazipTextCodec *codecForName(const QByteArray &name);
    static QuazipTextCodec *codecForLocale();
protected:

#ifndef QUAZIP_CAN_USE_QTEXTCODEC
    static void setup();
    QStringConverter::Encoding  mEncoding;
#endif
};

#endif // QUAZIPTEXTCODEC_H
