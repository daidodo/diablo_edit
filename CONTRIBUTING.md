# How to Contribute

Thank you for helping improve the project!

## Introduction

The software is developed in MFC on **Microsoft Visual Studio 2017**.

There are 2 projects in *暗黑II.sln*:

* Diablo Edit2: Main project for the editor.
* Generate Data: Helper poject to generate language and game data for the editor.

## Language Support

The editor supports 3 languages: English, Traditional and Simplified Chinese.

All language data are in *Diablo Edit2\language.dat* which is generated from *Generate Data\language.txt*.

### Add Translations

Currently, Traditional and Simplified Chinese translations are incomplete. If you want to add translations, here are the steps:

* Open *Generate Data\language.txt* via **Microsoft Excel**.
  * `*LANG`: File tag. DO NOT MODIFY!
  * First column: String index. If empty, it'll start from 0 and increase automatically.
  * Remaining columns: Translations of all languages.
  * Other lines starting with `*`: Section headers. A section is a group of translations, e.g. all skill names.
  ![image](https://user-images.githubusercontent.com/8170176/166330206-ddefe7b4-dc93-4686-a263-8e7036b75a56.png)
* Find the string(s) you want to translate, and add missing translations
  ![image](https://user-images.githubusercontent.com/8170176/166332112-dec1ade3-6bc2-426a-9514-051a2bc62e06.png)
* Save the file and KEEP the original format.
* Compile and run **Generate Data** project. It'll refresh all data files for the editor.
* Compile and run **Diablo Edit2** project, and check your translations on UI.
  * Select language via menu: View => Language
  * Confirm your changes
  ![image](https://user-images.githubusercontent.com/8170176/166332972-f10ceafe-b2fb-43bd-8651-0fb4b784f8c2.png)

