#ifndef YANDEXTRANSLATE_H
#define YANDEXTRANSLATE_H

#include <QMainWindow>
#include <QString>
#include <QMap>

namespace Ui {
class YandexTranslate;
}

class YandexTranslate : public QMainWindow
{
    Q_OBJECT

public:
    explicit YandexTranslate(QWidget *parent = 0);
    ~YandexTranslate();

    void getSupportedLanguages();   // Get the list of supported languages from Translate API and show them in comboBoxes.
    void getTranslationDirections();// Get a list of translation directions from Dictionary API.

public slots:
    void detectLanguage();  // Detect a source language and show the result in a corresponding comboBox.
    void translate();       // Translate some text and show the result.

    void getDictionaryEntry();  // Get an automatically generated dictionary entry for a word or phrase.
private:
    Ui::YandexTranslate *ui;

    // Translate API
    QString trnsl_api_key = "";    // Put your Translate API key here.
    QMap<QString, QString> languages; // Supported languages are values, their codes are keys.

    // Dictionary API
    QString dict_api_key = "";   // Put your Dictionary API key here.
    QStringList directions; // Translation directions. They can be "en-en", "ru-ru", for example, but we don't consider this case.
};

#endif // YANDEXTRANSLATE_H
