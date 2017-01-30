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

    void getSupportedLanguages();   // Get the list of supported languages and show them in comboBoxes.

public slots:
    void detectLanguage();
    void translate();

private:
    Ui::YandexTranslate *ui;

    QString api_key;    // Put your API key here.
    QMap<QString, QString> languages; // Supported languages are values, their codes are keys.
    //QStringList directions; // Deprecated: Translation directions.
};

#endif // YANDEXTRANSLATE_H
