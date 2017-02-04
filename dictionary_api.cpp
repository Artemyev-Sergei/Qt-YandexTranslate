#include "dictionary_api.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QUrl>

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QDebug>
#include <QMessageBox>

Example::Example()
{}

Example::~Example()
{}

Translation::Translation()
{}

Translation::~Translation()
{}

DictionaryEntry::DictionaryEntry()
{}

DictionaryEntry::~DictionaryEntry()
{}

YandexDictionary::YandexDictionary()
{}

YandexDictionary::~YandexDictionary()
{}

void Example::showExample()
{
    qDebug() << this->text; // Text of an example.

    QStringList::const_iterator iter;
    for(iter = this->translations.constBegin(); iter != this->translations.constEnd(); ++iter)
    {
        qDebug() << *iter;  // Translations of the text of an example.
    }
}

void Translation::showTranslation()
{
    QString header = (this->text + " "
                      + this->pos + " "
                      + this->gen + " "
                      + this->asp + " "
                      + this->num); // Header of a translation.
    qDebug() << header.trimmed();   // Get rid of spaces in the beginning and in the end of a string.

    if(!this->synonyms.isEmpty())
    {
        QStringList::const_iterator iter;
        qDebug() << "Synonyms:";
        for(iter = this->synonyms.constBegin(); iter != this->synonyms.constEnd(); ++iter)
        {
            qDebug() << *iter;
        }
    }

    if(!this->meanings.isEmpty())
    {
        QStringList::const_iterator iter;
        qDebug() << "Meanings:";
        for(iter = this->meanings.constBegin(); iter != this->meanings.constEnd(); ++iter)
        {
            qDebug() << *iter;
        }
    }

    int i = 1;  // An example number.
    QVector<Example>::iterator it;
    for(it = this->examples.begin(); it != this->examples.end(); ++it, ++i)
    {
        qDebug() << "Example" << i;
        it->showExample();
    }
}

void DictionaryEntry::showEntry()
{
    QString header = (this->text + " [" + this->ts + "] " + this->pos); // Header of an entry.
    qDebug() << header.trimmed();   // Get rid of spaces in the beginning and in the end of a string.

    int i = 1;  // A translation number.
    QVector<Translation>::iterator iter;
    for(iter = this->translations.begin(); iter != this->translations.end(); ++iter, ++i)
    {
        qDebug() << "Translation" << i;
        iter->showTranslation();
    }
}

void YandexDictionary::getLanguages(QMap<QString, QString> languages_)
{
    this->languages = languages_;
}

void YandexDictionary::getTranslationDirections()
{
    // Create a request.
    QUrl url(QString("https://dictionary.yandex.net/api/v1/dicservice.json/getLangs?")
             + QString("key=") + dict_api_key
             );

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    // Send the request.
    QEventLoop loop;
    QNetworkAccessManager manager;
    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    // Receive a reply.
    QNetworkReply* reply = manager.get(request);
    loop.exec();

    // Parse the reply.
    QByteArray bytes = reply->readAll();

    /* Another approach to parsing the reply:
     * you can check a type of the reply data
     * in order to choose an appropriate strategy for the parsing process. */
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (doc.isObject())
    {
        QJsonObject obj = doc.object();
        // Check the response codes.
        if(obj.contains("code") && obj.contains("message"))
        {
            QMessageBox messageBox;
            messageBox.critical(0,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
            messageBox.setFixedSize(500,200);
            // TODO: Error handling.
        }
    }
    else if(doc.isArray())
    {
        QJsonArray dirs = doc.array();
        for(QJsonValue v : dirs)
        {
            directions << v.toString();
        }
    }
}

QVector<DictionaryEntry> YandexDictionary::getDictionaryEntry(QString& source_lang, QString& target_lang, QString& text)
{
    QString dict_dir = languages.key(source_lang) + "-" + languages.key(target_lang);

    /* The following commented condition check is not necessary because
     * Dictionary API returns ERR_LANG_NOT_SUPPORTED code
     * if the specified translation direction is not supported.
     * But, anyway, inasmuch as we have our translation directions,
     * we could use it if we wanted to.
     * if (!directions.contains(dict_dir)) ... */

    // Create a request.
    QUrl url(QString("https://dictionary.yandex.net/api/v1/dicservice.json/lookup?")
             + QString("key=") + dict_api_key
             + QString("&lang=") + dict_dir
             + QString("&text=") + text
             //+ QString("&ui=") + <the language of the user's interface for displaying the dictionary entry>
             //+ QString("&flags=") + <search options (bitmask of flags)>
             );

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    // Send the request.
    QEventLoop loop;
    QNetworkAccessManager manager;
    connect(&manager, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);

    // Receive a reply.
    QNetworkReply* reply = manager.get(request);
    loop.exec();

    // Parse the reply.
    QByteArray bytes = reply->readAll();

    QJsonDocument doc = QJsonDocument::fromJson(bytes);\
    QJsonObject obj = doc.object(); // Keys are ("def", "head")

    // Check the response codes.
    if(obj.contains("code") && obj.contains("message"))
    {
        /* By the way, it seems like Dictionary API never returns
         * ERR_OK code, i.e 200. This is strange because
         * the code is yet mentioned in the documentation. */
        QMessageBox messageBox;
        messageBox.critical(0,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        messageBox.setFixedSize(500,200);
        // TODO: Error handling.
    }

    //QJsonObject head = obj.value("head").toObject();    // Result header (not used).

    QVector<DictionaryEntry> entries;   // We will fill this QVector with data, i.e. dictionary entries.
    QJsonArray def = obj.value("def").toArray();    // Dictionary entries.
    for (QJsonValue v : def)
    {
        DictionaryEntry entry;
        QJsonObject o = v.toObject();
        //qDebug() << o.keys();
        if(o.contains("text"))  // Text of an entry.
        {
            entry.setText(o.value("text").toString());
        }
        if(o.contains("ts"))    // Transcription.
        {
            entry.setTranscription(o.value("ts").toString());
        }
        if(o.contains("pos"))   // Part of speech.
        {
            entry.setPartOfSpeech(o.value("pos").toString());
        }
        if(o.contains("tr"))    // Translations.
        {
            QJsonArray tr = o.value("tr").toArray();
            for (QJsonValue val : tr)
            {
                Translation translation;
                QJsonObject obj = val.toObject();
                //qDebug() << obj.keys();
                if(obj.contains("text"))// Text of a translation.
                {
                    translation.setText(obj.value("text").toString());
                }
                if(obj.contains("pos")) // Part of speech.
                {
                    translation.setPartOfSpeech(obj.value("pos").toString());
                }
                if(obj.contains("asp")) // The aspect of a verb.
                {
                    translation.setAspect(obj.value("asp").toString());
                }
                if(obj.contains("num")) // The form of a noun and its variations: plural, etc.
                {
                    translation.setNounForm(obj.value("num").toString());
                }
                if(obj.contains("gen")) // Gender.
                {
                    translation.setGender(obj.value("gen").toString());
                }
                if(obj.contains("syn")) // Synonyms.
                {
                    QJsonArray syn = obj.value("syn").toArray();
                    for(QJsonValue value : syn)
                    {
                        translation.addSynonym(value.toObject().value("text").toString());
                    }
                }
                if(obj.contains("mean"))// Meanings.
                {
                    QJsonArray mean = obj.value("mean").toArray();
                    for(QJsonValue value : mean)
                    {
                        translation.addMeaning(value.toObject().value("text").toString());
                    }
                }
                if(obj.contains("ex"))  // Usage examples.
                {
                    QJsonArray ex = obj.value("ex").toArray();
                    for(QJsonValue value : ex)
                    {
                        Example example;
                        QJsonObject object = value.toObject();
                        //qDebug() << object.keys();
                        if(object.contains("text")) // Text of an example.
                        {
                            example.setText(object.value("text").toString());
                        }
                        if(object.contains("tr"))   // Translations.
                        {
                            QJsonArray transl = object.value("tr").toArray();
                            for (QJsonValue tr_value : transl)
                            {
                                if(object.contains("text"))
                                {
                                    example.addTranslation(tr_value.toObject().value("text").toString());
                                }
                            }
                        }
                        translation.addExample(example);    // Add another example.
                    }
                }
                entry.addTranslation(translation);  // Add another translation.
            }
        }
        entries.push_back(entry);   // Add another entry.
    }

    // We have collected all of our entries. Let's return al of them.
    return entries;
}
