#include "yandextranslate.h"
#include "ui_yandextranslate.h"

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

#include <QVector>
#include <QMessageBox>
#include "dictionaryentry.h"

YandexTranslate::YandexTranslate(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::YandexTranslate)
{
    // Initial setup.
    ui->setupUi(this);
    this->setFixedSize(this->geometry().width(),this->geometry().height());
    this->getSupportedLanguages();
    this->getTranslationDirections();

    ui->comboBoxSource->setCurrentText("English");
    ui->comboBoxTarget->setCurrentText("Russian");

    connect(ui->detectButton,SIGNAL(clicked(bool)),this,SLOT(detectLanguage()));
    connect(ui->translateButton,SIGNAL(clicked(bool)),this,SLOT(translate()));
    connect(ui->dictionaryButton,SIGNAL(clicked(bool)),this,SLOT(getDictionaryEntry()));
}

YandexTranslate::~YandexTranslate()
{
    delete ui;
}

void YandexTranslate::getSupportedLanguages()
{
    // Create a request.
    QUrl url(QString("https://translate.yandex.net/api/v1.5/tr.json/getLangs?")
             + QString("key=") + trnsl_api_key
             + QString("&ui=") + QString("en")
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
    QJsonObject obj = QJsonDocument::fromJson(bytes).object();  // Keys are ("dirs", "langs")
    // qDegub() << obj.keys();  // Use it to check the keys.

    // Check the response codes.
    if(obj.contains("code") && obj.contains("message"))
    {
        QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        return;
    }

    /* Use the following commented line to check the type of an object
     * when you are not sure.
     * There're also such functions as isArray(), isDouble(), etc.
     * See Qt Documentation for more examples. */
    //qDebug() << obj.value("langs").isObject();

    QJsonObject langs = obj.value("langs").toObject();
    for(const QString& code : langs.keys())
    {
        QString lang = langs.value(code).toString();    // "lang" is a full name of a language.
        languages[code] = lang;
        // Fill comboBoxes with initial data.
        ui->comboBoxSource->addItem(lang);
        ui->comboBoxTarget->addItem(lang);
    }
}

void YandexTranslate::detectLanguage()
{
    // Create a request.
    QString source_text = ui->SourceEdit->toPlainText();
    QUrl url(QString("https://translate.yandex.net/api/v1.5/tr.json/detect?")
             + QString("key=") + trnsl_api_key
             + QString("&text=") + source_text
             //+ QString("&hint=") + <list of probable text languages>
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
    QJsonObject obj = QJsonDocument::fromJson(bytes).object();  // Keys are ("code", "lang")

    // Check the response codes.
    if(obj.contains("code") && obj.contains("message"))
    {
        QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        return;
    }

    // Set a detected language.
    QString lang_code = obj.value("lang").toString();
    ui->comboBoxSource->setCurrentText(languages[lang_code]);
}

void YandexTranslate::translate()
{
    QString source_lang = ui->comboBoxSource->currentText();
    QString target_lang = ui->comboBoxTarget->currentText();

    // Create a request.
    QUrl url(QString("https://translate.yandex.net/api/v1.5/tr.json/translate?")
             + QString("key=") + trnsl_api_key
             + QString("&lang=")
             + languages.key(source_lang) + "-" + languages.key(target_lang)
             + QString("&text=") + ui->SourceEdit->toPlainText()
             //+ QString("&format=") + <text format>
             //+ QString("&options=") + <translation options>
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
    QJsonObject obj = QJsonDocument::fromJson(bytes).object();  // Keys are ("code", "lang", "text")

    // Check the response codes.
    if(obj.contains("code") && obj.contains("message"))
    {
        QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        return;
    }

    //QString dir = obj.value("lang").toString();   // Use the auto-defined translation direction from the server.

    // Show the translated text.
    QJsonArray text = obj.value("text").toArray();
    ui->TargetEdit->clear();
    for(QJsonValue v : text)
    {
        ui->TargetEdit->append(v.toString());
    }
}

void YandexTranslate::getTranslationDirections()
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

    /* A different approach to parsing the reply:
     * you can check a type of the reply data
     * in order to choose an appropriate strategy for the parsing process. */
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (doc.isObject())
    {
        QJsonObject obj = doc.object();
        // Check the response codes.
        if(obj.contains("code") && obj.contains("message"))
        {
            QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
            return;
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

void YandexTranslate::getDictionaryEntry()
{
    QString source_lang = ui->comboBoxSource->currentText();
    QString target_lang = ui->comboBoxTarget->currentText();

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
             + QString("&text=") + ui->SourceEdit->toPlainText()
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
        QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        return;
    }

    //QJsonObject head = obj.value("head").toObject();    // Result header (not used).

    QVector<DictionaryEntry> entries;   // We will fill this QVector with data, i.e dictionary entries.
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

    // We have collected all of our entries. Let's show them.
    QVector<DictionaryEntry>::iterator iter;
    for(iter = entries.begin(); iter != entries.end(); ++iter)
    {
        iter->showEntry();  // Look into dictionaryentry.cpp for the current implementation of showEntry().
    }
}
