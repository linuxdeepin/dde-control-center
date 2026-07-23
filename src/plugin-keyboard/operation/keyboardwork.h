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

#include <QHash>
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

    void modifyShortcutEditAux(ShortcutInfo *info, quint64 generation);
    void modifyShortcutEdit(ShortcutInfo *info, quint64 generation);
    void replaceShortcutEdit(ShortcutInfo *info, const QString &expectedConflictId,
                             quint64 generation);
    void setShortcutGeneration(const QString &id, int type, quint64 generation);
    void addCustomShortcut(const QString& name, const QString& command, const QString& accels,
                           quint64 requestId);
    void modifyCustomShortcut(ShortcutInfo *info, quint64 requestId);
    void replaceCustomShortcut(const QString &id, const QString &name,
                               const QString &command, const QString &accels,
                               const QString &expectedConflictId, quint64 generation,
                               quint64 requestId);
    void requestShortcutCommand(const QString &id, quint64 requestSerial);
    // Fire ModifyCustomShortcut on the Wayland service and wire the reply to
    // onModifyCustomShortcutFinished. Shared by modifyCustomShortcut (Wayland
    // branch) and setNewCustomShortcut so the watcher/id/type setup stays in
    // one place.
    void callModifyCustomShortcut(const QString &id, const QString &name,
                                  const QString &command, const QString &accels,
                                  int type, quint64 requestId = 0);

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
    void searchChangd(ShortcutInfo* info, const QString& key);
    void removed(const QString &id, int type);
    void requestSetAutoHide(const bool visible);
    void onDatasChanged(QList<MetaData> datas);
    void onLettersChanged(QList<QString> letters);
    // 快捷键恢复默认完成
    void onResetFinished();
    void shortcutCommandReady(const QString &id, const QString &command, bool available,
                              quint64 requestSerial);
    void captureRequestFinished(quint64 requestId, bool success, const QString &reason);
    void shortcutConflictDetected(const QString &id, int type, const QString &shortcut,
                                  const QString &conflictId, const QString &conflictName,
                                  bool replaceable, quint64 generation);
    void shortcutModificationFinished(const QString &id, int type, const QString &shortcut,
                                      bool success, quint64 generation);
    void customShortcutOperationFinished(quint64 requestId, bool success,
                                         const QString &errorMessage);

public Q_SLOTS:
    void setLang(const QString &value);
    void addLang(const QString &value);
    void deleteLang(const QString& value);
    void setLayout(const QString& value);
    void addUserLayout(const QString& value);
    void delUserLayout(const QString& value);
    void onAllShortcutsReady(const QString &info);
    void onModifyHotkeysFinished(QDBusPendingCallWatcher *watch);
    void onReplaceHotkeyFinished(QDBusPendingCallWatcher *watch);
    void onAdded(const QString&in0, int in1);
    void onDisableShortcut(ShortcutInfo* info);
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
    void append(const MetaData& md);
    void onLangSelectorServiceFinished();
    void onShortcutChanged(const QString &id, int type);
    void updateKey(ShortcutInfo *info);
    void beginCapture(quint64 requestId);
    void endCapture();
    void cleanShortcutSlef(const QString &id, const int type, const QString &shortcut,
                           quint64 generation);
    void setNewCustomShortcut(const QString &id, const QString &name, const QString &command, const QString &accles);
    void onLookupConflictForShortcutFinished(QDBusPendingCallWatcher *watch);
    void onAddCustomShortcutFinished(QDBusPendingCallWatcher *watch);
    void onModifyCustomShortcutFinished(QDBusPendingCallWatcher *watch);
    void onDeleteCustomShortcutFinished(QDBusPendingCallWatcher *watch);
    void onGetShortcutCommandFinished(QDBusPendingCallWatcher *watch);

private:
    static QString shortcutKey(const QString &id, int type);
    bool isCurrentShortcutGeneration(const QString &id, int type, quint64 generation) const;
    void lookupShortcutConflict(const QString &id, int type, const QString &shortcut,
                                quint64 generation, bool reportFailureIfNoConflict = false);
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
    QHash<QString, quint64> m_shortcutGenerations;
    bool m_isTreelandSession = false;
#ifdef ENABLE_TREELAND_INPUT_MANAGER
    DCC_NAMESPACE::KeyboardWaylandProxy *m_waylandProxy = nullptr;
#endif
    DCC_NAMESPACE::IKeyboardDeviceProxy *m_deviceProxy = nullptr; ///< 当前会话的键盘设备代理（构造时注入）
};
}
#endif // KEYBOARDWORK_H
