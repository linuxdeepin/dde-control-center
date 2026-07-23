//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "keyboardwork.h"

#include "dcclocale.h"
#include <DGuiApplicationHelper>

#include <QTimer>
#include <QDebug>
#include <QLoggingCategory>
#include <QLocale>
#include <QCollator>
#include <QCoreApplication>
#include <QDBusArgument>
#include <QDBusError>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDBusInterface>
#include <QDBusPendingCallWatcher>
#include <QTranslator>

using namespace DTK_NAMESPACE::Core;
using namespace dccV25;
bool caseInsensitiveLessThan(const MetaData &s1, const MetaData &s2);

Q_LOGGING_CATEGORY(lcKeyboardWorker, "dde.dcc.keyboard.worker")

namespace {

QString customShortcutErrorMessage(const QDBusError &error)
{
    switch (error.type()) {
    case QDBusError::ServiceUnknown:
    case QDBusError::NoReply:
    case QDBusError::Timeout:
    case QDBusError::Disconnected:
        return QCoreApplication::translate(
                "KeyboardWorker", "The shortcut service is unavailable. Please try again.");
    default:
        return QCoreApplication::translate(
                "KeyboardWorker", "Failed to save the shortcut. Please try again.");
    }
}

QString canonicalShortcut(const QString &shortcut)
{
    // Store one logical Delete binding. dde-services expands it to both
    // Delete and KP_Delete when registering shortcuts in the X11 backend.
    return QString(shortcut).replace(QStringLiteral("KP_Delete"), QStringLiteral("Delete"));
}

} // namespace

const QMap<QString, QString> &ModelKeycode = {{"minus", "-"}, {"equal", "="}, {"backslash", "\\"}, {"question", "?/"}, {"exclam", "1"}, {"numbersign", "3"},
    {"semicolon", ";"}, {"apostrophe", "'"}, {"less", ",<"}, {"period", ">."}, {"slash", "?/"}, {"parenleft", "9"}, {"bracketleft", "["},
    {"parenright", "0"}, {"bracketright", "]"}, {"quotedbl", "'"}, {"space", " "}, {"dollar", "$"}, {"plus", "+"}, {"asterisk", "*"},
    {"underscore", "_"}, {"bar", "|"}, {"grave", "`"}, {"at", "2"}, {"percent", "5"}, {"greater", ">."}, {"asciicircum", "6"},
    {"braceleft", "["}, {"colon", ":"}, {"comma", ",<"}, {"asciitilde", "~"}, {"ampersand", "7"}, {"braceright", "]"}, {"Escape", "Esc"}
};


KeyboardWorker::KeyboardWorker(KeyboardModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_keyboardDBusProxy(new KeyboardDBusProxy(this))
    , m_translatorLanguage(nullptr)
    , m_inputDevCfg(nullptr)
    , m_isTreelandSession(Dtk::Gui::DGuiApplicationHelper::testAttribute(
               Dtk::Gui::DGuiApplicationHelper::IsWaylandPlatform))
{
    qCDebug(lcKeyboardWorker) << "KeyboardWorker: session type =" << (m_isTreelandSession ? "Treeland/Wayland" : "X11/DBus");
#ifdef ENABLE_TREELAND_INPUT_MANAGER
    if (m_isTreelandSession) {
        m_waylandProxy = new DCC_NAMESPACE::KeyboardWaylandProxy(this);
        qCDebug(lcKeyboardWorker) << "KeyboardWorker: KeyboardWaylandProxy created for treeland session";
        // treeland 下由 waylandProxy 上报键盘重复参数、NumLock 和 keyboardAvailable 状态
        connect(m_waylandProxy, &DCC_NAMESPACE::KeyboardWaylandProxy::RepeatDelayChanged,
                this, &KeyboardWorker::setModelRepeatDelay);
        connect(m_waylandProxy, &DCC_NAMESPACE::KeyboardWaylandProxy::RepeatIntervalChanged,
                this, &KeyboardWorker::setModelRepeatInterval);
        connect(m_waylandProxy, &DCC_NAMESPACE::KeyboardWaylandProxy::NumLockStateChanged,
                m_model, &KeyboardModel::setNumLock);
        connect(m_waylandProxy, &DCC_NAMESPACE::KeyboardWaylandProxy::KeyboardAvailableChanged,
                m_model, &KeyboardModel::setKeyboardEnabled);
        m_deviceProxy = m_waylandProxy;
    } else
#endif
    {
        m_inputDevCfg = DConfig::create("org.deepin.dde.daemon", "org.deepin.dde.daemon.inputdevices", QString(), this);
        connect(m_keyboardDBusProxy, &KeyboardDBusProxy::NumLockStateChanged, m_model, &KeyboardModel::setNumLock);
        connect(m_keyboardDBusProxy, &KeyboardDBusProxy::RepeatDelayChanged, this, &KeyboardWorker::setModelRepeatDelay);
        connect(m_keyboardDBusProxy, &KeyboardDBusProxy::RepeatIntervalChanged, this, &KeyboardWorker::setModelRepeatInterval);
        m_deviceProxy = m_keyboardDBusProxy;
    }

    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::compositingEnabledChanged, this, &KeyboardWorker::onGetWindowWM);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::AllShortcutsReady, this, &KeyboardWorker::onAllShortcutsReady);
    // Wayland: category metadata (ordering/display/custom flag) from the
    // service's ListCategories(). Feeds the model so it never hardcodes
    // category strings for grouping or Custom detection.
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::categoriesReady, this, [this](const QList<CategoryInfoNew> &cats) {
        if (!m_shortcutModel)
            return;
        QList<ShortcutModel::CategoryMeta> meta;
        meta.reserve(cats.size());
        for (const auto &c : cats) {
            ShortcutModel::CategoryMeta m;
            m.key = c.key;
            m.displayName = c.displayName;
            m.order = c.order;
            m.isCustom = c.isCustom;
            meta.append(m);
        }
        m_shortcutModel->setCategoryMeta(meta);
    });
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::searchShortcutsReady, this, [this](const QString &json) {
        if (m_shortcutModel)
            m_shortcutModel->setSearchResult(json);
    });
    // Wayland GetShortcut results arrive on this signal; route them
    // directly to the model so they never re-enter onShortcutChanged.
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::shortcutQueried, this, [this](const QString &json) {
        if (m_shortcutModel)
            m_shortcutModel->onKeyBindingChanged(json);
    });
    // Wayland: server-originated removal (ShortcutRemoved) updates the model
    // directly so the row leaves the UI (e.g. on Reset). The X11 Deleted→removed
    // connection above is a dead-end relay, left intact for X11 parity.
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::shortcutRemovedById, this, [this](const QString &id) {
        if (m_shortcutModel)
            m_shortcutModel->removeShortcutById(id);
    });
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::UserLayoutListChanged, this, &KeyboardWorker::onUserLayout);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::CurrentLayoutChanged, this, &KeyboardWorker::onCurrentLayout);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::CapslockToggleChanged, m_model, &KeyboardModel::setCapsLock);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::langSelectorServiceStartFinished, this, [=] {
        QTimer::singleShot(100, this, &KeyboardWorker::onLangSelectorServiceFinished);
    });


    m_model->setLangChangedState(m_keyboardDBusProxy->localeState());
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::LocaleStateChanged, m_model, &KeyboardModel::setLangChangedState);

    if (!m_isTreelandSession)
        initKeyboardEnabledSync();
}

void KeyboardWorker::resetAll() {
    // Prevent duplicate calls while resetting
    if (m_isResetting) {
        qDebug() << "KeyboardWorker::resetAll: Already resetting, ignoring duplicate call";
        return;
    }

    m_isResetting = true;
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(m_keyboardDBusProxy->KeybindingReset(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] (QDBusPendingCallWatcher *reply) {
        watcher->deleteLater();

        if (reply->isError()) {
            qDebug() << Q_FUNC_INFO << reply->error();
        }

        // Reset completed, restore flag and emit signal
        m_isResetting = false;
        Q_EMIT onResetFinished();
    });
}

void KeyboardWorker::onGetWindowWM(bool)
{
    windowSwitch();
}

void KeyboardWorker::setShortcutModel(ShortcutModel *model)
{
    m_shortcutModel = model;
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::KeyEvent,
            model, &ShortcutModel::keyEvent, Qt::UniqueConnection);
}

void KeyboardWorker::refreshShortcut()
{
    m_keyboardDBusProxy->ListAllShortcuts();
    m_keyboardDBusProxy->ListCategories();
}

void KeyboardWorker::refreshLang()
{
    m_keyboardDBusProxy->blockSignals(false);
    if (!m_keyboardDBusProxy->langSelectorIsValid())
        m_keyboardDBusProxy->langSelectorStartServiceProcess();
    else
        onLangSelectorServiceFinished();
}

void KeyboardWorker::windowSwitch()
{
    if (m_isTreelandSession) {
        return;
    }
    QDBusInterface licenseInfo("com.deepin.wm",
                               "/com/deepin/wm",
                               "com.deepin.wm",
                               QDBusConnection::sessionBus());
    if (!licenseInfo.isValid()) {
        qDebug() << "com.deepin.license error ," << licenseInfo.lastError().name();
        return;
    }

    if (m_shortcutModel)
        m_shortcutModel->onWindowSwitchChanged(licenseInfo.property("compositingEnabled").toBool());
}

void KeyboardWorker::active()
{
    qCDebug(lcKeyboardWorker) << "KeyboardWorker::active()";
    m_keyboardDBusProxy->blockSignals(false);

    m_deviceProxy->active();
    setModelRepeatDelay(m_deviceProxy->repeatDelay());
    setModelRepeatInterval(m_deviceProxy->repeatInterval());
    qCDebug(lcKeyboardWorker) << "initial repeatDelay=" << m_deviceProxy->repeatDelay()
                              << "repeatInterval=" << m_deviceProxy->repeatInterval();

    m_metaDatas.clear();
    m_letters.clear();

    Q_EMIT onDatasChanged(m_metaDatas);
    Q_EMIT onLettersChanged(m_letters);

    m_model->setCapsLock(m_keyboardDBusProxy->capslockToggle());
    m_model->setNumLock(m_deviceProxy->numLockState());

    onRefreshKBLayout();
    refreshLang();
    windowSwitch();
    refreshShortcut();
#ifdef ENABLE_TREELAND_INPUT_MANAGER
    if (m_waylandProxy) {
        m_model->setKeyboardEnabled(m_waylandProxy->keyboardAvailable());
    } else
#endif
    {
        syncKeyboardEnabled();
    }
}

void KeyboardWorker::deactive()
{
    m_keyboardDBusProxy->blockSignals(true);
    if (m_deviceProxy)
        m_deviceProxy->deactive();
}

void KeyboardWorker::refreshKeyboard()
{
#ifdef ENABLE_TREELAND_INPUT_MANAGER
    if (m_waylandProxy) {
        qCDebug(lcKeyboardWorker) << "KeyboardWorker::refreshKeyboard() via wayland proxy";
        m_waylandProxy->refreshKeyboard();
        return;
    }
#endif
}

bool KeyboardWorker::keyOccupy(const QStringList &list)
{
    int bit = 0;
    for (QString t : list) {
        if (t == "Control")
            bit +=  Modifier::control;
        else if (t == "Alt")
            bit += Modifier::alt;
        else if (t == "Super")
            bit += Modifier::super;
        else if (t == "Shift")
            bit += Modifier::shift;
        else
            continue;
    }

    QMap<QStringList,int> keylist = m_model->allShortcut();
    QMap<QStringList, int>::iterator i;
    for (i = keylist.begin(); i != keylist.end(); ++i) {
        if (bit == i.value() && i.key().last() == list.last()) {
            return false;
        }
    }

    return true;
}

#ifndef DCC_DISABLE_KBLAYOUT
void KeyboardWorker::onRefreshKBLayout()
{
    QDBusPendingCallWatcher *allLayoutResult = new QDBusPendingCallWatcher(m_keyboardDBusProxy->AllLayoutList(), this);
    connect(allLayoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onAllLayoutListsFinished);
    QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardDBusProxy->LayoutList(), this);
    connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onLayoutListsFinished);

    onCurrentLayout(m_keyboardDBusProxy->currentLayout());
    onUserLayout(m_keyboardDBusProxy->userLayoutList());
}
#endif

void KeyboardWorker::modifyShortcutEditAux(ShortcutInfo *info, quint64 generation)
{
    if (!info || !isCurrentShortcutGeneration(info->id, static_cast<int>(info->type), generation))
        return;

    const QString shortcut = canonicalShortcut(info->accels);
    lookupShortcutConflict(info->id, static_cast<int>(info->type), shortcut, generation);
}

void KeyboardWorker::modifyShortcutEdit(ShortcutInfo *info, quint64 generation)
{
    modifyShortcutEditAux(info, generation);
}

void KeyboardWorker::replaceShortcutEdit(ShortcutInfo *info, const QString &expectedConflictId,
                                         quint64 generation)
{
    if (!info || expectedConflictId.isEmpty()
            || !isCurrentShortcutGeneration(info->id, static_cast<int>(info->type), generation)) {
        return;
    }

    const QString shortcut = canonicalShortcut(info->accels);
    QDBusPendingReply<bool> reply = m_keyboardDBusProxy->callReplaceHotkey(
            info->id, shortcut, expectedConflictId);
    auto *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("id", info->id);
    watcher->setProperty("type", static_cast<int>(info->type));
    watcher->setProperty("shortcut", shortcut);
    watcher->setProperty("generation", generation);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &KeyboardWorker::onReplaceHotkeyFinished);
}

void KeyboardWorker::addCustomShortcut(const QString &name, const QString &command,
                                       const QString &accels, quint64 requestId)
{
    QDBusPendingReply<QString> reply = m_keyboardDBusProxy->AddCustomShortcut(name, command, accels);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("requestId", QVariant::fromValue(requestId));
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &KeyboardWorker::onAddCustomShortcutFinished);
}

void KeyboardWorker::callModifyCustomShortcut(const QString &id, const QString &name,
                                              const QString &command, const QString &accels,
                                              int type, quint64 requestId)
{
    QDBusPendingReply<bool> reply = m_keyboardDBusProxy->ModifyCustomShortcut(id, name, command, accels);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("id", id);
    watcher->setProperty("type", type);
    watcher->setProperty("requestId", QVariant::fromValue(requestId));
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &KeyboardWorker::onModifyCustomShortcutFinished);
}

void KeyboardWorker::requestShortcutCommand(const QString &id, quint64 requestSerial)
{
    ShortcutInfo *shortcut = m_shortcutModel ? m_shortcutModel->findInfoIf([&id](ShortcutInfo *info) {
        return info->id == id;
    }) : nullptr;
    const QString fallbackCommand = shortcut ? shortcut->command : QString();

    if (id.isEmpty()) {
        Q_EMIT shortcutCommandReady(id, fallbackCommand, !id.isEmpty(), requestSerial);
        return;
    }

    QDBusPendingReply<QString> reply = m_keyboardDBusProxy->GetShortcutCommand(id);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("id", id);
    watcher->setProperty("fallbackCommand", fallbackCommand);
    watcher->setProperty("requestSerial", QVariant::fromValue(requestSerial));
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &KeyboardWorker::onGetShortcutCommandFinished);
}

void KeyboardWorker::modifyCustomShortcut(ShortcutInfo *info, quint64 requestId)
{
    callModifyCustomShortcut(info->id, info->name, info->command, info->accels, info->type,
                             requestId);
}

void KeyboardWorker::replaceCustomShortcut(const QString &id, const QString &name,
                                           const QString &command, const QString &accels,
                                           const QString &expectedConflictId,
                                           quint64 generation, quint64 requestId)
{
    const int type = ShortcutModel::Custom;
    if (expectedConflictId.isEmpty()
            || !isCurrentShortcutGeneration(id, type, generation)) {
        return;
    }

    if (id.isEmpty()) {
        QDBusPendingReply<QString> reply = m_keyboardDBusProxy->AddCustomShortcutWithConflict(
                name, command, accels, expectedConflictId);
        auto *watcher = new QDBusPendingCallWatcher(reply, this);
        watcher->setProperty("id", id);
        watcher->setProperty("type", type);
        watcher->setProperty("generation", generation);
        watcher->setProperty("requestId", QVariant::fromValue(requestId));
        connect(watcher, &QDBusPendingCallWatcher::finished,
                this, &KeyboardWorker::onAddCustomShortcutFinished);
        return;
    }

    QDBusPendingReply<bool> reply = m_keyboardDBusProxy->ModifyCustomShortcutWithConflict(
            id, name, command, accels, expectedConflictId);
    auto *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("id", id);
    watcher->setProperty("type", type);
    watcher->setProperty("generation", generation);
    watcher->setProperty("requestId", QVariant::fromValue(requestId));
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &KeyboardWorker::onModifyCustomShortcutFinished);
}

void KeyboardWorker::delShortcut(ShortcutInfo* info)
{
    QDBusPendingReply<bool> reply = m_keyboardDBusProxy->DeleteCustomShortcut(info->id);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("id", info->id);
    watcher->setProperty("type", info->type);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &KeyboardWorker::onDeleteCustomShortcutFinished);
}

void KeyboardWorker::setRepeatDelay(uint value)
{
    qCDebug(lcKeyboardWorker) << "setRepeatDelay: slider=" << value
                              << "-> dbus=" << converToDBusDelay(value);
    m_deviceProxy->setRepeatDelay(converToDBusDelay(value));
}

void KeyboardWorker::setRepeatInterval(uint value)
{
    qCDebug(lcKeyboardWorker) << "setRepeatInterval: slider=" << value
                              << "-> dbus=" << converToDBusInterval(value);
    m_deviceProxy->setRepeatInterval(static_cast<uint>(converToDBusInterval(value)));
}

void KeyboardWorker::setModelRepeatDelay(uint value)
{
    m_model->setRepeatDelay(converToModelDelay(value));
}

void KeyboardWorker::setModelRepeatInterval(uint value)
{
    m_model->setRepeatInterval(converToModelInterval(value));
}

void KeyboardWorker::setNumLock(bool value)
{
    qCDebug(lcKeyboardWorker) << "setNumLock:" << value;
    m_deviceProxy->setNumLockState(value ? 1 : 0);
}

void KeyboardWorker::setCapsLock(bool value)
{
    qCDebug(lcKeyboardWorker) << "setCapsLock:" << value;
    m_keyboardDBusProxy->setCapslockToggle(value);
}

void KeyboardWorker::setKeyboardEnabled(bool value)
{
    if (!m_inputDevCfg) {
        if (m_isTreelandSession) {
            m_model->setKeyboardEnabled(value);
            return;
        }
        qWarning() << "DConfig is null.";
        return;
    }
    if (!m_inputDevCfg->isValid()) {
        qWarning() << QString("DConfig is invalide, name:[%1], subpath[%2].").arg(m_inputDevCfg->name(), m_inputDevCfg->subpath());
        return;
    }

    // 先同步 model，保证 Wayland 下界面状态立即更新；后续仍由 DConfig 变更信号做一致性回写。
    m_model->setKeyboardEnabled(value);
    m_inputDevCfg->setValue("keyboardEnabled", value);
    syncKeyboardEnabled();
}

void KeyboardWorker::initKeyboardEnabledSync()
{
    if (!m_inputDevCfg) {
        qWarning() << "DConfig is null.";
        return;
    }
    if (!m_inputDevCfg->isValid()) {
        qWarning() << QString("DConfig is invalide, name:[%1], subpath[%2].").arg(m_inputDevCfg->name(), m_inputDevCfg->subpath());
        return;
    }

    connect(m_inputDevCfg, &DConfig::valueChanged, this, [this](const QString &key) {
        if (key == "keyboardEnabled")
            syncKeyboardEnabled();
    });
}

void KeyboardWorker::syncKeyboardEnabled()
{
    if (!m_inputDevCfg) {
        qWarning() << "DConfig is null.";
        return;
    }
    if (!m_inputDevCfg->isValid()) {
        qWarning() << QString("DConfig is invalide, name:[%1], subpath[%2].").arg(m_inputDevCfg->name(), m_inputDevCfg->subpath());
        return;
    }

    QMetaObject::invokeMethod(m_model, "setKeyboardEnabled", Qt::DirectConnection,
                              Q_ARG(bool, m_inputDevCfg->value("keyboardEnabled", true).toBool()));
}

void KeyboardWorker::addUserLayout(const QString &value)
{
    // Use allLayout as the data source
    const auto &layouts = m_model->allLayout();
    // Find the layout key in the layout source
    QString layoutKey = layouts.key(value);

    // If not found, the value might already be a key, try to use it directly
    if (layoutKey.isEmpty() && layouts.contains(value)) {
        layoutKey = value;
    }

    // If we still don't have a valid key, log error and return
    if (layoutKey.isEmpty()) {
        qWarning() << "KeyboardWorker::addUserLayout: Could not find layout key for value:" << value;
        qWarning() << "Available layout keys:" << layouts.keys();
        return;
    }

    qDebug() << "KeyboardWorker::addUserLayout: Using layout key:" << layoutKey << "for value:" << value;
    m_keyboardDBusProxy->AddUserLayout(layoutKey);
}

void KeyboardWorker::delUserLayout(const QString &value)
{
    m_keyboardDBusProxy->DeleteUserLayout(m_model->userLayout().key(value));
}

bool caseInsensitiveLessThan(const MetaData &s1, const MetaData &s2)
{
    QCollator qc;
    int i = qc.compare(s1.pinyin(), s2.pinyin());
    if (i < 0)
        return true;
    else
        return false;
}

void KeyboardWorker::onAllShortcutsReady(const QString &info)
{
    parseShortcutListJson(info);
}

void KeyboardWorker::parseShortcutListJson(const QString &info)
{
    QMap<QStringList,int> map;
    QJsonArray array = QJsonDocument::fromJson(info.toStdString().c_str()).array();
    Q_FOREACH(QJsonValue value, array) {
        QJsonObject obj = value.toObject();
        if (obj.isEmpty())
            continue;
        if (obj["Accels"].toArray().isEmpty())
            continue;
        QString accels = obj["Accels"].toArray().at(0).toString();
        accels.replace("<", "");
        accels.replace(">", "-");
        //转换为list
        QStringList key;
        key = accels.split("-");
        int bit = 0;
        for (QString &t : key) {
            if (t == "Control")
                bit += Modifier::control;
            else if (t == "Alt")
                bit += Modifier::alt;
            else if (t == "Super")
                bit += Modifier::super;
            else if (t == "Shift")
                bit += Modifier::shift;
            else {
                QString s = t;
                s = ModelKeycode.value(s);
                if (!s.isEmpty())
                    t = s;
            }
        }
        if (bit == 0)
            continue;

        map.insert(key, bit);
    }
    m_model->setAllShortcut(map);
    if (m_shortcutModel)
        m_shortcutModel->onParseInfo(info);
}

void KeyboardWorker::onAdded(const QString &in0, int in1)
{
    m_keyboardDBusProxy->GetShortcut(in0, in1);
}

void KeyboardWorker::onDisableShortcut(ShortcutInfo *info)
{
    QDBusPendingReply<bool> reply = m_keyboardDBusProxy->ClearShortcutKeystrokes(
        info->id, static_cast<int>(info->type));
    auto *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("id", info->id);
    watcher->setProperty("type", static_cast<int>(info->type));
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        QDBusPendingReply<bool> result = *watcher;
        if (result.isError() || !result.value()) {
            qWarning() << "Disable shortcut failed:" << watcher->property("id").toString()
                       << (result.isError() ? result.error().message() : QStringLiteral("server returned false"));
            onShortcutChanged(watcher->property("id").toString(), watcher->property("type").toInt());
        }
        watcher->deleteLater();
    });
}

void KeyboardWorker::onLayoutListsFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<KeyboardLayoutList> reply = *watch;

    KeyboardLayoutList tmp_map = reply.value();
    m_model->setLayoutLists(tmp_map);

    watch->deleteLater();
}

void KeyboardWorker::onAllLayoutListsFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<KeyboardLayoutList> reply = *watch;

    KeyboardLayoutList tmp_map = reply.value();
    m_model->setAllLayoutLists(tmp_map);

    watch->deleteLater();
}

void KeyboardWorker::onLocalListsFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<LocaleList> reply = *watch;

    m_datas.clear();

    LocaleList list = reply.value();

    QStringList languageCodes;
    QList<MetaData> metaDatas;
    for (int i = 0; i != list.size(); ++i) {
        MetaData md;
        md.setKey(list.at(i).id);
        languageCodes << list.at(i).id;
        metaDatas << md;
    }
    QStringList dialectNames = DCCLocale::dialectNames(languageCodes);
    for (int i = 0; i != metaDatas.size(); ++i) {
        metaDatas[i].setText(QString("%1 - %2").arg(list.at(i).name).arg(dialectNames.at(i)));
    }
    m_datas.append(metaDatas);

    std::sort(m_datas.begin(), m_datas.end(), caseInsensitiveLessThan);

    m_model->setLocaleList(m_datas);

    watch->deleteLater();

    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::CurrentLocaleChanged, m_model, &KeyboardModel::setLang);
    connect(m_keyboardDBusProxy, &KeyboardDBusProxy::LocalesChanged, m_model, &KeyboardModel::setLocaleLang);
    m_model->setLocaleLang(m_keyboardDBusProxy->locales());
    m_model->setLang(m_keyboardDBusProxy->currentLocale());
}

void KeyboardWorker::onUserLayout(const QStringList &list)
{
    m_model->cleanUserLayout();
    m_model->getUserLayoutList() = list;

    for (const QString &data : list) {
        QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardDBusProxy->GetLayoutDesc(data), this);
        layoutResult->setProperty("id", data);
        connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onUserLayoutFinished);
    }
}

void KeyboardWorker::onUserLayoutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    m_model->addUserLayout(watch->property("id").toString(), reply.value());

    watch->deleteLater();
}

void KeyboardWorker::onCurrentLayout(const QString &value)
{
    QDBusPendingCallWatcher *layoutResult = new QDBusPendingCallWatcher(m_keyboardDBusProxy->GetLayoutDesc(value), this);
    connect(layoutResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onCurrentLayoutFinished);
}

void KeyboardWorker::onSearchShortcuts(const QString &searchKey)
{
    qDebug() << "onSearchShortcuts: " << searchKey;
    m_keyboardDBusProxy->SearchShortcuts(searchKey);
}

void KeyboardWorker::onCurrentLayoutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;

    m_model->setLayout(reply.value());

    watch->deleteLater();
}

void KeyboardWorker::onPinyin()
{
    m_letters.clear();
    m_metaDatas.clear();
    QDBusInterface dbus_pinyin("org.deepin.dde.Pinyin1", "/org/deepin/dde/Pinyin1",
                               "org.deepin.dde.Pinyin1");

    const auto &currentLayouts = m_model->kbLayout();
    const auto &layouts = m_model->allLayout().isEmpty() ? currentLayouts : m_model->allLayout();
    Q_FOREACH (const QString &key, layouts.keys()) {
        MetaData md;
        QString title = layouts[key];
        md.setText(title);
        md.setKey(key);
        QChar letterFirst = title[0];
        QStringList letterFirstList;
        if (letterFirst.isLower() || letterFirst.isUpper()) {
            letterFirstList << QString(letterFirst);
            md.setPinyin(title);
        } else {
            QDBusMessage message = dbus_pinyin.call("Query", title);
            letterFirstList = message.arguments()[0].toStringList();
            md.setPinyin(letterFirstList.at(0));
        }
        append(md);
    }

    QLocale locale;

    if (locale.language() == QLocale::Chinese) {
        // ListView.section does not need this....
        // QChar ch = '\0';
        // for (int i(0); i != m_metaDatas.size(); ++i)
        // {
        //     const QChar flag = m_metaDatas[i].pinyin().at(0).toUpper();
        //     if (flag == ch)
        //         continue;
        //     ch = flag;

        //     m_letters.append(ch);
        //     m_metaDatas.insert(i, MetaData(ch, true));
        // }
    } else {
        std::sort(m_metaDatas.begin(), m_metaDatas.end(), caseInsensitiveLessThan);
    }

    Q_EMIT onDatasChanged(m_metaDatas);
    Q_EMIT onLettersChanged(m_letters);
}

void KeyboardWorker::append(const MetaData &md)
{
    if(m_metaDatas.count() == 0)
    {
        m_metaDatas.append(md);
        return;
    }

    int index = 0;
    for (int i = 0; i != m_metaDatas.size(); ++i) {
        if(m_metaDatas.at(i) > md)
        {
            m_metaDatas.insert(index,md);
            return;
        }
        index++;
    }

    m_metaDatas.append(md);
}

void KeyboardWorker::onLangSelectorServiceFinished()
{
    QDBusPendingCallWatcher *localResult = new QDBusPendingCallWatcher(m_keyboardDBusProxy->GetLocaleList(), this);
    connect(localResult, &QDBusPendingCallWatcher::finished, this, &KeyboardWorker::onLocalListsFinished);
    m_keyboardDBusProxy->currentLocale();
}

void KeyboardWorker::onShortcutChanged(const QString &id, int type)
{
    m_keyboardDBusProxy->Query(id, type);
}

void KeyboardWorker::updateKey(ShortcutInfo *info)
{
    if (m_shortcutModel)
        m_shortcutModel->setCurrentInfo(info);
}

QString KeyboardWorker::shortcutKey(const QString &id, int type)
{
    return id + QChar(0x1f) + QString::number(type);
}

void KeyboardWorker::setShortcutGeneration(const QString &id, int type, quint64 generation)
{
    m_shortcutGenerations.insert(shortcutKey(id, type), generation);
}

bool KeyboardWorker::isCurrentShortcutGeneration(const QString &id, int type,
                                                  quint64 generation) const
{
    return generation != 0
            && m_shortcutGenerations.value(shortcutKey(id, type)) == generation;
}

void KeyboardWorker::beginCapture(quint64 requestId)
{
    QDBusPendingReply<bool> reply = m_keyboardDBusProxy->BeginCapture();
    auto *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher, requestId] {
        QDBusPendingReply<bool> result = *watcher;
        const bool success = !result.isError() && result.value();
        const QString reason = result.isError()
                ? result.error().message()
                : (success ? QString() : QStringLiteral("server returned false"));
        Q_EMIT captureRequestFinished(requestId, success, reason);
        watcher->deleteLater();
    });
}

void KeyboardWorker::endCapture()
{
    m_keyboardDBusProxy->EndCapture();
}

void KeyboardWorker::cleanShortcutSlef(const QString &id, const int type, const QString &shortcut,
                                       quint64 generation)
{
    if (!isCurrentShortcutGeneration(id, type, generation))
        return;

    QDBusPendingReply<bool> reply = m_keyboardDBusProxy->callModifyHotkeys(id, QStringList{shortcut});
    auto *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("id", id);
    watcher->setProperty("type", type);
    watcher->setProperty("shortcut", shortcut);
    watcher->setProperty("generation", generation);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &KeyboardWorker::onModifyHotkeysFinished);
}

void KeyboardWorker::setNewCustomShortcut(const QString &id, const QString &name, const QString &command, const QString &accles)
{
    callModifyCustomShortcut(id, name, command, accles, ShortcutModel::Custom);
}

void KeyboardWorker::lookupShortcutConflict(const QString &id, int type, const QString &shortcut,
                                            quint64 generation, bool reportFailureIfNoConflict)
{
    if (!isCurrentShortcutGeneration(id, type, generation))
        return;

    QDBusPendingReply<ShortcutInfoNew> reply = m_keyboardDBusProxy->LookupConflictShortcut(shortcut);
    auto *watcher = new QDBusPendingCallWatcher(reply, this);
    watcher->setProperty("id", id);
    watcher->setProperty("type", type);
    watcher->setProperty("shortcut", shortcut);
    watcher->setProperty("generation", generation);
    watcher->setProperty("reportFailureIfNoConflict", reportFailureIfNoConflict);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &KeyboardWorker::onLookupConflictForShortcutFinished);
}

void KeyboardWorker::onLookupConflictForShortcutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<ShortcutInfoNew> reply = *watch;
    const QString id = watch->property("id").toString();
    const int type = watch->property("type").toInt();
    const QString shortcut = watch->property("shortcut").toString();
    const quint64 generation = watch->property("generation").toULongLong();
    const bool reportFailureIfNoConflict = watch->property("reportFailureIfNoConflict").toBool();

    if (!isCurrentShortcutGeneration(id, type, generation)) {
        watch->deleteLater();
        return;
    }

    if (reply.isError()) {
        qWarning() << "LookupConflictShortcut failed:" << reply.error();
        Q_EMIT shortcutModificationFinished(id, type, shortcut, false, generation);
        watch->deleteLater();
        return;
    }

    const ShortcutInfoNew conflict = reply.value();
    if (!conflict.id.isEmpty() && conflict.id != id) {
        const QString conflictName = conflict.localLanguageName.isEmpty()
                ? conflict.displayName : conflict.localLanguageName;
        Q_EMIT shortcutConflictDetected(id, type, shortcut, conflict.id, conflictName,
                                        conflict.modifiable, generation);
    } else if (reportFailureIfNoConflict) {
        Q_EMIT shortcutModificationFinished(id, type, shortcut, false, generation);
    } else {
        cleanShortcutSlef(id, type, shortcut, generation);
    }

    watch->deleteLater();
}

void KeyboardWorker::onModifyHotkeysFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<bool> reply = *watch;
    const QString id = watch->property("id").toString();
    const int type = watch->property("type").toInt();
    const QString shortcut = watch->property("shortcut").toString();
    const quint64 generation = watch->property("generation").toULongLong();
    if (!isCurrentShortcutGeneration(id, type, generation)) {
        watch->deleteLater();
        return;
    }

    const bool success = (!reply.isError() && reply.value());
    if (!success) {
        qWarning() << "ModifyHotkeys failed for" << id
                   << (reply.isError() ? reply.error().message() : QStringLiteral("server returned false"));
        lookupShortcutConflict(id, type, shortcut, generation, true);
    } else {
        Q_EMIT shortcutModificationFinished(id, type, shortcut, true, generation);
    }
    // On success the server emits ShortcutChanged, which the proxy forwards via
    // shortcutQueried → onKeyBindingChanged to update the model. No extra work here.

    watch->deleteLater();
}

void KeyboardWorker::onReplaceHotkeyFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<bool> reply = *watch;
    const QString id = watch->property("id").toString();
    const int type = watch->property("type").toInt();
    const QString shortcut = watch->property("shortcut").toString();
    const quint64 generation = watch->property("generation").toULongLong();
    if (!isCurrentShortcutGeneration(id, type, generation)) {
        watch->deleteLater();
        return;
    }

    const bool success = (!reply.isError() && reply.value());
    if (!success) {
        qWarning() << "ReplaceHotkey failed for" << id
                   << (reply.isError() ? reply.error().message() : QStringLiteral("server returned false"));
    }
    Q_EMIT shortcutModificationFinished(id, type, shortcut, success, generation);
    // On success the server emits two ShortcutChanged signals (one per
    // shortcut), which the proxy forwards to update both model rows.

    watch->deleteLater();
}

void KeyboardWorker::onAddCustomShortcutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;
    const quint64 requestId = watch->property("requestId").toULongLong();
    const quint64 generation = watch->property("generation").toULongLong();
    if (generation != 0
            && !isCurrentShortcutGeneration(watch->property("id").toString(),
                                            watch->property("type").toInt(),
                                            generation)) {
        if (requestId != 0) {
            Q_EMIT customShortcutOperationFinished(
                    requestId, false, tr("The shortcut conflict is no longer current."));
        }
        watch->deleteLater();
        return;
    }

    const bool success = !reply.isError() && !reply.value().isEmpty();
    const QString errorMessage = reply.isError()
            ? customShortcutErrorMessage(reply.error())
            : (success ? QString() : tr("Failed to save the shortcut. Please try again."));
    if (!success) {
        qWarning() << "AddCustomShortcut failed:"
                   << (reply.isError() ? reply.error().message() : QStringLiteral("server returned empty id"));
        refreshShortcut();
    } else {
        onAdded(reply.value(), ShortcutModel::Custom);
    }
    if (requestId != 0)
        Q_EMIT customShortcutOperationFinished(requestId, success, errorMessage);

    watch->deleteLater();
}

void KeyboardWorker::onModifyCustomShortcutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<bool> reply = *watch;
    const QString id = watch->property("id").toString();
    const int type = watch->property("type").toInt();
    const quint64 requestId = watch->property("requestId").toULongLong();
    const quint64 generation = watch->property("generation").toULongLong();
    if (generation != 0 && !isCurrentShortcutGeneration(id, type, generation)) {
        if (requestId != 0) {
            Q_EMIT customShortcutOperationFinished(
                    requestId, false, tr("The shortcut conflict is no longer current."));
        }
        watch->deleteLater();
        return;
    }

    const bool success = (!reply.isError() && reply.value());
    const QString errorMessage = reply.isError()
            ? customShortcutErrorMessage(reply.error())
            : (success ? QString() : tr("Failed to save the shortcut. Please try again."));
    if (!success) {
        qWarning() << "ModifyCustomShortcut failed for" << id
                   << (reply.isError() ? reply.error().message() : QStringLiteral("server returned false"));
        refreshShortcut();
    } else if (!id.isEmpty()) {
        onShortcutChanged(id, type);
    }
    if (requestId != 0)
        Q_EMIT customShortcutOperationFinished(requestId, success, errorMessage);

    watch->deleteLater();
}

void KeyboardWorker::onDeleteCustomShortcutFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<bool> reply = *watch;
    const QString id = watch->property("id").toString();
    const bool success = (!reply.isError() && reply.value());
    if (!success) {
        qWarning() << "DeleteCustomShortcut failed for" << id
                   << (reply.isError() ? reply.error().message() : QStringLiteral("server returned false"));
    }
    refreshShortcut();

    watch->deleteLater();
}

void KeyboardWorker::onGetShortcutCommandFinished(QDBusPendingCallWatcher *watch)
{
    QDBusPendingReply<QString> reply = *watch;
    const QString id = watch->property("id").toString();
    const quint64 requestSerial = watch->property("requestSerial").toULongLong();
    QString command = watch->property("fallbackCommand").toString();

    if (reply.isError()) {
        qWarning() << "GetShortcutCommand failed for" << id << reply.error().message();
    } else {
        command = reply.value();
    }

    Q_EMIT shortcutCommandReady(id, command, !reply.isError() || !command.isEmpty(),
                                requestSerial);
    watch->deleteLater();
}

uint KeyboardWorker::converToDBusDelay(uint value)
{
    switch (value) {
    case 1:
        return 20;
    case 2:
        return 80;
    case 3:
        return 150;
    case 4:
        return 250;
    case 5:
        return 360;
    case 6:
        return 480;
    case 7:
        return 600;
    default:
        return 4;
    }
}

uint KeyboardWorker::converToModelDelay(uint value)
{
    if (value <= 20)
        return 1;
    else if (value <= 80)
        return 2;
    else if (value <= 150)
        return 3;
    else if (value <= 250)
        return 4;
    else if (value <= 360)
        return 5;
    else if (value <= 480)
        return 6;
    else
        return 7;
}

int KeyboardWorker::converToDBusInterval(int value)
{
    switch (value) {
    case 1:
        return 100;
    case 2:
        return 80;
    case 3:
        return 65;
    case 4:
        return 50;
    case 5:
        return 35;
    case 6:
        return 25;
    case 7:
        return 20;
    default:
        return 4;
    }
}

uint KeyboardWorker::converToModelInterval(uint value)
{
    if (value <= 20)
        return 7;
    else if (value <= 25)
        return 6;
    else if (value <= 35)
        return 5;
    else if (value <= 50)
        return 4;
    else if (value <= 65)
        return 3;
    else if (value <= 80)
        return 2;
    else
        return 1;
}

void KeyboardWorker::setLayout(const QString &value)
{
    m_keyboardDBusProxy->setCurrentLayout(value);
}

void KeyboardWorker::setLang(const QString &value)
{
    Q_EMIT requestSetAutoHide(false);

    QDBusPendingCall call = m_keyboardDBusProxy->SetLocale(value);
    qDebug() << "setLang is " << value;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            qDebug() << "setLang error: " << call.error().type();
            m_model->setLang(m_keyboardDBusProxy->currentLocale());
        }

        qDebug() << "setLang success";
        Q_EMIT requestSetAutoHide(true);
        watcher->deleteLater();
    });
}

void KeyboardWorker::addLang(const QString &value)
{
    Q_EMIT requestSetAutoHide(false);

    QDBusPendingCall call = m_keyboardDBusProxy->AddLocale(value);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            qDebug() << "add Locale language error: " << call.error().type();
        }

        Q_EMIT requestSetAutoHide(true);
        watcher->deleteLater();
    });
}

void KeyboardWorker::deleteLang(const QString &value)
{
    Q_EMIT requestSetAutoHide(false);

    QString lang = m_model->langFromText(value);
    QDBusPendingCall call = m_keyboardDBusProxy->DeleteLocale(lang);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [=] {
        if (call.isError()) {
            qDebug() << "delete Locale language error: " << call.error().type();
        }

        Q_EMIT requestSetAutoHide(true);
        watcher->deleteLater();
    });
}
