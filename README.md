# Qt-YandexTranslate

This project is dedicated to those who want to understand how to work with the Yandex.Translate API in Qt using C++. Inasmuch as the project is for educational purpose mainly, I will probably add some new code from time to time to it if it helps you (and me!) to undestand some peculiarities of both Qt and C++.

## Overview
#### Features:
 * Get the list of supported languages
 * Detect the language
 * Translate text
 * GUI
 * Line by line commentary
 * Based on [Yandex.Translate API Documentation](https://tech.yandex.com/translate/doc/dg/concepts/About-docpage/)


## Getting Started and Tips

First of all, make sure you have your **API key** and you have filled an empty space in **yandextranslate.h** with data:
```
QString api_key = "";    // Put your API key here.
```
In fact, that's all what you need to build and run the project. 

I used JSON interface to access Yandex.Translate API, therefore you can see how to parse JSON correctly in Qt. 

If you have any questions, feel free to ask me about them!


## Powered by [Yandex.Translate](http://translate.yandex.com/)
