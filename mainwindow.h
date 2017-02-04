#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Add the libraries.
#include "translate_api.h"
#include "dictionary_api.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_detectButton_clicked();

    void on_dictionaryButton_clicked();

    void on_translateButton_clicked();

private:
    Ui::MainWindow *ui;

    // Add these classes to provide Dictionary API and Translate API support to the application.
    YandexTranslate ya_translate;
    YandexDictionary ya_dictionary;
};

#endif // MAINWINDOW_H
