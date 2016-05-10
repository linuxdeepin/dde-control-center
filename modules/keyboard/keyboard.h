/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QObject>
#include <QtPlugin>
#include <QRunnable>
#include <QSettings>

#include <dwidget_global.h>
#include <dbuttonlist.h>

#include "interfaces.h"

#include "dbus/dbuskeyboard.h"

DWIDGET_USE_NAMESPACE

class QFrame;
class SearchItem;
class SearchList;
class AddRmDoneLine;
class FirstLetterClassify;
class QVBoxLayout;
class KeyboardLayoutDelegate;
class DbusLangSelector;
class LocaleInfo;
class KeyboardModule: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "keyboard.json")
    Q_INTERFACES(ModuleInterface)

public:
    QFrame *getContent() Q_DECL_OVERRIDE;
};

class Keyboard: public QObject, QRunnable
{
    Q_OBJECT

public:
    Keyboard();
    ~Keyboard();
    QFrame *getContent();

signals:
    void addLayoutItem(const QString &id, const QString &title, const QStringList &letterFirstList);

protected:
    void run() Q_DECL_OVERRIDE;

private:
    void updateKeyboardLayout(SearchList *button_list, AddRmDoneLine *line, bool showRemoveButton = false);
    void initBackend();
    void initUI();

private slots:
    void loadLater();
    void onAddLayoutItem(const QString &id, const QString &title, const QStringList &letterFirstList);
    void loadLanguageList(const QList<LocaleInfo> &infos);

private:
    QFrame *m_frame;
    QLabel *m_languageTips;
    QVBoxLayout *m_mainLayout;
    SearchList *m_langItemList;
    FirstLetterClassify *m_letterClassifyList;

    QSettings *m_settings;

    DBusKeyboard *m_dbusKeyboard;
    DbusLangSelector *m_dbusLangSelector;

    QMap<QString, QString> m_mapUserLayoutInfo;
    QMap<QString, int> m_mapUserLayoutIndex;
    QList<KeyboardLayoutDelegate *> m_selectLayoutList;
};

#endif //   KEYBOARD_H
