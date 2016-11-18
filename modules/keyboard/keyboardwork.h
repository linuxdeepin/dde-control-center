#ifndef KEYBOARDWORK_H
#define KEYBOARDWORK_H

#include <QObject>
#include "indexmodel.h"
#include <com_deepin_daemon_inputdevice_keyboard.h>
#include <com_deepin_daemon_langselector.h>
#include <com_deepin_daemon_keybinding.h>

using KeyboardInter = com::deepin::daemon::inputdevice::Keyboard;
using LangSelector = com::deepin::daemon::LangSelector;
using KeybingdingInter = com::deepin::daemon::Keybinding;

class KeyboardWork : public QObject
{
    Q_OBJECT

public:
    explicit KeyboardWork(QObject *parent = 0);
    void getProperty();

    QString curLang() const;
    QString curLayout() const;
    QStringList userLayout() const;
    KeyboardLayoutList layoutLists() const;

signals:
    void langValid(const QList<MetaData>& langs);
    void curLayout(const QString& layout);
    void curLang(const QString& lang);
    void delLayout(const QString& layout);
    void addLayout(const QString& layout);
    void UserLayoutListChanged(const QStringList & value) const;
    void shortcutInfo(const QString& info);

public slots:
    void setLang(const QString& value);
    void setLayout(const QString& value);
    void addUserLayout(const QString& value);
    void delUserLayout(const QString& value);
    void onValid(bool value);
    void onLocaleListFinish(QDBusPendingCallWatcher* watch);
    void onRequestShortcut(QDBusPendingCallWatcher* watch);

private:
    KeyboardInter* m_keyboardInter;
    LangSelector* m_langSelector;
    KeybingdingInter* m_keybindInter;
    QList<MetaData> m_datas;
};

#endif // KEYBOARDWORK_H
