#include "yandextranslate.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    YandexTranslate w;
    w.show();

    return a.exec();
}
