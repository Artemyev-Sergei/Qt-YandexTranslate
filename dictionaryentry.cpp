#include "dictionaryentry.h"

#include <QDebug>

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
