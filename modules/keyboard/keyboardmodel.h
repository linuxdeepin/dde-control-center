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

    void setLayoutLists(QMap<QString, QString> lists);

    QString layoutByValue(const QString& value);
    QString langByKey(const QString& key) const;

    QString curLayout() const;
    QString curLang() const;
    QStringList userLayout() const;
    QList<MetaData> langLists() const;
    bool capsLock() const;
    QMap<QStringList,int> allShortcut() const;

    uint repeatInterval() const;
    void setRepeatInterval(const uint &repeatInterval);

    uint repeatDelay() const;
    void setRepeatDelay(const uint &repeatDelay);

signals:
    void curLayoutChanged(const QString& layout);
    void curLangChanged(const QString& lang);
    void capsLockChanged(bool value);
    void repeatDelayChanged(const uint value);
    void repeatIntervalChanged(const uint value);

public slots:
    void setLayout(const QString& value);
    void setLang(const QString& value);
    void setUserLayout(const QStringList& list);
    void addUserLayout(const QString& value);
    void delUserLayout(const QString& value);
    void setLocaleList(const QList<MetaData>& langs);
    void setCapsLock(bool value);
    void setAllShortcut(const QMap<QStringList,int> &map);

private:
    bool m_capsLock;
    uint m_repeatInterval;
    uint m_repeatDelay;
    QString m_layout;
    QString m_lang;
    QStringList m_userLayout;
    QMap<QString, QString> m_layouts;
    QList<MetaData> m_langs;
    QMap<QStringList,int> m_shortcutMap;
};
}
}
#endif // KEYBOARDMODEL_H
