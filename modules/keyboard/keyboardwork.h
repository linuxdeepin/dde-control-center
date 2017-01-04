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
    inline QList<MetaData> getDatas() {return m_metaDatas;}
    inline QList<QString> getLetters() {return m_letters;}

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
    void active();
    void deactive();
signals:
    void shortcutInfo(const QString& info);
    void custonInfo(const QString& info);
    void KeyEvent(bool in0, const QString &in1);
    void searchChangd(ShortcutInfo* info, const QString& key);
    void requestSetLangTitle(const QString &title);

public slots:
    void setLang(const QString& value);
    void setLayout(const QString& value);
    void addUserLayout(const QString& value);
    void delUserLayout(const QString& value);
    void onValid();
    void onLocaleListFinish(QDBusPendingCallWatcher* watch);
    void onRequestShortcut(QDBusPendingCallWatcher* watch);
    void onAdded(const QString&in0, int in1);

private:
    void append(const MetaData& md);

private:
    QList<MetaData> m_datas;
    QList<MetaData> m_metaDatas;
    QList<QString> m_letters;
    int m_delayValue;
    int m_speedValue;
    KeyboardModel* m_model;
    KeyboardInter* m_keyboardInter;
    LangSelector* m_langSelector;
    KeybingdingInter* m_keybindInter;
};
}
}
#endif // KEYBOARDWORK_H
