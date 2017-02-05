# Qt-YandexTranslate

This project is dedicated to those who want to understand how to work with the Yandex.Translate API and Yandex.Dictionary API in Qt using C++. Inasmuch as the project is for educational purposes mainly, I will probably add some new code from time to time to the project if it helps you (and me!) to undestand some peculiarities of both Qt and C++. In fact, I use this project for different experiments with Git as well.

## Overview

Here is the list of what you can do using this code in your own application:

 * Get a list of supported languages
 * Get a list of supported translation directions
 * Get a dictionary entry for a word or a phrase
 * Detect the language
 * Translate text

Generally speaking, the project features are:

 * Example of usage with GUI
 * Line by line commentary
 * [Yandex.Translate API](https://tech.yandex.com/translate/doc/dg/concepts/About-docpage/) support
 * [Yandex.Dictionary API](https://tech.yandex.com/dictionary/doc/dg/concepts/About-docpage/) support

## Getting Started and Tips

Let's assume that you cloned the repository and opened the project in Qt Creator. 

Then, first of all, make sure you have acquired your [**API keys**](https://tech.yandex.com/) and you have filled empty spaces in the corresponding header files with data. For example, in **translate_api.h** you will find the following line where you should put your API key:

``
QString trnsl_api_key = "";    // Put your Translate API key here.
``

That's basically everything you need to do before building and runnning the project successfully. 

The **dictionary_api.h** and **dictionary_api.cpp** files represent how Yandex.Dictionary API support was implemented.

Thee **translate_api.h** and **translate_api.cpp** files represent how Yandex.Translate API support was implemented.

The **mainwindow.h** and **mainwindow.cpp** files represent an example of libraries usage with GUI.

I used JSON interface to access the APIs, therefore you can see how to parse JSON using Qt.

I think there's a lot of space for improvement, therefore if you have any questions or suggestions, feel free to contact me about them!

### Powered by [Yandex.Translate](http://translate.yandex.com/) 
### Powered by [Yandex.Dictionary](https://tech.yandex.com/dictionary/)
