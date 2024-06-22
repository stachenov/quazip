#ifndef QUAZIPTEXTCODEC_H
#define QUAZIPTEXTCODEC_H

#include <QByteArray>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <QStringConverter>
#else
    #include <QTextCodec>
#endif


#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
class QuazipTextCodec
#else
class QuazipTextCodec : public QTextCodec
#endif
{
    
public:
    explicit QuazipTextCodec();

    QByteArray fromUnicode(const QString &str) const;
    QString toUnicode(const QByteArray &a) const;

    static QuazipTextCodec *codecForName(const QByteArray &name);
    static QuazipTextCodec *codecForLocale();
protected:

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    static void setup();
    QStringConverter::Encoding  mEncoding;
#endif
};

#endif // QUAZIPTEXTCODEC_H


};

#endif // QUAZIPTEXTCODEC_H

