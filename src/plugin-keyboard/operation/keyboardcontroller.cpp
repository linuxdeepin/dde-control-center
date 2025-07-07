//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "keyboardcontroller.h"
#include "dccfactory.h"
#include "layoutsmodel.h"

#include <PolkitQt1/Authority>

namespace dccV25 {
DCC_FACTORY_CLASS(KeyboardController)

KeyboardController::KeyboardController(QObject *parent)
    : QObject(parent)
{
    m_model = new KeyboardModel(this);
    m_worker = new KeyboardWorker(m_model, this);
    m_shortcutModel = new ShortcutModel(this);
    m_worker->setShortcutModel(m_shortcutModel);

    connect(m_model, &KeyboardModel::repeatIntervalChanged, this, &KeyboardController::repeatIntervalChanged);
    connect(m_model, &KeyboardModel::repeatDelayChanged, this, &KeyboardController::repeatDelayChanged);
    connect(m_model, &KeyboardModel::numLockChanged, this, &KeyboardController::numLockChanged);
    connect(m_model, &KeyboardModel::capsLockChanged, this, &KeyboardController::capsLockChanged);
    connect(m_model, &KeyboardModel::userLayoutChanged, this, &KeyboardController::layoutCountChanged);
    connect(m_model, &KeyboardModel::curLayoutChanged, this, &KeyboardController::currentLayoutChanged);
    connect(m_model, &KeyboardModel::keyboardEnabledChanged, this, &KeyboardController::keyboardEnabledChanged);

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
                if (current == nullptr || (conflict == current && conflict->accels == current->accels)) {
                    Q_EMIT requestRestore();
                    return;
                }

                auto conflictName = QString("<font color=\"red\">%1</font>").arg(conflict->name);
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
    connect(m_shortcutModel, &ShortcutModel::shortcutChanged, sourceModel, &ShortcutListModel::reset);
    connect(m_shortcutModel, &ShortcutModel::windowSwitchChanged, sourceModel, &ShortcutListModel::reset);

    m_shortcutSearchModel->setSourceModel(sourceModel);
    m_shortcutSearchModel->setFilterRole(ShortcutListModel::SearchedTextRole);
    m_shortcutSearchModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    return m_shortcutSearchModel;
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

void KeyboardController::addCustomShortcut(const QString &name, const QString &cmd, const QString &accels)
{
    // 添加时清理冲突快捷键
    if (auto conflict = m_shortcutModel->getInfo(accels)) {
        m_worker->onDisableShortcut(conflict);
    }

    m_worker->addCustomShortcut(name, cmd, accels);
}

void KeyboardController::modifyCustomShortcut(const QString &id, const QString &name, const QString &cmd, const QString &accels)
{
    ShortcutInfo *shortcut = m_shortcutModel->findInfoIf([id](ShortcutInfo *info){ return id == info->id; });
    if (!shortcut) {
        qWarning() << "shortcut not found..." << id << name;
        return;
    }

    // 修改时清理冲突快捷键
    if (auto conflict = m_shortcutModel->getInfo(accels)) {
        m_worker->onDisableShortcut(conflict);
    }

    shortcut->name = name;
    shortcut->command = cmd;
    shortcut->accels = accels;

    m_worker->modifyCustomShortcut(shortcut);
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
        // 修改时清理冲突快捷键
        if (auto conflict = m_shortcutModel->getInfo(accels)) {
            m_worker->onDisableShortcut(conflict);
            shortcut->accels = accels;
        }
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

}

#include "keyboardcontroller.moc"
