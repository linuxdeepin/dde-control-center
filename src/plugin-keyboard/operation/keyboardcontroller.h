//SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KEYBOARDCONTROLLER_H
#define KEYBOARDCONTROLLER_H

#include "keyboardmodel.h"
#include "keyboardwork.h"

#include <QObject>
#include <QHash>
#include <QPointer>
#include <QSortFilterProxyModel>

class QQuickItem;

namespace dccV25 {

class TreelandShortcutCaptureManager;
class TreelandShortcutCaptureSession;

class KeyboardController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint repeatInterval READ repeatInterval WRITE setRepeatInterval NOTIFY repeatIntervalChanged FINAL)
    Q_PROPERTY(uint repeatDelay READ repeatDelay WRITE setRepeatDelay NOTIFY repeatDelayChanged FINAL)
    Q_PROPERTY(bool keyboardEnabled READ keyboardEnabled WRITE setKeyboardEnabled NOTIFY keyboardEnabledChanged FINAL)
    Q_PROPERTY(bool numLock READ numLock WRITE setNumLock NOTIFY numLockChanged FINAL)
    Q_PROPERTY(bool capsLock READ capsLock WRITE setCapsLock NOTIFY capsLockChanged FINAL)
    Q_PROPERTY(int layoutCount READ layoutCount NOTIFY layoutCountChanged FINAL)
    Q_PROPERTY(QString currentLayout READ currentLayout WRITE setCurrentLayout NOTIFY currentLayoutChanged FINAL)
    Q_PROPERTY(QString conflictText READ conflictText WRITE setConflictText NOTIFY conflictTextChanged FINAL)

public:
    explicit KeyboardController(QObject *parent = nullptr);

    uint repeatInterval() const;
    void setRepeatInterval(uint newRepeatInterval);

    uint repeatDelay() const;
    void setRepeatDelay(uint newRepeatDelay);

    bool keyboardEnabled() const;
    void setKeyboardEnabled(bool enabled);

    bool numLock() const;
    void setNumLock(bool newNumLock);

    bool capsLock() const;
    void setCapsLock(bool newCapsLock);

    int layoutCount() const;

    QString currentLayout() const;
    void setCurrentLayout(const QString &key);

    QString conflictText() const;
    void setConflictText(const QString &newConflictText);

public Q_SLOTS:
    void addUserLayout(const QString &layout);
    void deleteUserLayout(const QString &layout);

    QMap<QString, QString> userLayouts() const;
    QString userLayoutAt(int index, bool isValue = true) const;
    bool userLayoutsContains(const QString &key);
    QSortFilterProxyModel *layoutSearchModel();
    QSortFilterProxyModel *shortcutSearchModel();

    // Wayland: category metadata is supplied by the service (ListCategories).
    // Exposed to QML so the section delegate can render the display name for
    // a section key and detect the user-editable (Custom) group without any
    // hardcoded category strings.
    Q_INVOKABLE QString sectionDisplayName(const QString &key) const;
    Q_INVOKABLE QString customCategoryKey() const;

    void updateKey(const QString &id, const int &type);
    QStringList formatKeys(const QString &shortcuts);
    Q_INVOKABLE bool isWaylandSession() const;
    Q_INVOKABLE void beginKeyCapture(QQuickItem *item, const QString &id, int type);
    Q_INVOKABLE void endKeyCapture();
    Q_INVOKABLE void submitCapturedKeystroke(const QString &id, int type, const QString &accels);

    Q_INVOKABLE quint64 addCustomShortcut(const QString &name, const QString &cmd,
                                          const QString &accels);
    Q_INVOKABLE quint64 modifyCustomShortcut(const QString &id, const QString &name,
                                             const QString &cmd, const QString &accels);
    Q_INVOKABLE quint64 replaceCustomShortcut(const QString &id, const QString &name,
                                              const QString &cmd, const QString &accels);
    Q_INVOKABLE void clearPendingConflict(const QString &id, int type);
    Q_INVOKABLE quint64 requestShortcutCommand(const QString &id);
    void modifyShortcut(const QString &id, const QString &accels, const int &type);
    void replaceShortcut(const QString &id, const QString &accels, const int &type);
    void deleteCustomShortcut(const QString &id);
    void clearShortcut(const QString &id, const int &type);

    void resetAllShortcuts();
    
    // 检查自定义快捷键名称是否已存在（排除指定ID）
    bool isCustomShortcutNameExists(const QString &name, const QString &excludeId = QString());

    /** 刷新键盘 settings） */
    Q_INVOKABLE void refreshKeyboard();
    
    // 检查系统快捷键名称是否已存在
    bool isSystemShortcutNameExists(const QString &name);
    
    // 统一检查快捷键名称是否已存在（包含系统和自定义快捷键）
    bool isShortcutNameExists(const QString &name, const QString &excludeId = QString());

signals:
    void repeatIntervalChanged();
    void repeatDelayChanged();
    void numLockChanged();
    void capsLockChanged();
    void layoutCountChanged();
    void currentLayoutChanged();

    void requestRestore(const QString &id, int type);
    void requestClear(const QString &id, int type);
    void keyConflicted(const QString &id, int type, const QString &oldAccels,
                       const QString &newAccels, const QString &message, bool replaceable);
    void keyDone(const QString &id, int type, const QString &accels);
    void keyEvent(const QString &id, int type, const QString &accels);

    void keyCaptureStarted(const QString &id, int type);
    void keyCaptureFailed(const QString &id, int type, const QString &reason);
    void shortcutModificationFinished(const QString &id, int type, const QString &accels, bool success);
    void customShortcutOperationFinished(quint64 requestId, bool success,
                                         const QString &errorMessage);

    void waylandKeyCaptureFinished(const QString &id, int type, const QString &accels);
    void waylandKeyCaptureFailed(const QString &id, int type, int reason);
    void shortcutCommandReady(const QString &id, const QString &command, bool available,
                              quint64 requestSerial);

    void conflictTextChanged();
    void keyboardEnabledChanged();

private:
    struct PendingConflict {
        QString id;
        QString shortcut;
        quint64 generation = 0;
    };

    enum class CaptureBackend { None, X11, Wayland };
    enum class CapturePhase { Idle, Pending, Active };

    struct CaptureState {
        quint64 requestId = 0;
        quint64 generation = 0;
        QString id;
        int type = 0;
        QPointer<QQuickItem> item;
        CaptureBackend backend = CaptureBackend::None;
        CapturePhase phase = CapturePhase::Idle;

        void reset() { *this = {}; }
    };

    static QString shortcutKey(const QString &id, int type);
    ShortcutInfo *shortcutInfo(const QString &id, int type) const;
    quint64 shortcutGeneration(const QString &id, int type) const;
    bool isCurrentShortcutGeneration(const QString &id, int type, quint64 generation) const;
    void startRequestedX11Capture();
    void startWaylandKeyCapture(QQuickItem *item, const QString &id, int type,
                                quint64 requestId, quint64 generation);
    void stopWaylandKeyCapture();
    void handleCapturedKeystroke(const QString &id, int type, const QString &accels,
                                 quint64 generation);
    // 检查并处理desktop命令，如果desktop文件存在则添加dde-am前缀
    QString checkDesktopCmd(const QString &cmd);
    
    uint m_repeatInterval;
    uint m_repeatDelay;
    bool m_numLock;
    bool m_capsLock;
    KeyboardWorker *m_worker = nullptr;
    KeyboardModel *m_model = nullptr;
    ShortcutModel *m_shortcutModel = nullptr;
    QSortFilterProxyModel *m_layoutSearchModel = nullptr;
    QSortFilterProxyModel *m_shortcutSearchModel = nullptr;
    QString m_conflictText;
    TreelandShortcutCaptureManager *m_captureManager = nullptr;
    QPointer<TreelandShortcutCaptureSession> m_activeCapture;
    quint64 m_captureRequestSerial = 0;
    quint64 m_shortcutCommandRequestSerial = 0;
    quint64 m_customShortcutRequestSerial = 0;
    quint64 m_generationSerial = 0;
    quint64 m_pendingX11RequestId = 0;
    CaptureState m_capture;
    QHash<QString, quint64> m_shortcutGenerations;
    QHash<QString, PendingConflict> m_pendingConflicts;
};

}
#endif // KEYBOARDCONTROLLER_H
