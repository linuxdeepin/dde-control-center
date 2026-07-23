//SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "keyboardcontroller.h"
#include "dccfactory.h"
#include "layoutsmodel.h"
#include "treelandshortcutcapture.h"

#include <PolkitQt1/Authority>
#include <QFileInfo>
#include <QQuickItem>
#include <QQuickWindow>
#include <QTimer>

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
    // captureNext() returns null and reports a visible
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
    connect(m_worker, &KeyboardWorker::captureRequestFinished, this,
            [this](quint64 requestId, bool success, const QString &reason) {
                if (requestId != m_pendingX11RequestId)
                    return;

                m_pendingX11RequestId = 0;
                if (requestId != m_capture.requestId
                        || m_capture.backend != CaptureBackend::X11) {
                    if (success)
                        m_worker->endCapture();
                    startRequestedX11Capture();
                    return;
                }

                const QString id = m_capture.id;
                const int type = m_capture.type;
                const quint64 generation = m_capture.generation;
                if (!isCurrentShortcutGeneration(id, type, generation)) {
                    if (success)
                        m_worker->endCapture();
                    m_capture.reset();
                    return;
                }

                if (!success) {
                    m_capture.reset();
                    qWarning() << "BeginCapture failed:" << reason;
                    Q_EMIT keyCaptureFailed(id, type, reason);
                    return;
                }

                if (!m_capture.item) {
                    m_capture.reset();
                    m_worker->endCapture();
                    return;
                }

                m_capture.phase = CapturePhase::Active;
                Q_EMIT keyCaptureStarted(id, type);
            });
    connect(m_worker, &KeyboardWorker::shortcutConflictDetected, this,
            [this](const QString &id, int type, const QString &shortcut,
                   const QString &conflictId, const QString &conflictName,
                   bool replaceable, quint64 generation) {
                if (!isCurrentShortcutGeneration(id, type, generation))
                    return;

                ShortcutInfo *current = shortcutInfo(id, type);
                if (current)
                    m_shortcutModel->setCurrentInfo(current);
                if (replaceable) {
                    m_pendingConflicts.insert(shortcutKey(id, type),
                                              PendingConflict{conflictId, shortcut, generation});
                } else {
                    m_pendingConflicts.remove(shortcutKey(id, type));
                }

                const QString escapedName = conflictName.toHtmlEscaped();
                const QString markedName = QStringLiteral("<font color=\"red\">%1</font>").arg(escapedName);
                const QString message = KeyboardController::tr("This shortcut conflicts with [%1]").arg(markedName);
                setConflictText(message);
                Q_EMIT keyConflicted(id, type, current ? current->accels : QString(),
                                     shortcut, message, replaceable);
            });
    connect(m_worker, &KeyboardWorker::shortcutModificationFinished,
            this, [this](const QString &id, int type, const QString &shortcut,
                         bool success, quint64 generation) {
                if (!isCurrentShortcutGeneration(id, type, generation))
                    return;
                if (!success)
                    m_worker->onShortcutChanged(id, type);
                Q_EMIT shortcutModificationFinished(id, type, shortcut, success);
            });
    connect(m_worker, &KeyboardWorker::customShortcutOperationFinished,
            this, &KeyboardController::customShortcutOperationFinished);

    connect(m_shortcutModel, &ShortcutModel::keyEvent, this, [this](bool press, const QString &shortcut){
        if (m_capture.backend != CaptureBackend::X11
                || m_capture.phase != CapturePhase::Active)
            return;

        const QString id = m_capture.id;
        const int type = m_capture.type;
        const quint64 generation = m_capture.generation;
        if (!isCurrentShortcutGeneration(id, type, generation))
            return;

        if (press)
            Q_EMIT keyEvent(id, type, shortcut);
        else {
            endKeyCapture();
            handleCapturedKeystroke(id, type, shortcut, generation);
        }
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
    ShortcutInfo *shortcut = shortcutInfo(id, type);
    if (!id.isEmpty() && !shortcut) {
        qWarning() << "shortcut not found..." << id << type;
        return;
    }

    m_worker->updateKey(shortcut);
}

QStringList KeyboardController::formatKeys(const QString &shortcuts)
{
    return ShortcutModel::formatKeys(shortcuts);
}

bool KeyboardController::isWaylandSession() const
{
    return m_worker->isWayland();
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

quint64 KeyboardController::addCustomShortcut(const QString &name, const QString &cmd,
                                              const QString &accels)
{
    const quint64 requestId = ++m_customShortcutRequestSerial;
    m_pendingConflicts.remove(shortcutKey(QString(), ShortcutModel::Custom));
    m_worker->addCustomShortcut(name, checkDesktopCmd(cmd), accels, requestId);
    return requestId;
}

quint64 KeyboardController::modifyCustomShortcut(const QString &id, const QString &name,
                                                 const QString &cmd, const QString &accels)
{
    const quint64 requestId = ++m_customShortcutRequestSerial;
    ShortcutInfo *shortcut = m_shortcutModel->findInfoIf([id](ShortcutInfo *info){ return id == info->id; });
    if (!shortcut) {
        qWarning() << "shortcut not found..." << id << name;
        QTimer::singleShot(0, this, [this, requestId] {
            Q_EMIT customShortcutOperationFinished(requestId, false,
                                                   tr("The shortcut no longer exists."));
        });
        return requestId;
    }

    ShortcutInfo updated = *shortcut;
    updated.name = name;
    updated.command = checkDesktopCmd(cmd);
    updated.accels = accels;
    m_pendingConflicts.remove(shortcutKey(id, ShortcutModel::Custom));
    m_worker->modifyCustomShortcut(&updated, requestId);
    return requestId;
}

quint64 KeyboardController::replaceCustomShortcut(const QString &id, const QString &name,
                                                  const QString &cmd, const QString &accels)
{
    const quint64 requestId = ++m_customShortcutRequestSerial;
    const int type = ShortcutModel::Custom;
    if (!id.isEmpty() && !shortcutInfo(id, type)) {
        qWarning() << "custom shortcut not found..." << id << name;
        QTimer::singleShot(0, this, [this, requestId] {
            Q_EMIT customShortcutOperationFinished(requestId, false,
                                                   tr("The shortcut no longer exists."));
        });
        return requestId;
    }

    const quint64 generation = shortcutGeneration(id, type);
    if (!isCurrentShortcutGeneration(id, type, generation)) {
        qWarning() << "ignore custom shortcut replacement outside current capture:" << id;
        QTimer::singleShot(0, this, [this, requestId] {
            Q_EMIT customShortcutOperationFinished(requestId, false,
                                                   tr("The shortcut conflict is no longer current."));
        });
        return requestId;
    }

    const PendingConflict pendingConflict = m_pendingConflicts.take(shortcutKey(id, type));
    if (pendingConflict.id.isEmpty()
            || pendingConflict.shortcut != accels
            || pendingConflict.generation != generation) {
        qWarning() << "replace custom shortcut requested without a confirmed conflict:" << id;
        QTimer::singleShot(0, this, [this, requestId] {
            Q_EMIT customShortcutOperationFinished(requestId, false,
                                                   tr("Please confirm the shortcut conflict again."));
        });
        return requestId;
    }

    m_worker->replaceCustomShortcut(id, name, checkDesktopCmd(cmd), accels,
                                    pendingConflict.id, generation, requestId);
    return requestId;
}

void KeyboardController::clearPendingConflict(const QString &id, int type)
{
    m_pendingConflicts.remove(shortcutKey(id, type));
}

quint64 KeyboardController::requestShortcutCommand(const QString &id)
{
    if (!m_worker)
        return 0;

    if (++m_shortcutCommandRequestSerial == 0)
        ++m_shortcutCommandRequestSerial;
    m_worker->requestShortcutCommand(id, m_shortcutCommandRequestSerial);
    return m_shortcutCommandRequestSerial;
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
    ShortcutInfo *shortcut = shortcutInfo(id, type);
    if (!shortcut) {
        qWarning() << "shortcut not found..." << id << type;
        return;
    }

    ShortcutInfo updated = *shortcut;
    updated.accels = accels;
    const quint64 generation = shortcutGeneration(id, type);
    if (!isCurrentShortcutGeneration(id, type, generation)) {
        qWarning() << "ignore shortcut modification outside current capture:" << id << type;
        return;
    }
    m_pendingConflicts.remove(shortcutKey(id, type));
    m_worker->modifyShortcutEdit(&updated, generation);
}

void KeyboardController::replaceShortcut(const QString &id, const QString &accels, const int &type)
{
    ShortcutInfo *shortcut = shortcutInfo(id, type);
    if (!shortcut) {
        qWarning() << "shortcut not found..." << id << type;
        return;
    }

    ShortcutInfo updated = *shortcut;
    updated.accels = accels;
    const quint64 generation = shortcutGeneration(id, type);
    if (!isCurrentShortcutGeneration(id, type, generation)) {
        qWarning() << "ignore shortcut replacement outside current capture:" << id << type;
        return;
    }

    const PendingConflict pendingConflict = m_pendingConflicts.take(shortcutKey(id, type));
    if (pendingConflict.id.isEmpty()
            || pendingConflict.shortcut != accels
            || pendingConflict.generation != generation) {
        qWarning() << "replace shortcut requested without a confirmed conflict:" << id;
        return;
    }
    m_worker->replaceShortcutEdit(&updated, pendingConflict.id, generation);
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

ShortcutInfo *KeyboardController::shortcutInfo(const QString &id, int type) const
{
    if (id.isEmpty() || !m_shortcutModel)
        return nullptr;

    return m_shortcutModel->findInfoIf([&](ShortcutInfo *info) {
        return info->id == id && static_cast<int>(info->type) == type;
    });
}

QString KeyboardController::shortcutKey(const QString &id, int type)
{
    return id + QChar(0x1f) + QString::number(type);
}

quint64 KeyboardController::shortcutGeneration(const QString &id, int type) const
{
    return m_shortcutGenerations.value(shortcutKey(id, type));
}

bool KeyboardController::isCurrentShortcutGeneration(const QString &id, int type,
                                                      quint64 generation) const
{
    const QString key = shortcutKey(id, type);
    return generation != 0
            && m_shortcutGenerations.value(key) == generation;
}

void KeyboardController::beginKeyCapture(QQuickItem *item, const QString &id, int type)
{
    endKeyCapture();

    ShortcutInfo *shortcut = shortcutInfo(id, type);
    if (!id.isEmpty() && !shortcut) {
        const QString reason = QStringLiteral("shortcut not found");
        qWarning() << reason << id << type;
        Q_EMIT keyCaptureFailed(id, type, reason);
        return;
    }
    m_worker->updateKey(shortcut);

    const QString key = shortcutKey(id, type);
    const quint64 generation = ++m_generationSerial;
    m_shortcutGenerations.insert(key, generation);
    m_worker->setShortcutGeneration(id, type, generation);
    m_pendingConflicts.remove(key);

    const quint64 requestId = ++m_captureRequestSerial;
    m_capture.requestId = requestId;
    m_capture.generation = generation;
    m_capture.id = id;
    m_capture.type = type;
    m_capture.item = item;
    m_capture.phase = CapturePhase::Pending;
    if (item) {
        connect(item, &QObject::destroyed, this, [this, requestId] {
            if (requestId == m_capture.requestId)
                endKeyCapture();
        });
    }

    if (m_worker->isWayland()) {
        m_capture.backend = CaptureBackend::Wayland;
        startWaylandKeyCapture(item, id, type, requestId, generation);
        return;
    }

    m_capture.backend = CaptureBackend::X11;
    startRequestedX11Capture();
}

void KeyboardController::startRequestedX11Capture()
{
    if (m_capture.backend != CaptureBackend::X11
            || m_capture.phase != CapturePhase::Pending
            || m_pendingX11RequestId != 0)
        return;

    m_pendingX11RequestId = m_capture.requestId;
    const quint64 requestId = m_capture.requestId;
    const QString id = m_capture.id;
    const int type = m_capture.type;
    const quint64 generation = m_capture.generation;
    QTimer::singleShot(30000, this, [this, requestId, id, type, generation] {
        const bool requestPending = requestId == m_pendingX11RequestId;
        const bool requestActive = requestId == m_capture.requestId
                && m_capture.backend == CaptureBackend::X11
                && m_capture.phase == CapturePhase::Active;
        if ((!requestPending && !requestActive)
                || !isCurrentShortcutGeneration(id, type, generation)) {
            return;
        }

        endKeyCapture();
        Q_EMIT keyCaptureFailed(id, type, QStringLiteral("shortcut capture timed out"));
    });
    m_worker->beginCapture(m_pendingX11RequestId);
}

void KeyboardController::startWaylandKeyCapture(QQuickItem *item, const QString &id, int type,
                                                quint64 requestId, quint64 generation)
{
    const QString failureReason = QStringLiteral("unable to start compositor shortcut capture");
    if (!item || !item->window()) {
        m_capture.reset();
        Q_EMIT keyCaptureFailed(id, type, failureReason);
        Q_EMIT waylandKeyCaptureFailed(id, type, 0);
        return;
    }

    if (!m_captureManager)
        m_captureManager = new TreelandShortcutCaptureManager(this);

    TreelandShortcutCaptureSession *session = m_captureManager->captureNext(item->window());
    if (!session) {
        m_capture.reset();
        Q_EMIT keyCaptureFailed(id, type, failureReason);
        Q_EMIT waylandKeyCaptureFailed(id, type, 0);
        return;
    }

    m_activeCapture = session;
    m_capture.phase = CapturePhase::Active;
    if (!isCurrentShortcutGeneration(id, type, generation))
        return;
    Q_EMIT keyCaptureStarted(id, type);

    connect(session, &TreelandShortcutCaptureSession::captured, this,
            [this, id, type, requestId, generation](const QString &key) {
                if (requestId != m_capture.requestId
                        || m_capture.backend != CaptureBackend::Wayland
                        || m_capture.phase != CapturePhase::Active
                        || !isCurrentShortcutGeneration(id, type, generation)) {
                    return;
                }
                Q_EMIT waylandKeyCaptureFinished(id, type, key);
                submitCapturedKeystroke(id, type, key);
            });
    connect(session, &TreelandShortcutCaptureSession::failed, this,
            [this, id, type, requestId, generation](uint32_t reason) {
                if (requestId != m_capture.requestId
                        || m_capture.backend != CaptureBackend::Wayland
                        || m_capture.phase != CapturePhase::Active
                        || !isCurrentShortcutGeneration(id, type, generation)) {
                    return;
                }
                endKeyCapture();
                const QString message = QStringLiteral("compositor capture failed: %1").arg(reason);
                Q_EMIT keyCaptureFailed(id, type, message);
                Q_EMIT waylandKeyCaptureFailed(id, type, static_cast<int>(reason));
            });
}

void KeyboardController::stopWaylandKeyCapture()
{
    if (!m_activeCapture)
        return;

    disconnect(m_activeCapture, nullptr, this, nullptr);
    m_activeCapture->deleteLater();
    m_activeCapture.clear();
}

void KeyboardController::endKeyCapture()
{
    ++m_captureRequestSerial;
    const bool x11Capture = m_capture.backend == CaptureBackend::X11;
    m_capture.reset();

    if (x11Capture || m_pendingX11RequestId != 0)
        m_worker->endCapture();
    stopWaylandKeyCapture();
}

void KeyboardController::submitCapturedKeystroke(const QString &id, int type, const QString &accels)
{
    if (m_capture.phase != CapturePhase::Active
            || id != m_capture.id || type != m_capture.type)
        return;

    const quint64 generation = m_capture.generation;
    if (!isCurrentShortcutGeneration(id, type, generation))
        return;

    endKeyCapture();
    handleCapturedKeystroke(id, type, accels, generation);
}

void KeyboardController::handleCapturedKeystroke(const QString &id, int type, const QString &accels,
                                                 quint64 generation)
{
    if (!isCurrentShortcutGeneration(id, type, generation))
        return;

    if (accels.isEmpty()) {
        Q_EMIT requestRestore(id, type);
        return;
    }
    if (accels == QLatin1String("Esc") || accels == QLatin1String("Escape")) {
        Q_EMIT requestRestore(id, type);
        return;
    }
    if (accels == QLatin1String("BackSpace")
        || accels == QLatin1String("Backspace")
        || accels == QLatin1String("Delete")
        || accels == QLatin1String("Del")) {
        Q_EMIT requestClear(id, type);
        return;
    }

    ShortcutInfo *current = shortcutInfo(id, type);
    if (current)
        m_shortcutModel->setCurrentInfo(current);

    const QString xkb = qtAccelToXkb(accels);
    ShortcutInfo *conflict = m_shortcutModel->getInfo(xkb);
    if (conflict) {
        if (conflict == current && conflict->accels == xkb) {
            Q_EMIT requestRestore(id, type);
            return;
        }

        const QString conflictName = QStringLiteral("<font color=\"red\">%1</font>")
                .arg(conflict->name.toHtmlEscaped());
        m_pendingConflicts.insert(shortcutKey(id, type),
                                  PendingConflict{conflict->id, xkb, generation});
        const QString message = KeyboardController::tr("This shortcut conflicts with [%1]").arg(conflictName);
        setConflictText(message);
        Q_EMIT keyConflicted(id, type, current ? current->accels : QString(),
                             xkb, message, true);
    } else {
        Q_EMIT keyDone(id, type, xkb);
    }

    Q_EMIT keyEvent(id, type, xkb);
}

}

#include "keyboardcontroller.moc"
