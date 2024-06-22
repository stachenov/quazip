#include "quazip_textcodec.h"
#include <QCoreApplication>
#include <QDebug>

static QList<QuazipTextCodec*> *static_list_quazip_codecs  = nullptr;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))


static QHash<QStringConverter::Encoding,QuazipTextCodec*> *static_hash_quazip_codecs  = nullptr;
#else
static QHash<QTextCodec*,QuazipTextCodec*> *static_hash_quazip_codecs  = nullptr;

#endif



class QuazipTextTextCodecCleanup
{

public:
    explicit QuazipTextTextCodecCleanup()
    {
    }

    ~QuazipTextTextCodecCleanup()
    {
        if (static_list_quazip_codecs)
        {

            //qWarning()<<"~QuazipTextTextCodecCleanup()";
            qDeleteAll(static_list_quazip_codecs->begin(),static_list_quazip_codecs->end());
            static_list_quazip_codecs->clear();
            delete static_list_quazip_codecs;
            static_list_quazip_codecs = nullptr;
        }
    }
};

//////
Q_GLOBAL_STATIC(QuazipTextTextCodecCleanup, createQuazipTextTextCodecCleanup)



QuazipTextCodec::QuazipTextCodec()
{
    QuazipTextCodec::setup();
}


void QuazipTextCodec::setup()
{
    if (static_list_quazip_codecs) return;
      (void)createQuazipTextTextCodecCleanup();

   // qWarning()<<"QuazipTextCodec::setup()";


    static_list_quazip_codecs = new QList<QuazipTextCodec*>;

    #if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    static_hash_quazip_codecs = new QHash<QStringConverter::Encoding,QuazipTextCodec*>;


    #else

    static_hash_quazip_codecs = new QHash<QTextCodec*,QuazipTextCodec*>;

    #endif
}




QuazipTextCodec *QuazipTextCodec::codecForName(const QByteArray &name)
{
    QuazipTextCodec::setup();

    ///
    #if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
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

        static_list_quazip_codecs->append(codec);
        return codec;

    #else

        QTextCodec *qt_text_codec = QTextCodec::codecForName(name);

        if (qt_text_codec==nullptr)
        {
            qt_text_codec = QTextCodec::codecForLocale();
        }

        if (static_hash_quazip_codecs->contains(qt_text_codec))
        {
            return static_hash_quazip_codecs->value(qt_text_codec);
        }
        QuazipTextCodec *codec = new QuazipTextCodec();
        codec->mTextCodec = qt_text_codec;
        static_hash_quazip_codecs->insert(qt_text_codec,codec);

        static_list_quazip_codecs->append(codec);
        return codec;


    #endif

}




QuazipTextCodec *QuazipTextCodec::codecForLocale()
{
    QuazipTextCodec::setup();
    return QuazipTextCodec::codecForName("System");
}

QByteArray QuazipTextCodec::fromUnicode(const QString &str) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    auto from = QStringEncoder(mEncoding);
    return from(str);
#else
    if (mTextCodec)
    {
        return mTextCodec->fromUnicode(str);
    }
#endif
    return QByteArray();
}


QString QuazipTextCodec::toUnicode(const QByteArray &a) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    auto to = QStringDecoder(mEncoding);
    return to(a);
#else
    if (mTextCodec)
    {
        return mTextCodec->toUnicode(a);
    }
#endif
    return QString();
}

