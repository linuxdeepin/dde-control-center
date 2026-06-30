//SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "keyboardcontroller.h"
#include "dccfactory.h"
#include "layoutsmodel.h"
#include "treelandshortcutcapture.h"

#include <PolkitQt1/Authority>
#include <QFileInfo>
#include <QGuiApplication>
#include <QKeySequence>
#include <QQuickItem>
#include <QQuickWindow>

namespace dccV25 {
DCC_FACTORY_CLASS(KeyboardController)

KeyboardController::KeyboardController(QObject *parent)
    : QObject(parent)
{
    m_model = new KeyboardModel(this);
    m_worker = new KeyboardWorker(m_model, this);
    m_shortcutModel = new ShortcutModel(this);
    m_worker->setShortcutModel(m_shortcutModel);

    // Construct the Wayland capture manager eagerly so the global is bound
    // before the user can click an edit row — otherwise the first
    // captureNext() loses the bind race and silently fails.
    // Use the worker's session-based detection (env: XDG_SESSION_TYPE /
    // WAYLAND_DISPLAY) rather than QGuiApplication::platformName(), so this
    // matches the proxy that decides which D-Bus API to call. On a divergent
    // setup (XWayland / forced -platform xcb) the manager simply won't bind and
    // captureNext() returns null → beginWaylandKeyCapture reports a visible
    // failure instead of the two halves disagreeing.
    if (m_worker->isWayland()) {
        m_captureManager = new TreelandShortcutCaptureManager(this);
    }

    connect(m_model, &KeyboardModel::repeatIntervalChanged, this, &KeyboardController::repeatIntervalChanged);
    connect(m_model, &KeyboardModel::repeatDelayChanged, this, &KeyboardController::repeatDelayChanged);
    connect(m_model, &KeyboardModel::numLockChanged, this, &KeyboardController::numLockChanged);
    connect(m_model, &KeyboardModel::capsLockChanged, this, &KeyboardController::capsLockChanged);
    connect(m_model, &KeyboardModel::userLayoutChanged, this, &KeyboardController::layoutCountChanged);
    connect(m_model, &KeyboardModel::curLayoutChanged, this, &KeyboardController::currentLayoutChanged);
    connect(m_model, &KeyboardModel::keyboardEnabledChanged, this, &KeyboardController::keyboardEnabledChanged);
    connect(m_worker, &KeyboardWorker::shortcutCommandReady,
            this, &KeyboardController::shortcutCommandReady);

    connect(m_shortcutModel, &ShortcutModel::keyEvent, this, [this](bool press, const QString &shortcut){
        ShortcutInfo *current = m_shortcutModel->currentInfo();

        ShortcutInfo *conflict = m_shortcutModel->getInfo(shortcut);

        if (!press) {
            if (shortcut.isEmpty()) {
                Q_EMIT requestRestore();
                return;
            }

            if (shortcut == "BackSpace" || shortcut == "Delete") {
                Q_EMIT requestClear();
                return;
            }

            // have conflict
            if (conflict) {
                // self conflict
                if (conflict == current && conflict->accels == current->accels) {
                    Q_EMIT requestRestore();
                    return;
                }

                auto conflictName = QString("<font color=\"red\">%1</font>").arg(conflict->name.toHtmlEscaped());
                QString text = KeyboardController::tr("This shortcut conflicts with [%1]").arg(conflictName);
                setConflictText(text);
                Q_EMIT keyConflicted(current ? current->accels : "", conflict->accels);
            } else {
                // save
                if (current) {
                    current->accels = shortcut;
                }

                Q_EMIT keyDone(shortcut);
            }
        }

        Q_EMIT keyEvent(shortcut);
    });

    QMetaObject::invokeMethod(m_worker, "active", Qt::QueuedConnection);
}

uint KeyboardController::repeatInterval() const
{
    return m_model->repeatInterval();
}

void KeyboardController::setRepeatInterval(uint newRepeatInterval)
{
    if (repeatInterval() == newRepeatInterval)
        return;

    m_worker->setRepeatInterval(newRepeatInterval);
}

uint KeyboardController::repeatDelay() const
{
    return m_model->repeatDelay();
}

void KeyboardController::setRepeatDelay(uint newRepeatDelay)
{
    if (repeatDelay() == newRepeatDelay)
        return;

    m_worker->setRepeatDelay(newRepeatDelay);
}

bool KeyboardController::keyboardEnabled() const
{
    return m_model->keyboardEnabled();
}

void KeyboardController::setKeyboardEnabled(bool enabled)
{
    if (keyboardEnabled() == enabled)
        return;

    connect(PolkitQt1::Authority::instance(), &PolkitQt1::Authority::checkAuthorizationFinished, this, [this, enabled](PolkitQt1::Authority::Result authenticationResult) {
        disconnect(PolkitQt1::Authority::instance(), nullptr, this, nullptr);
        if (PolkitQt1::Authority::Result::Yes == authenticationResult) {
            m_worker->setKeyboardEnabled(enabled);
        } else {
            Q_EMIT keyboardEnabledChanged();
        }
    });
    PolkitQt1::Authority::instance()->checkAuthorization("org.deepin.dde.accounts.user-administration", PolkitQt1::UnixProcessSubject(getpid()), PolkitQt1::Authority::AllowUserInteraction);
}

bool KeyboardController::numLock() const
{
    return m_model->numLock();
}

void KeyboardController::setNumLock(bool newNumLock)
{
    if (numLock() == newNumLock)
        return;

    m_worker->setNumLock(newNumLock);
}

bool KeyboardController::capsLock() const
{
    return m_model->capsLock();
}

void KeyboardController::setCapsLock(bool newCapsLock)
{
    if (capsLock() == newCapsLock)
        return;

    m_worker->setCapsLock(newCapsLock);
}

QMap<QString, QString> KeyboardController::userLayouts() const
{
    return m_model->userLayout();
}

QString KeyboardController::userLayoutAt(int index, bool isValue/* = true*/) const
{
    const QStringList &list = m_model->getUserLayoutList();
    const QString &key = list.value(index);
    if (!isValue)
        return key;

    return userLayouts().value(key);
}

bool KeyboardController::userLayoutsContains(const QString &key)
{
    return userLayouts().contains(key);
}

QSortFilterProxyModel *KeyboardController::layoutSearchModel()
{
    if (m_layoutSearchModel)
        return m_layoutSearchModel;

    m_layoutSearchModel = new LayoutFilterModel(this);

    m_worker->onPinyin();

    connect(m_model, &KeyboardModel::userLayoutChanged, m_layoutSearchModel,
            [this](const QString &, const QString &){
        m_layoutSearchModel->invalidate();
    });

    auto sourceModel = new LayoutsModel(m_worker);

    m_layoutSearchModel->setSourceModel(sourceModel);
    m_layoutSearchModel->setFilterRole(LayoutsModel::SearchedTextRole);
    m_layoutSearchModel->setSortRole(LayoutsModel::SectionRole);
    m_layoutSearchModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    return m_layoutSearchModel;
}

QSortFilterProxyModel *KeyboardController::shortcutSearchModel()
{
    if (m_shortcutSearchModel)
        return m_shortcutSearchModel;

    m_shortcutSearchModel = new QSortFilterProxyModel(this);

    auto sourceModel = new ShortcutListModel(this);
    sourceModel->setSouceModel(m_shortcutModel);

    connect(m_shortcutModel, &ShortcutModel::delCustomInfo, sourceModel, &ShortcutListModel::reset);
    connect(m_shortcutModel, &ShortcutModel::addCustomInfo, sourceModel, &ShortcutListModel::reset);
    connect(m_shortcutModel, &ShortcutModel::shortcutChanged, sourceModel, &ShortcutListModel::onUpdateShortcut);
    connect(m_shortcutModel, &ShortcutModel::windowSwitchChanged, sourceModel, &ShortcutListModel::reset);
    // Wayland: re-layout rows when category ordering metadata arrives.
    connect(m_shortcutModel, &ShortcutModel::categoryMetaChanged, sourceModel, &ShortcutListModel::reset);

    m_shortcutSearchModel->setSourceModel(sourceModel);
    m_shortcutSearchModel->setFilterRole(ShortcutListModel::SearchedTextRole);
    m_shortcutSearchModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    return m_shortcutSearchModel;
}

QString KeyboardController::sectionDisplayName(const QString &key) const
{
    return m_shortcutModel ? m_shortcutModel->sectionDisplayName(key) : key;
}

QString KeyboardController::customCategoryKey() const
{
    return m_shortcutModel ? m_shortcutModel->customCategoryKey() : QString();
}

void KeyboardController::updateKey(const QString &id, const int &type)
{
    ShortcutInfo *shortcut = nullptr;
    if (!id.isEmpty()) { // new shortcuts
        shortcut = m_shortcutModel->findInfoIf([id, type](ShortcutInfo *info){
            return id == info->id && type == info->type;
        });
        if (!shortcut) {
            qWarning() << "shortcut not found..." << id << type;
            return;
        }
    }

    m_worker->updateKey(shortcut);
}

QStringList KeyboardController::formatKeys(const QString &shortcuts)
{
    return ShortcutModel::formatKeys(shortcuts);
}

QString KeyboardController::keyEventToAccels(int key, int modifiers)
{
    return QKeySequence(key | modifiers).toString(QKeySequence::PortableText);
}

QString KeyboardController::checkDesktopCmd(const QString &cmd)
{
    // Check and process desktop commands, add dde-am prefix if desktop file exists
    if (!cmd.isEmpty() && cmd.startsWith("/") && cmd.endsWith(".desktop")) {
        QFileInfo fileInfo(cmd);
        if (fileInfo.exists() && fileInfo.isFile()) {
            return "dde-am " + cmd;
        }
    }
    return cmd;
}

void KeyboardController::addCustomShortcut(const QString &name, const QString &cmd, const QString &accels)
{
    if (m_worker->isWayland()) {
        m_worker->addCustomShortcut(name, checkDesktopCmd(cmd), accels);
        return;
    }

    // Clear conflicting shortcuts when adding
    if (auto conflict = m_shortcutModel->getInfo(accels)) {
        m_worker->onDisableShortcut(conflict);
    }

    // Check and process desktop commands, add dde-am prefix if needed
    QString newCmd = checkDesktopCmd(cmd);
    m_worker->addCustomShortcut(name, newCmd, accels);
}

void KeyboardController::modifyCustomShortcut(const QString &id, const QString &name, const QString &cmd, const QString &accels)
{
    ShortcutInfo *shortcut = m_shortcutModel->findInfoIf([id](ShortcutInfo *info){ return id == info->id; });
    if (!shortcut) {
        qWarning() << "shortcut not found..." << id << name;
        return;
    }

    if (m_worker->isWayland()) {
        ShortcutInfo updated = *shortcut;
        updated.name = name;
        updated.command = checkDesktopCmd(cmd);
        updated.accels = accels;
        m_worker->modifyCustomShortcut(&updated);
        return;
    }

    // Clear conflicting shortcuts when modifying
    if (auto conflict = m_shortcutModel->getInfo(accels)) {
        m_worker->onDisableShortcut(conflict);
    }

    shortcut->name = name;
    shortcut->command = checkDesktopCmd(cmd);
    shortcut->accels = accels;

    m_worker->modifyCustomShortcut(shortcut);
}

void KeyboardController::requestShortcutCommand(const QString &id)
{
    if (m_worker)
        m_worker->requestShortcutCommand(id);
}

void KeyboardController::deleteCustomShortcut(const QString &id)
{
    ShortcutInfo *shortcut = m_shortcutModel->findInfoIf([id](ShortcutInfo *info){ return id == info->id; });
    if (!shortcut) {
        qWarning() << "shortcut not found..." << id;
        return;
    }

    m_worker->delShortcut(shortcut);
}

void KeyboardController::modifyShortcut(const QString &id, const QString &accels, const int &type)
{
    ShortcutInfo *shortcut = m_shortcutModel->findInfoIf([id, type](ShortcutInfo *info){ return id == info->id && type == info->type; });
    if (!shortcut) {
        qWarning() << "shortcut not found..." << id << type;
        return;
    }

    if (shortcut->accels != accels) {
        // Clear conflicting shortcuts when modifying.
        // X11 only: onDisableShortcut does a blocking waitForFinished(). On
        // Wayland that maps to an async Disable + commitAndWait that would
        // freeze the GUI thread, and it's redundant — the Wayland path in
        // modifyShortcutEditAux resolves the conflict server-side
        // (LookupConflictShortcut → Disable → ModifyHotkeys), and dde-services'
        // ModifyHotkeys already unbinds the conflicting owner atomically.
        if (auto conflict = m_shortcutModel->getInfo(accels)) {
            if (!m_worker->isWayland())
                m_worker->onDisableShortcut(conflict);
        }
        // Always update the target's accels so downstream modifyShortcutEditAux
        // operates on the new value. The previous "only on conflict" placement
        // left a stale accels when there was no conflict (e.g. Wayland path
        // where the keyEvent lambda's pre-update doesn't run).
        shortcut->accels = accels;
    }

    m_worker->modifyShortcutEdit(shortcut);
}

void KeyboardController::clearShortcut(const QString &id, const int &type)
{
    ShortcutInfo *shortcut = m_shortcutModel->findInfoIf([id, type](ShortcutInfo *info){ return id == info->id && type == info->type; });
    if (!shortcut) {
        qWarning() << "shortcut not found..." << id << type;
        return;
    }

    m_worker->onDisableShortcut(shortcut);
}

void KeyboardController::resetAllShortcuts()
{
    m_worker->resetAll();
}

void KeyboardController::addUserLayout(const QString &layout)
{
    m_worker->addUserLayout(layout);
}

void KeyboardController::deleteUserLayout(const QString &layout)
{
    m_worker->delUserLayout(layout);
}

int KeyboardController::layoutCount() const
{
    return  userLayouts().count();
}

QString KeyboardController::currentLayout() const
{
    return m_model->curLayout();
}

void KeyboardController::setCurrentLayout(const QString &key)
{
    if (currentLayout() == key)
        return;

    m_worker->setLayout(key);
}

QString KeyboardController::conflictText() const
{
    return m_conflictText;
}

void KeyboardController::setConflictText(const QString &newConflictText)
{
    if (m_conflictText == newConflictText)
        return;

    m_conflictText = newConflictText;
    emit conflictTextChanged();
}

bool KeyboardController::isCustomShortcutNameExists(const QString &name, const QString &excludeId)
{
    if (name.trimmed().isEmpty())
        return false;
        
    const auto customInfos = m_shortcutModel->customInfo();
    for (const auto *info : customInfos) {
        // Exclude the current editing shortcut (if in edit mode)
        if (!excludeId.isEmpty() && info->id == excludeId)
            continue;
            
        if (info->name.trimmed() == name.trimmed()) {
            return true;
        }
    }
    return false;
}

bool KeyboardController::isSystemShortcutNameExists(const QString &name)
{
    if (!m_shortcutModel) {
        return false;
    }
    
    return m_shortcutModel->containsSystemShortcutName(name);
}

bool KeyboardController::isShortcutNameExists(const QString &name, const QString &excludeId)
{
    if (name.trimmed().isEmpty()) {
        return false;
    }
    
    // Check system shortcut conflicts first
    if (isSystemShortcutNameExists(name)) {
        return true;
    }
    
    // Then check custom shortcut conflicts
    return isCustomShortcutNameExists(name, excludeId);
}

void KeyboardController::refreshKeyboard()
{
    if (m_worker) {
        m_worker->refreshKeyboard();
    }
}

// Convert a Qt PortableText accel string ("Ctrl+Alt+T", "Meta+P") to the
// X11/XKB form used internally by the shortcut model and the legacy wire
// format ("<Control><Alt>T", "<Super>P"). Only handles modifier+key combos —
// modifiable shortcuts are conventional combos, not media keys.
static QString qtAccelToXkb(const QString &accel)
{
    if (accel.isEmpty())
        return accel;
    // PortableText joins modifiers and key with '+', so a literal '+' key
    // ("Ctrl++", or "+" on its own) collapses under SkipEmptyParts and the key
    // is lost. Split keeping empties and fold the trailing empty tokens that
    // the separator produced back into a single "+" key token.
    QStringList parts = accel.split('+', Qt::KeepEmptyParts);
    if (parts.size() >= 2 && parts.last().isEmpty()) {
        while (!parts.isEmpty() && parts.last().isEmpty())
            parts.removeLast();
        parts.append(QStringLiteral("+"));
    } else {
        parts.removeAll(QString()); // drop stray empties from odd input
    }
    if (parts.size() <= 1)
        return accel; // standalone key, no modifiers
    QString out;
    for (int i = 0; i < parts.size() - 1; ++i) {
        const QString &p = parts.at(i);
        if (p == QLatin1String("Ctrl") || p == QLatin1String("Control"))
            out += QStringLiteral("<Control>");
        else if (p == QLatin1String("Shift"))
            out += QStringLiteral("<Shift>");
        else if (p == QLatin1String("Alt"))
            out += QStringLiteral("<Alt>");
        else if (p == QLatin1String("Meta") || p == QLatin1String("Super"))
            out += QStringLiteral("<Super>");
        else
            out += QLatin1Char('<') + p + QLatin1Char('>');
    }
    out += parts.last();
    return out;
}

void KeyboardController::beginWaylandKeyCapture(QQuickItem *item, const QString &id, int type)
{
    // Silent no-op on non-Wayland — the X11 path captures via QML Keys.onPressed.
    // Session-based detection (same source as the proxy / capture manager guard).
    if (!m_worker->isWayland()) {
        return;
    }
    if (!item) {
        Q_EMIT waylandKeyCaptureFailed(id, type, 0);
        return;
    }
    QQuickWindow *window = item->window();
    if (!window) {
        Q_EMIT waylandKeyCaptureFailed(id, type, 0);
        return;
    }

    if (!m_captureManager) {
        // Cold path: KeyboardController was constructed before the platform
        // resolved to wayland (shouldn't happen normally). Build lazily.
        m_captureManager = new TreelandShortcutCaptureManager(this);
    }

    if (m_activeCapture) {
        // The user is restarting edit. Disconnect first so any wayland event
        // still in flight for the old session cannot land in this controller
        // and modify the wrong row; then schedule the session for delete
        // (its destructor cancels the wayland capture).
        disconnect(m_activeCapture, nullptr, this, nullptr);
        m_activeCapture->deleteLater();
        m_activeCapture.clear();
    }

    TreelandShortcutCaptureSession *session = m_captureManager->captureNext(window);
    if (!session) {
        Q_EMIT waylandKeyCaptureFailed(id, type, 0);
        return;
    }
    m_activeCapture = session;

    connect(session, &TreelandShortcutCaptureSession::captured, this,
            [this, id, type](const QString &key) {
                submitWaylandKeystroke(id, type, key);
                Q_EMIT waylandKeyCaptureFinished(id, type, key);
            });
    connect(session, &TreelandShortcutCaptureSession::failed, this,
            [this, id, type](uint32_t reason) {
                Q_EMIT waylandKeyCaptureFailed(id, type, static_cast<int>(reason));
            });
}

void KeyboardController::endWaylandKeyCapture()
{
    if (!m_activeCapture)
        return;

    // Disconnect first so any pending wayland event cannot deliver a
    // captured() signal after we've already moved on (Replace / Cancel).
    disconnect(m_activeCapture, nullptr, this, nullptr);
    m_activeCapture->deleteLater();
    m_activeCapture.clear();
}

void KeyboardController::submitWaylandKeystroke(const QString &id, int type, const QString &accels)
{
    if (accels.isEmpty()) {
        Q_EMIT requestRestore();
        return;
    }
    if (accels == QLatin1String("Backspace")
        || accels == QLatin1String("Delete")
        || accels == QLatin1String("Del")) {
        Q_EMIT requestClear();
        return;
    }

    ShortcutInfo *current = m_shortcutModel->findInfoIf([&](ShortcutInfo *info) {
        return info->id == id && info->type == type;
    });
    if (current)
        m_shortcutModel->setCurrentInfo(current);

    const QString xkb = qtAccelToXkb(accels);

    ShortcutInfo *conflict = m_shortcutModel->getInfo(xkb);

    if (conflict) {
        // Self-conflict (user pressed the same combo already bound to this row)
        if (conflict == current && conflict->accels == xkb) {
            Q_EMIT requestRestore();
            return;
        }
        auto conflictName = QString("<font color=\"red\">%1</font>").arg(conflict->name.toHtmlEscaped());
        const QString text = KeyboardController::tr("This shortcut conflicts with [%1]").arg(conflictName);
        setConflictText(text);
        Q_EMIT keyConflicted(current ? current->accels : QString(), xkb);
        // NOTE: deliberately no early return here — fall through to keyEvent
        // below, mirroring the X11 keyEvent lambda which emits keyEvent
        // unconditionally after the conflict branch. QML onKeyEvent updates
        // editItem.keys, so the edit chip shows the combo the user just pressed;
        // without it the conflict text appears but the chip keeps the old combo.
    } else {
        if (current)
            current->accels = xkb;
        Q_EMIT keyDone(xkb);
    }

    Q_EMIT keyEvent(xkb);
}

}

#include "keyboardcontroller.moc"
