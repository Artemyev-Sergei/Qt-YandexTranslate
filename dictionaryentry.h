#ifndef DICTIONARYENTRY_H
#define DICTIONARYENTRY_H

#include <QVector>
#include <QString>
#include <QStringList>


class Example
{
public:
    Example();
    ~Example();

    void setText(QString& text_)        { text = text_; }
    void addTranslation(QString& tr_)   { translations << tr_; }

    void showExample();
private:
    QString text;               // Text of an example.
    QStringList translations;   // Translations of the text of an example.
};

class Translation
{
public:
    Translation();
    ~Translation();

    void setText(QString& text_)        { text = text_; }
    void setPartOfSpeech(QString& pos_) { pos = pos_; }
    void setAspect(QString& asp_)       { asp = asp_; }
    void setNounForm(QString& num_)     { text = num_; }
    void setGender(QString& gen_)       { text = gen_; }
    void addSynonym(QString& syn_)      { synonyms << syn_; }
    void addMeaning(QString& mean_)     { meanings << mean_; }
    void addExample(Example& example_)  { examples.push_back(example_); }

    void showTranslation();
private:
    QString text;   // Translation text.
    QString pos;    // Part of speech.
    QString asp;    // The aspect of a verb.
    QString num;    // The form of a noun and its variations: plural, etc.
    QString gen;    // Gender.
    QStringList synonyms;// Synonyms.
    QStringList meanings;   // Meanings.
    QVector<Example> examples;// Usage examples.
};

class DictionaryEntry
{
public:
    DictionaryEntry();
    ~DictionaryEntry();

    void setText(QString& text_)            { text = text_; }
    void setTranscription(QString& ts_)     { ts = ts_; }
    void setPartOfSpeech(QString& pos_)     { pos = pos_; }
    void addTranslation(Translation& tr_)   { translations.push_back(tr_); }

    void showEntry();
private:
    QString text;   // A word which you wanted to find a dictionary entry for.
    QString ts;     // Transcription.
    QString pos;    // Part of speech.
    QVector<Translation> translations; // Translations.
};

#endif // DICTIONARYENTRY_H
