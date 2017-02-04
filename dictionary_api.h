#ifndef DICTIONARY_API_H
#define DICTIONARY_API_H

#include <QObject>

#include <QVector>
#include <QString>
#include <QStringList>
#include <QMap>

class Example
{
public:
    Example();
    ~Example();

    void setText(QString& text_)        { text = text_; }
    void addTranslation(QString& tr_)   { translations << tr_; }

    void showExample(); // Show a usage example.
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
    void setNounForm(QString& num_)     { num = num_; }
    void setGender(QString& gen_)       { gen = gen_; }
    void addSynonym(QString& syn_)      { synonyms << syn_; }
    void addMeaning(QString& mean_)     { meanings << mean_; }
    void addExample(Example& example_)  { examples.push_back(example_); }

    void showTranslation(); // Show a translation.
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

    void showEntry();   // Show a dictionary entry.
private:
    QString text;   // A word which you wanted to find a dictionary entry for.
    QString ts;     // Transcription.
    QString pos;    // Part of speech.
    QVector<Translation> translations; // Translations.
};

class YandexDictionary : public QObject
{
    Q_OBJECT
public:
    YandexDictionary();
    ~YandexDictionary();

    QMap<QString, QString>& getLanguages() { return languages; }

    void getTranslationDirections();// Get a list of translation directions from Dictionary API.
    QVector<DictionaryEntry> getDictionaryEntry(QString& source_lang, QString& target_lang, QString& text);    // Get an automatically generated dictionary entry for a word or phrase.

public slots:
    void getLanguages(QMap<QString, QString> languages_);    // Get languages and their codes from Translate API.

private:
    QString dict_api_key = "";   // Put your Dictionary API key here.
    QStringList directions; // Translation directions. They can be "en-en", "ru-ru", for example, but we don't consider this case.
    QMap<QString, QString> languages; // Supported languages are values, their codes are keys. They're acquired from Translate API!
};

#endif // DICTIONARY_API_H
