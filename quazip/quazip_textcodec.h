#ifndef QUAZIPTEXTCODEC_H
#define QUAZIPTEXTCODEC_H

#include <QByteArray>



#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QStringConverter>
#else
    #include <QTextCodec>
#endif



class QuazipTextCodec 
{
    
public:
    explicit QuazipTextCodec();

    QByteArray fromUnicode(const QString &str) const;
    QString toUnicode(const QByteArray &a) const;

    static QuazipTextCodec *codecForName(const QByteArray &name);
    static QuazipTextCodec *codecForLocale();
protected:

    static void setup();

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QStringConverter::Encoding  mEncoding;
#else
    QTextCodec *mTextCodec = nullptr;
#endif





};

#endif // QUAZIPTEXTCODEC_H
