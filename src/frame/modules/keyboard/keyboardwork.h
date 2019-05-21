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

class KeyboardWorker : public QObject
{
    Q_OBJECT

public:
    explicit KeyboardWorker(KeyboardModel* model, QObject *parent = 0);
    enum Modifier {
        control = 1,
        super = 2,
        alt = 4,
        shift = 8
    };

    void resetAll();

    void setShortcutModel(ShortcutModel * model);

    void refreshShortcut();

#ifndef DCC_DISABLE_LANGUAGE
    void refreshLang();
#endif

    inline QList<MetaData> getDatas() {return m_metaDatas;}
    inline QList<QString> getLetters() {return m_letters;}

    void modifyShortcutEdit(ShortcutInfo* info);
    void addCustomShortcut(const QString& name, const QString& command, const QString& accels);
    void modifyCustomShortcut(ShortcutInfo *info);

    void grabScreen();
    bool checkAvaliable(const QString& key);
    void delShortcut(ShortcutInfo *info);

    void setRepeatDelay(int value);
    void setRepeatInterval(int value);
    void setModelRepeatDelay(int value);
    void setModelRepeatInterval(int value);

    void setNumLock(bool value);
    void setCapsLock(bool value);
    void active();
    void deactive();
    bool keyOccupy(const QStringList &list);

#ifndef DCC_DISABLE_KBLAYOUT
    void onRefreshKBLayout();
#endif

Q_SIGNALS:
    void KeyEvent(bool in0, const QString &in1);
    void searchChangd(ShortcutInfo* info, const QString& key);
    void removed(const QString &id, int type);
    void requestSetAutoHide(const bool visible);
    void onDatasChanged(QList<MetaData> datas);
    void onLettersChanged(QList<QString> letters);

public Q_SLOTS:
#ifndef DCC_DISABLE_LANGUAGE
    void setLang(const QString& value);
#endif
    void setLayout(const QString& value);
    void addUserLayout(const QString& value);
    void delUserLayout(const QString& value);
    void onRequestShortcut(QDBusPendingCallWatcher* watch);
    void onAdded(const QString&in0, int in1);
    void onDisableShortcut(ShortcutInfo* info);
    void onAddedFinished(QDBusPendingCallWatcher *watch);
    void onLocalListsFinished(QDBusPendingCallWatcher *watch);
    void onSetSwitchKBLayout(int value);
#ifndef DCC_DISABLE_KBLAYOUT
    void onLayoutListsFinished(QDBusPendingCallWatcher *watch);
    void onUserLayout(const QStringList &list);
    void onUserLayoutFinished(QDBusPendingCallWatcher *watch);
    void onCurrentLayout(const QString &value);
    void onCurrentLayoutFinished(QDBusPendingCallWatcher *watch);
    void onPinyin();
    void append(const MetaData& md);
#endif

#ifndef DCC_DISABLE_LANGUAGE
    void onLangSelectorServiceFinished();
#endif

    void onShortcutChanged(const QString &id, int type);
    void onGetShortcutFinished(QDBusPendingCallWatcher *watch);
    void updateKey(ShortcutInfo *info);

    void cleanShortcutSlef(const QString &id, const int type, const QString &shortcut);
    void setNewCustomShortcut(const QString &id, const QString &name, const QString &command, const QString &accles);
    void onConflictShortcutCleanFinished(QDBusPendingCallWatcher *watch);
    void onShortcutCleanFinished(QDBusPendingCallWatcher *watch);
    void onCustomConflictCleanFinished(QDBusPendingCallWatcher *w);

private:
    int converToDBusDelay(int value);
    int converToModelDelay(int value);
    int converToDBusInterval(int value);
    int converToModelInterval(int value);

private:
    QList<MetaData> m_datas;
    QList<MetaData> m_metaDatas;
    QList<QString> m_letters;
    int m_delayValue;
    int m_speedValue;
    KeyboardModel* m_model;
    KeyboardInter* m_keyboardInter;
#ifndef DCC_DISABLE_LANGUAGE
    LangSelector* m_langSelector;
#endif
    KeybingdingInter* m_keybindInter;
    ShortcutModel *m_shortcutModel;
};
}
}
#endif // KEYBOARDWORK_H
