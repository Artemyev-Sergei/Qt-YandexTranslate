#include "translate_api.h"

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

YandexTranslate::YandexTranslate()
{}

YandexTranslate::~YandexTranslate()
{}

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
        QMessageBox messageBox;
        messageBox.critical(0,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        messageBox.setFixedSize(500,200);
        // TODO: Error handling.
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
    }

    emit languagesReceived(languages);  // If you want to use Dictionary API as well, you should transfer languages and their codes to the corresponding object.
}

QString YandexTranslate::detectLanguage(QString &source_text)
{
    // Create a request.
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
        QMessageBox messageBox;
        messageBox.critical(0,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        messageBox.setFixedSize(500,200);
        // TODO: Error handling.
    }

    // Return a detected language.
    QString lang_code = obj.value("lang").toString();
    return languages[lang_code];    // Return the full name of a detected language.
}

QStringList YandexTranslate::translate(QString& source_lang, QString& target_lang, QString& text_to_translate)
{
    // Create a request.
    QUrl url(QString("https://translate.yandex.net/api/v1.5/tr.json/translate?")
             + QString("key=") + trnsl_api_key
             + QString("&lang=")
             + languages.key(source_lang) + "-" + languages.key(target_lang)
             + QString("&text=") + text_to_translate
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
        QMessageBox messageBox;
        messageBox.critical(0,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        messageBox.setFixedSize(500,200);
        // TODO: Error handling.
    }

    //QString dir = obj.value("lang").toString();   // Use the auto-defined translation direction from the server.

    // Return the translated text.
    QJsonArray text = obj.value("text").toArray();
    QStringList result; // The result of translation, i.e. text.
    for(QJsonValue v : text)
    {
        result << v.toString();
    }
    return result;
}
