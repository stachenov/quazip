#include "quazip_textcodec.h"
#include <QCoreApplication>
#include <QDebug>


#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

static QHash<QStringConverter::Encoding,QuazipTextCodec*> *static_hash_quazip_codecs  = nullptr;

class QuazipTextTextCodecCleanup
{
public:
    explicit QuazipTextTextCodecCleanup()
    {
    }
    ~QuazipTextTextCodecCleanup()
    {
        if (static_hash_quazip_codecs)
        {
            QList<QuazipTextCodec*>list_quazip_codecs = static_hash_quazip_codecs->values();
            qDeleteAll(list_quazip_codecs.begin(),list_quazip_codecs.end());
            static_hash_quazip_codecs->clear();
            delete static_hash_quazip_codecs;
            static_hash_quazip_codecs = nullptr;
        }
    }
};

Q_GLOBAL_STATIC(QuazipTextTextCodecCleanup, createQuazipTextTextCodecCleanup)

#endif


QuazipTextCodec::QuazipTextCodec()
{
}

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    void QuazipTextCodec::setup()
    {
        if (static_hash_quazip_codecs) return;
          (void)createQuazipTextTextCodecCleanup();

        static_hash_quazip_codecs = new QHash<QStringConverter::Encoding,QuazipTextCodec*>;
    }

#endif


QuazipTextCodec *QuazipTextCodec::codecForName(const QByteArray &name)
{
    #if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        QuazipTextCodec::setup();
        QStringConverter::Encoding  encoding = QStringConverter::Utf8;

        std::optional<QStringConverter::Encoding> opt_encoding = QStringConverter::encodingForName(name);
        if (opt_encoding != std::nullopt)
        {
            encoding = opt_encoding.value();
        }
        if (static_hash_quazip_codecs->contains(encoding))
        {
            return static_hash_quazip_codecs->value(encoding);
        }

        QuazipTextCodec *codec = new QuazipTextCodec();
        /////
        codec->mEncoding = encoding;
        static_hash_quazip_codecs->insert(encoding,codec);
        return codec;

    #else

    return (QuazipTextCodec*) QTextCodec::codecForName(name);

    #endif

}


QuazipTextCodec *QuazipTextCodec::codecForLocale()
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QuazipTextCodec::setup();
    return QuazipTextCodec::codecForName("System");
#else

    return (QuazipTextCodec*) QTextCodec::codecForLocale();
#endif
}

QByteArray QuazipTextCodec::fromUnicode(const QString &str) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    auto from = QStringEncoder(mEncoding);
    return from(str);
#else

    return QTextCodec::fromUnicode(str);

#endif
}


QString QuazipTextCodec::toUnicode(const QByteArray &a) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    auto to = QStringDecoder(mEncoding);
    return to(a);
#else

        return QTextCodec::toUnicode(a);

#endif
}
