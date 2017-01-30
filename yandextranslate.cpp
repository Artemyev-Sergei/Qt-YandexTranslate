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

    ui->comboBoxSource->setCurrentText("English");
    ui->comboBoxTarget->setCurrentText("Russian");

    connect(ui->detectButton,SIGNAL(clicked(bool)),this,SLOT(detectLanguage()));
    connect(ui->translateButton,SIGNAL(clicked(bool)),this,SLOT(translate()));
}

YandexTranslate::~YandexTranslate()
{
    delete ui;
}

void YandexTranslate::getSupportedLanguages()
{
    // Create a request.
    QUrl url(QString("https://translate.yandex.net/api/v1.5/tr.json/getLangs?")
             + QString("key=") + api_key
             + QString("&ui=") + QString("en")
             //+ QString("$callback=") + <name of the callback function>
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

    // Check the response codes.
    if(obj.contains("code") || obj.contains("message"))
    {
        QMessageBox::information(this,QString::number(obj.value("code").toDouble()),obj.value("message").toString());
        return;
    }

    QJsonObject langs = obj.value("langs").toObject();  // Codes and Languages.
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
             + QString("key=") + api_key
             + QString("&text=") + source_text
             //+ QString("&hint=") + <list of probable text languages>
             //+ QString("&callback=") + <name of the callback function>
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
             + QString("key=") + api_key
             + QString("&lang=")
             + languages.key(source_lang) + "-" + languages.key(target_lang)
             + QString("&text=") + ui->SourceEdit->toPlainText()
             //+ QString("&format=") + <text format>
             //+ QString("&options=") + <translation options>
             //+ QString("&callback=") + <name of the callback function>
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

    //QString dir = obj.value("lang").toString();   // Use it if you need it.

    // Show the translated text.
    QJsonArray text = obj.value("text").toArray();
    ui->TargetEdit->clear();
    for(QJsonValue v : text)
    {
        ui->TargetEdit->append(v.toString());
    }
}
