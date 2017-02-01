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

#include <QMessageBox>

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
    if(obj.contains("code") || obj.contains("message"))
    {
        QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        return;
    }

    QJsonObject langs = obj.value("langs").toObject();  // Codes and Languages.

    /* Use the following commented line to check the type of an object.
     * There're also such functions as isArray(), isDouble(), etc.
     * See Qt Documentation for more examples.
     */
    //qDebug() << obj.value("langs").isObject();

    for(const QString& code : langs.keys())
    {
        QString lang = langs.value(code).toString();
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
    switch(int(obj.value("code").toDouble()))
    {
    case 200: break;
    case 401:
    case 402:
    case 404:
        QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        return;
    default:
        QMessageBox::information(this,"Unknown","Something unexpected happened");
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
    switch(int(obj.value("code").toDouble()))
    {
    case 200: break;
    case 401:
    case 402:
    case 404:
    case 413:
    case 422:
    case 501:
        QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        return;
    default:
        QMessageBox::information(this,"Unknown","Something unexpected happened");
        return;
    }

    //QString dir = obj.value("lang").toString();   // Use the auto-defined translation direction if you need it.

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
     * in order to choose an appropriate strategy for the parsing process.
     */
    QJsonDocument doc = QJsonDocument::fromJson(bytes);
    if (doc.isObject())
    {
        QJsonObject obj = doc.object();
        // Check the response codes.
        if(obj.contains("code") || obj.contains("message"))
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

    /* The following condition is not necessary because
     * Dictionary API returns ERR_LANG_NOT_SUPPORTED code
     * if the specified translation direction is not supported.
     * But, anyway, I want to leave it here for educational purposes.
     */
    if(!directions.contains(dict_dir))
    {
        QMessageBox::information(this,"501","The specified translation direction is not supported.");
        return;
    }
    else    // else is unnecessary here but I leave it be.
    {
        // Create a request.
        QUrl url(QString("https://dictionary.yandex.net/api/v1/dicservice.json/lookup?")
                 + QString("key=") + dict_api_key
                 + QString("&lang=")
                 + dict_dir
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
        if(obj.contains("code") || obj.contains("message"))
        {
            /* In fact, it seems like Dictionary API never returns
             * ERR_OK code, i.e 200. This is strange because
             * the code is yet mentioned in the documentation.
             */
            QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
            return;
        }

        //QJsonObject head = obj.value("head").toObject();    // Result header (not used).

        QJsonArray def = obj.value("def").toArray();
        for (QJsonValue v : def)
        {
            QJsonObject o = v.toObject();
            qDebug() << "First level def: " << o.keys();
            if(o.contains("tr"))
            {
                qDebug() << "tr";
                QJsonArray tr = o.value("tr").toArray();
                for (QJsonValue val : tr)
                {
                    QJsonObject obj = val.toObject();
                    qDebug() << "Second level tr: " << obj.keys();
                    if(obj.contains("text"))
                    {
                        qDebug() << "text";
                        qDebug() << obj.value("text").toString();
                    }
                    if(obj.contains("pos"))
                    {
                        qDebug() << "pos";
                        qDebug() << obj.value("pos").toString();
                    }
                    if(obj.contains("mean"))
                    {
                        qDebug() << "mean";
                        QJsonArray mean = obj.value("mean").toArray();
                        for(QJsonValue value : mean)
                        {
                            qDebug() << value.toObject().value("text").toString();
                        }
                    }
                    if(obj.contains("gen"))
                    {
                        qDebug() << "gen";
                        qDebug() << obj.value("gen").toString();
                    }
                    if(obj.contains("syn"))
                    {
                        qDebug() << "syn";
                        QJsonArray syn = obj.value("syn").toArray();
                        for(QJsonValue value : syn)
                        {
                            qDebug() << value.toObject().value("text").toString();
                        }
                    }
                    if(obj.contains("ex"))
                    {
                        qDebug() << "ex";
                        QJsonArray ex = obj.value("ex").toArray();
                        for(QJsonValue value : ex)
                        {
                            QJsonObject object = value.toObject();
                            qDebug() << object.keys();
                            qDebug() << "Third level ex: ";
                        }
                    }
                    if(obj.contains("asp"))
                    {
                        qDebug() << "asp";
                        qDebug() << obj.value("asp").toString();    //
                    }
                }
            }
            if(o.contains("text"))
            {
                qDebug() << "text";
                qDebug() << o.value("text").toString();

            }
            if(o.contains("pos"))
            {
                qDebug() << "pos";
                qDebug() << o.value("pos").toString();
            }
            if(o.contains("ts"))
            {
                qDebug() << "ts";
                qDebug() << o.value("ts").toString();
            }
            //qDebug() << v.isArray() << v.isBool() << v.isDouble() << v.isNull() << v.isObject() << v.isString() << v.isUndefined();
            //qDebug() << v.toObject().contains("tr") << v.toObject().contains("text") << v.toObject().contains("pos");
            //qDebug() << v.toObject().contains("ex") << v.toObject().contains("syn")  << v.toObject().contains("mean");
        }

        //qDebug() << def.size();

        /*
        // Show the dictionary entry.
        QJsonArray text = obj.value("text").toArray();
        ui->TargetEdit->clear();
        for(QJsonValue v : text)
        {
            ui->TargetEdit->append(v.toString());
        }
        */
    }
}
