#ifndef TRANSLATE_API_H
#define TRANSLATE_API_H

#include <QObject>

#include <QString>
#include <QStringList>
#include <QMap>

class YandexTranslate : public QObject
{
    Q_OBJECT
public:
    YandexTranslate();
    ~YandexTranslate();

    QMap<QString, QString>& getLanguages() { return languages; }

    void getSupportedLanguages();   // Get the list of supported languages from Translate API.
    QString detectLanguage(QString &source_text);    // Detect and return a source text language name.
    QStringList translate(QString& source_lang, QString& target_lang, QString& text_to_translate);   // Translate the text and return a result.

public slots:

signals:
    void languagesReceived(QMap<QString, QString> languages);   // This signal is emitted when getSupportedLanguages() is called.

private:
    QString trnsl_api_key = "";    // Put your Translate API key here.
    QMap<QString, QString> languages; // Supported languages are values, their codes are keys.
};

#endif // TRANSLATE_API_H
