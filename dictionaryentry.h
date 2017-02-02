#ifndef DICTIONARYENTRY_H
#define DICTIONARYENTRY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

class Example
{
public:
    Example();
    ~Example();
private:
    QString text;               // Example text.
    QStringList translations;   // Translations of an example text.
};

class Translation
{
public:
    Translation();
    ~Translation();
private:
    QString text;
    QString pos;    // Part of speech.
    QString asp;    // The aspect of a verb.
    QString num;    // The form of a noun and its variations: plural, etc.
    QString gen;    // Gender.
    QStringList sym;// Synonyms.
    QStringList mean;   // Meanings.
    QVector<Example> ex;// Usage examples.
};

class DictionaryEntry : public QObject
{
    Q_OBJECT
public:
    explicit DictionaryEntry(QObject *parent = 0);

signals:

public slots:

private:
    QString text;
    QString ts;     // Transcription.
    QString pos;    // Part of speech.
    QVector<Translation> tr; // Translations.
};

#endif // DICTIONARYENTRY_H
