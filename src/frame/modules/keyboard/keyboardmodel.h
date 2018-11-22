/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KEYBOARDMODEL_H
#define KEYBOARDMODEL_H

#include <QObject>
#include <QStringList>
#include <QMap>
#include "indexmodel.h"


static QStringList ModelKeylist = { "Esc" , "F1" , "F2"  , "F3" , "F4"  , "F5" , "F6" , "F7" , "F8" , "F9" , "F10" , "F11" , "F12" , "Power"
                               ,  "~" , "1" , "2" , "3" , "4" , "5" , "6" , "7" , "8" , "9" , "0" ,  "-" , "=" , "Delete" ,  "Tab" , "Q"
                               , "W" , "E" , "R" , "T" , "Y" , "U" , "I" , "O" , "P" , "[" , "]" , "\\" , "CAPS" , "A" , "S" , "D"
                               , "F" , "G" , "H" , "J" , "K" , "L" , ";" , "'" , "ENTER" , "Shift" , "Z" , "X" , "C" , "V" , "B"
                               , "N" , "M" , ",<" , ">." , "?/" , "Shift" , "Fn" , "Ctrl" , "Alt" , "Super" , "Space" , "Alt" , "Left" , "Up"
                               , "Right" , "Down"};

static QMap<QString, QString> ModelKeycode = {{"minus", "-"},{"equal","="},{"backslash" ,"\\"},{"question" ,"?/"},{"exclam" ,"1"},{"numbersign" ,"3"},
                                           {"semicolon" ,";"},{"apostrophe","'"},{"less",",<"},{"period" ,">."},{"slash" ,"?/"},{"parenleft" ,"9"},{"bracketleft" ,"["},
                                           {"parenright" ,"0"},{"bracketright" ,"]"},{"quotedbl" ,"'"},{"space" ," "},{"dollar" ,"$"},{"plus" ,"+"},{"asterisk" ,"*"},
                                           {"underscore" ,"_"},{"bar" ,"|"},{"grave" ,"`"},{"at" ,"2"},{"percent" ,"5"},{"greater" ,">."},{"asciicircum" ,"6"},
                                           {"braceleft" ,"["},{"colon" ,":"},{"comma" ,",<"},{"asciitilde" ,"~"},{"ampersand" ,"7"},{"braceright" ,"]"},{"Escape" ,"Esc"}};

namespace dcc {
namespace keyboard{

class KeyboardModel : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardModel(QObject *parent = 0);

#ifndef DCC_DISABLE_KBLAYOUT
    void setLayoutLists(QMap<QString, QString> lists);
#endif
    QString langByKey(const QString& key) const;

    QString curLayout() const;
    QString curLang() const;
    QMap<QString, QString> userLayout() const;
    QMap<QString, QString> kbLayout() const;
    QList<MetaData> langLists() const;
    bool capsLock() const;
    QMap<QStringList,int> allShortcut() const;

    uint repeatInterval() const;
    void setRepeatInterval(const uint &repeatInterval);

    uint repeatDelay() const;
    void setRepeatDelay(const uint &repeatDelay);

    bool numLock() const;
    void setNumLock(bool numLock);

    void cleanUserLayout();

    int kbSwitch() const;
    void setKbSwitch(int kbSwitch);

Q_SIGNALS:
#ifndef DCC_DISABLE_KBLAYOUT
    void curLayoutChanged(const QString& layout);
#endif
    void curLangChanged(const QString& lang);
    void capsLockChanged(bool value);
    void numLockChanged(bool value);
    void repeatDelayChanged(const uint value);
    void repeatIntervalChanged(const uint value);
    void userLayoutChanged(const QString &id, const QString &value);
    void langChanged(const QList<MetaData> &data);
    void kbSwitchChanged(int kbSwitch);

public Q_SLOTS:
#ifndef DCC_DISABLE_KBLAYOUT
    void setLayout(const QString& value);
#endif
    void setLang(const QString& value);
    void addUserLayout(const QString& id, const QString& value);
    void setLocaleList(const QList<MetaData>& langs);
    void setCapsLock(bool value);
    void setAllShortcut(const QMap<QStringList,int> &map);

private:
    bool m_capsLock;
    bool m_numLock;
    uint m_repeatInterval;
    uint m_repeatDelay;
    QString m_layout;
    QString m_currentLangKey;
    QMap<QString, QString> m_userLayout;
    QMap<QString, QString> m_layouts;
    QList<MetaData> m_langList;
    QMap<QStringList,int> m_shortcutMap;
    int m_kbSwitch;
};
}
}
#endif // KEYBOARDMODEL_H
