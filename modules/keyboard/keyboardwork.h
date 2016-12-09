#ifndef KEYBOARDWORK_H
#define KEYBOARDWORK_H

#include <QObject>
#include "indexmodel.h"
#include "shortcutmodel.h"
#include "keyboardmodel.h"
#include <com_deepin_daemon_inputdevice_keyboard.h>
#include <com_deepin_daemon_langselector.h>
#include <com_deepin_daemon_keybinding.h>


namespace dcc {
namespace keyboard{
using KeyboardInter = com::deepin::daemon::inputdevice::Keyboard;
using LangSelector = com::deepin::daemon::LangSelector;
using KeybingdingInter = com::deepin::daemon::Keybinding;

class KeyboardWork : public QObject
{
    Q_OBJECT

public:
    explicit KeyboardWork(KeyboardModel* model, QObject *parent = 0);
    void getProperty();

    QString curLang() const;
    QString curLayout() const;
    QStringList userLayout() const;
    KeyboardLayoutList layoutLists() const;
    void modifyShortcut(ShortcutInfo* info, const QString& key, bool clear = false);
    bool addCustonShortcut(const QString& name, const QString& command, const QString& accels, bool &result);

    void grabScreen();
    bool checkAvaliable(const QString& key);
    QString query(const QString& id, qint32 type);
    void delShortcut(ShortcutInfo *info);

    void setRepeatDelay(int value);
    uint repeatDelay() const;
    void setRepeatInterval(int value);
    uint repeatInterval() const;

    void setCapsLock(bool value);
signals:
    void langValid(const QList<MetaData>& langs);
    void curLayout(const QString& layout);
    void curLang(const QString& lang);
    void delLayout(const QString& layout);
    void addLayout(const QString& layout);
    void UserLayoutListChanged(const QStringList & value) const;
    void shortcutInfo(const QString& info);
    void custonInfo(const QString& info);
    void Deleted(const QString &in0, int in1);
    void KeyEvent(bool in0, const QString &in1);
    void searchChangd(ShortcutInfo* info, const QString& key);

public slots:
    void setLang(const QString& value);
    void setLayout(const QString& value);
    void addUserLayout(const QString& value);
    void delUserLayout(const QString& value);
    void onValid(bool value);
    void onLocaleListFinish(QDBusPendingCallWatcher* watch);
    void onRequestShortcut(QDBusPendingCallWatcher* watch);
    void onAdded(const QString&in0, int in1);

private:
    int m_delayValue;
    int m_speedValue;
    KeyboardModel* m_model;
    KeyboardInter* m_keyboardInter;
    LangSelector* m_langSelector;
    KeybingdingInter* m_keybindInter;
    QList<MetaData> m_datas;
};
}
}
#endif // KEYBOARDWORK_H
