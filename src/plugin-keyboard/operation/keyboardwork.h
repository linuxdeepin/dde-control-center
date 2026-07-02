//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KEYBOARDWORK_H
#define KEYBOARDWORK_H

#include "operation/metadata.h"
#include "shortcutmodel.h"
#include "keyboardmodel.h"
#include "keyboarddbusproxy.h"
#include "ikeyboarddeviceproxy.h"

#ifdef ENABLE_TREELAND_INPUT_MANAGER
#include "keyboardwaylandproxy.h"
#endif

#include <DConfig>

#include <QObject>

class QDBusPendingCallWatcher;
class QTranslator;

namespace dccV25 {
class KeyboardWorker : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardWorker(KeyboardModel* model, QObject *parent = nullptr);
    enum Modifier {
        control = 1,
        super = 2,
        alt = 4,
        shift = 8
    };

    void resetAll();

    void setShortcutModel(ShortcutModel * model);
    void refreshShortcut();
    void refreshLang();
    void windowSwitch();

    inline QList<MetaData> getDatas() {return m_metaDatas;}
    inline QList<QString> getLetters() {return m_letters;}

    void modifyShortcutEditAux(ShortcutInfo* info, bool isKPDelete = false);
    void modifyShortcutEdit(ShortcutInfo* info);
    void addCustomShortcut(const QString& name, const QString& command, const QString& accels);
    void modifyCustomShortcut(ShortcutInfo *info);
    // Fire ModifyCustomShortcut on the Wayland service and wire the reply to
    // onModifyCustomShortcutFinished. Shared by modifyCustomShortcut (Wayland
    // branch) and setNewCustomShortcut so the watcher/id/type setup stays in
    // one place.
    void callModifyCustomShortcut(const QString &id, const QString &name,
                                  const QString &command, const QString &accels,
                                  int type);

    void grabScreen();
    bool checkAvaliable(const QString& key);
    QString lookupConflictingShortcut(const QString &key);
    void delShortcut(ShortcutInfo *info);

    void setRepeatDelay(uint value);
    void setRepeatInterval(uint value);
    void setModelRepeatDelay(uint value);
    void setModelRepeatInterval(uint value);

    void setNumLock(bool value);
    void setCapsLock(bool value);
    void setKeyboardEnabled(bool value);
    Q_INVOKABLE void active();

    void refreshKeyboard();
    void deactive();
    bool keyOccupy(const QStringList &list);
    void onRefreshKBLayout();

    // Single source of truth for the session type, shared with KeyboardController
    // so the controller's capture-manager guard can't diverge from the proxy
    // that decides which D-Bus API to use. Delegates to the env-based detection
    // (XDG_SESSION_TYPE / WAYLAND_DISPLAY) in KeyboardDBusProxy.
    bool isWayland() const { return m_keyboardDBusProxy->isWayland(); }

Q_SIGNALS:
    void KeyEvent(bool in0, const QString &in1);
    void searchChangd(ShortcutInfo* info, const QString& key);
    void removed(const QString &id, int type);
    void requestSetAutoHide(const bool visible);
    void onDatasChanged(QList<MetaData> datas);
    void onLettersChanged(QList<QString> letters);
    // 快捷键恢复默认完成
    void onResetFinished();

public Q_SLOTS:
    void setLang(const QString &value);
    void addLang(const QString &value);
    void deleteLang(const QString& value);
    void setLayout(const QString& value);
    void addUserLayout(const QString& value);
    void delUserLayout(const QString& value);
    void onRequestShortcut(QDBusPendingCallWatcher* watch);
    void onAllShortcutsReady(const QString &info);
    void onModifyHotkeysFinished(QDBusPendingCallWatcher *watch);
    void onReplaceHotkeyFinished(QDBusPendingCallWatcher *watch);
    void onAdded(const QString&in0, int in1);
    void onDisableShortcut(ShortcutInfo* info);
    void onAddedFinished(QDBusPendingCallWatcher *watch);
    void onLocalListsFinished(QDBusPendingCallWatcher *watch);
    void onGetWindowWM(bool value);
    void onLayoutListsFinished(QDBusPendingCallWatcher *watch);
    void onAllLayoutListsFinished(QDBusPendingCallWatcher *watch);
    void onUserLayout(const QStringList &list);
    void onUserLayoutFinished(QDBusPendingCallWatcher *watch);
    void onCurrentLayout(const QString &value);
    void onCurrentLayoutFinished(QDBusPendingCallWatcher *watch);
    void onPinyin();
    void onSearchShortcuts(const QString &searchKey);
    void onSearchFinished(QDBusPendingCallWatcher *watch);
    void append(const MetaData& md);
    void onLangSelectorServiceFinished();
    void onShortcutChanged(const QString &id, int type);
    void onGetShortcutFinished(QDBusPendingCallWatcher *watch);
    void updateKey(ShortcutInfo *info);
    void cleanShortcutSlef(const QString &id, const int type, const QString &shortcut);
    void setNewCustomShortcut(const QString &id, const QString &name, const QString &command, const QString &accles);
    void onConflictShortcutCleanFinished(QDBusPendingCallWatcher *watch);
    void onLookupConflictForShortcutFinished(QDBusPendingCallWatcher *watch);
    void onShortcutCleanFinished(QDBusPendingCallWatcher *watch);
    void onCustomConflictCleanFinished(QDBusPendingCallWatcher *w);
    void onAddCustomShortcutFinished(QDBusPendingCallWatcher *watch);
    void onModifyCustomShortcutFinished(QDBusPendingCallWatcher *watch);
    void onDeleteCustomShortcutFinished(QDBusPendingCallWatcher *watch);

private:
    static QString makeShortcutKey(const QString &id, int type);
    void initKeyboardEnabledSync();
    void syncKeyboardEnabled();
    uint converToDBusDelay(uint value);
    uint converToModelDelay(uint value);
    int converToDBusInterval(int value);
    uint converToModelInterval(uint value);
    void parseShortcutListJson(const QString &info);

private:
    QList<MetaData> m_datas;
    QList<MetaData> m_metaDatas;
    QList<QString> m_letters;
    int m_delayValue;
    int m_speedValue;
    KeyboardModel* m_model;
    KeyboardDBusProxy *m_keyboardDBusProxy;
    ShortcutModel *m_shortcutModel = nullptr;
    QTranslator *m_translatorLanguage;
    Dtk::Core::DConfig *m_inputDevCfg;
    bool m_isResetting = false; // Flag to prevent duplicate reset calls
    QMap<QString, qint64> m_shortcutQueryTime; // 记录每个快捷键最后一次 Query 的时间戳
    QSet<QString> m_replacingShortcuts; // 记录正在替换的快捷键，用于屏蔽中间状态的UI更新
    bool m_isTreelandSession = false;
#ifdef ENABLE_TREELAND_INPUT_MANAGER
    DCC_NAMESPACE::KeyboardWaylandProxy *m_waylandProxy = nullptr;
#endif
    DCC_NAMESPACE::IKeyboardDeviceProxy *m_deviceProxy = nullptr; ///< 当前会话的键盘设备代理（构造时注入）
};
}
#endif // KEYBOARDWORK_H
