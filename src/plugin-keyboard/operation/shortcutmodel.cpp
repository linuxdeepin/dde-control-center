//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "shortcutmodel.h"

#include <DSysInfo>
#include <QDBusInterface>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QThreadPool>
#include <QGuiApplication>
#include <DPinyin>

QStringList systemFilter = {"terminal",
                            "terminalQuake",
                            "globalSearch",
                            "screenshot",
                            "screenshotDelayed",
                            "screenshotFullscreen",
                            "screenshotWindow",
                            "screenshotScroll",
                            "screenshotOcr",
                            "deepinScreenRecorder",
                            "switchGroup",
                            "switchGroupBackward",
                            "previewWorkspace",
                            "launcher",
                            "switchApplications",
                            "switchApplicationsBackward",
                            "showDesktop",
                            "fileManager",
                            "lockScreen",
                            "logout",
                            "wmSwitcher",
                            "systemMonitor",
                            "colorPicker",
                            "clipboard",
                            "switchMonitors"
};

const QStringList &windowFilter = {"activateWindowMenu",
                                   "maximize",
                                   "unmaximize",
                                   "minimize",
                                   "beginMove",
                                   "beginResize",
                                   "close",
                                   "toggleToLeft",
                                   "toggleToRight"
};

const QStringList &workspaceFilter = {"switchToWorkspaceLeft",
                                      "switchToWorkspaceRight",
                                      "moveToWorkspaceLeft",
                                      "moveToWorkspaceRight"};

const QStringList &assistiveToolsFilter = {"textToSpeech",
                                           "speechToText",
                                           "translation",
                                            "viewZoomIn",
                                            "viewZoomOut",
                                            "viewActualSize"};

// from dquickrectangle_p.h
#define NoneCorner 0x0
#define TopLeftCorner 0x1
#define TopRightCorner 0x2
#define BottomLeftCorner 0x4
#define BottomRightCorner 0x8
#define TopCorner TopLeftCorner | TopRightCorner
#define BottomCorner BottomLeftCorner| BottomRightCorner
#define LeftCorner TopLeftCorner| BottomLeftCorner
#define RightCorner TopRightCorner| BottomRightCorner
#define AllCorner TopCorner | BottomCorner

static const QString FieldSeparator = QString(QChar(0x1F));

static const QMap<QString, QString> &DisplaykeyMap = { {"exclam", "!"}, {"at", "@"}, {"numbersign", "#"}, {"dollar", "$"}, {"percent", "%"},
    {"asciicircum", "^"}, {"ampersand", "&"}, {"asterisk", "*"}, {"parenleft", "("},
    {"parenright", ")"}, {"underscore", "_"}, {"plus", "+"}, {"braceleft", "{"}, {"braceright", "}"},
    {"bar", "|"}, {"colon", ":"}, {"quotedbl", "\""}, {"less", "<"}, {"greater", ">"}, {"question", "?"},
    {"minus", "-"}, {"equal", "="}, {"brackertleft", "["}, {"breckertright", "]"}, {"backslash", "\\"},
    {"semicolon", ";"}, {"apostrophe", "'"}, {"comma", ","}, {"period", "."}, {"slash", "/"}, {"Up", "↑"},
    {"Left", "←"}, {"Down", "↓"}, {"Right", "→"}, {"asciitilde", "~"}, {"grave", "`"}, {"Control", "Ctrl"},
    {"Super_L", "Super"}, {"Super_R", "Super"}
};

static QString toPinyin(const QString &name)
{
    DCORE_USE_NAMESPACE
    return pinyin(name, TS_NoneTone).join(FieldSeparator) + FieldSeparator + firstLetters(name).join(FieldSeparator);
}

static void fillShortcutInfoFromJson(dccV25::ShortcutInfo *info, const QJsonObject &obj)
{
    if (!info)
        return;

    info->type = obj["Type"].toInt();
    const QJsonArray accels = obj["Accels"].toArray();
    info->accels = accels.isEmpty() ? QString() : accels.first().toString();
    info->name = obj["Name"].toString();
    info->pinyin = toPinyin(info->name);
    info->id = obj["Id"].toString();
    info->command = obj["Exec"].toString();

    const QString sectionKey = obj["Section"].toString();
    if (!sectionKey.isEmpty()) {
        info->sectionKey = sectionKey;
        info->sectionName = obj["SectionName"].toString();
        if (info->sectionName.isEmpty())
            info->sectionName = sectionKey;
    }
}

using namespace dccV25;
DCORE_USE_NAMESPACE
ShortcutModel::ShortcutModel(QObject *parent)
    : QObject(parent)
    , m_windowSwitchState(false)
{
}

ShortcutModel::~ShortcutModel()
{
    qDeleteAll(m_infos);

    m_infos.clear();
    m_systemInfos.clear();
    m_windowInfos.clear();
    m_workspaceInfos.clear();
    m_appInfos.clear();
    m_customInfos.clear();
    qDeleteAll(m_searchList);
    m_searchList.clear();
}

QList<ShortcutInfo *> ShortcutModel::systemInfo() const
{
    return m_systemInfos;
}

QList<ShortcutInfo *> ShortcutModel::windowInfo() const
{
    return m_windowInfos;
}

QList<ShortcutInfo *> ShortcutModel::workspaceInfo() const
{
    return m_workspaceInfos;
}

QList<ShortcutInfo *> ShortcutModel::assistiveToolsInfo() const
{
    return m_assistiveToolsInfos;
}

QList<ShortcutInfo *> ShortcutModel::appInfo() const
{
    return m_appInfos;
}

QList<ShortcutInfo *> ShortcutModel::customInfo() const
{
    // Wayland dynamic grouping: the custom group lives in m_groupInfos under
    // the service-reported custom key (falls back to the legacy list on X11).
    if (!m_groupInfos.isEmpty()) {
        const QString key = customCategoryKey();
        if (!key.isEmpty())
            return m_groupInfos.value(key);
    }
    return m_customInfos;
}

QList<ShortcutInfo *> ShortcutModel::infos() const
{
    return m_infos;
}

ShortcutInfo *ShortcutModel::shortcutAt(int index, int *corners)
{
    if (index < 0)
        return nullptr;

    auto getCorners = [](const QList<ShortcutInfo *> &list, int index) {
        if (index == 0)
            return TopCorner;
        else if (index == list.count() - 1)
            return BottomCorner;
        else
            return NoneCorner;
    };

    // Wayland dynamic grouping: walk groups in service-supplied order.
    if (!m_groupInfos.isEmpty()) {
        for (const QString &key : groupOrder()) {
            const auto &list = m_groupInfos[key];
            if (index < list.count()) {
                if (corners)
                    *corners = getCorners(list, index);
                return list.value(index);
            }
            index -= list.count();
        }
        return nullptr;
    }

#define CHECK_INDEX_DCC(List) \
    if (index < List.count()) { \
        if (corners) \
            *corners = getCorners(List, index); \
        return List.value(index); \
    } else { \
        index -= List.count(); \
    }

    CHECK_INDEX_DCC(m_systemInfos)
    CHECK_INDEX_DCC(m_windowInfos)
    CHECK_INDEX_DCC(m_workspaceInfos)
    CHECK_INDEX_DCC(m_assistiveToolsInfos)
    CHECK_INDEX_DCC(m_appInfos)
    CHECK_INDEX_DCC(m_customInfos)

    return nullptr;
}

void ShortcutModel::delInfo(ShortcutInfo *info)
{
    if (m_infos.contains(info)) {
        m_infos.removeOne(info);
    }
    if (m_customInfos.contains(info)) {
        m_customInfos.removeOne(info);
    }
    // Wayland dynamic-grouping container: sweep by section key too.
    for (auto it = m_groupInfos.begin(); it != m_groupInfos.end(); ++it)
        it.value().removeOne(info);

    Q_EMIT delCustomInfo(info);

    delete info;
    info = nullptr;
}

void ShortcutModel::removeShortcutById(const QString &id)
{
    // The new-API ShortcutRemoved carries only an id, so match on id alone.
    auto res = std::find_if(m_infos.begin(), m_infos.end(),
                            [&id](const ShortcutInfo *info) { return info->id == id; });
    if (res == m_infos.end())
        return;

    ShortcutInfo *info = *res;
    m_infos.removeOne(info);

    // The removed entry may live in any category list (not only custom), so
    // sweep them all. m_windowSwitchStateInfos aliases pointers also held in
    // m_systemInfos, so clear it too or it would dangle after delete.
    QList<ShortcutInfo *> *lists[] = {
        &m_systemInfos, &m_windowInfos, &m_workspaceInfos,
        &m_assistiveToolsInfos, &m_appInfos, &m_customInfos,
        &m_windowSwitchStateInfos
    };
    for (auto *list : lists)
        list->removeOne(info);

    // Wayland dynamic-grouping container: sweep by section key too.
    for (auto it = m_groupInfos.begin(); it != m_groupInfos.end(); ++it)
        it.value().removeOne(info);

    invalidateSystemShortcutNamesCache();
    // Reuse the optimistic-delete UI path: delCustomInfo → ShortcutListModel::reset.
    Q_EMIT delCustomInfo(info);

    delete info;
}

void ShortcutModel::onParseInfo(const QString &info)
{
    QStringList systemShortKeys;
    if (DSysInfo::UosServer == DSysInfo::uosType()) {
        QStringList systemFilterServer = systemFilter;
        systemFilterServer.removeOne("wm-switcher");
        systemFilterServer.removeOne("preview-workspace");
        systemShortKeys = systemFilterServer;
    } else {
        systemShortKeys = systemFilter;
    }
#ifdef DISABLE_SCREEN_RECORDING
    QStringList systemFilterServer = systemShortKeys;
    systemFilterServer.removeOne("deepin-screen-recorder");
    systemShortKeys = systemFilterServer;
#endif
    // Save custom info IDs before clearing
    QStringList customInfoIds;
    for (auto info : m_customInfos) {
        customInfoIds << info->id;
    }

    qDeleteAll(m_infos);

    m_infos.clear();
    m_systemInfos.clear();
    m_windowInfos.clear();
    m_workspaceInfos.clear();
    m_assistiveToolsInfos.clear();
    m_appInfos.clear();
    m_customInfos.clear();
    // Wayland dynamic grouping containers
    m_groupInfos.clear();
    
    // 清理系统快捷键名称缓存，因为数据即将更新
    invalidateSystemShortcutNamesCache();

    QJsonArray array = QJsonDocument::fromJson(info.toStdString().c_str()).array();

    Q_FOREACH (QJsonValue value, array) {
        QJsonObject obj  = value.toObject();
        int         type = obj["Type"].toInt();

        ShortcutInfo *info = new ShortcutInfo();
        info->type         = type;
        info->accels       = obj["Accels"].toArray().first().toString();
        info->id      = obj["Id"].toString();
        info->name    = obj["Name"].toString();
        if (systemShortKeys.contains(info->id)) {
            info->name    = info->name.trimmed();
        }
        info->pinyin  =  toPinyin(info->name);
        info->command = obj["Exec"].toString();

        m_infos << info;

        // Wayland: group by the stable category key from the new API. dcc
        // knows nothing about specific category values — ordering, display
        // names, and which group is the user-editable (Custom) one all come
        // from the service's ListCategories() metadata (see setCategoryMeta).
        // Section = logical key (grouping); SectionName = already-resolved
        // display text (rendered directly, dcc no longer translates).
        QString sectionKey = obj["Section"].toString();
        if (!sectionKey.isEmpty()) {
            info->sectionKey = sectionKey;
            info->sectionName = obj["SectionName"].toString();
            if (info->sectionName.isEmpty())
                info->sectionName = sectionKey;   // fallback to raw key
            m_groupInfos[sectionKey].append(info);
            continue;
        }

        // X11: old ID-based categorization. sectionKey mirrors sectionName
        // (the translated text) so the QML section.property:"sectionKey"
        // grouping reproduces the legacy group-by-translated-name behavior.
        // The Wayland path above instead uses stable English keys from the
        // service; X11 is legacy and single-locale, so this is equivalent.
        if (type != MEDIAKEY) {
            if (systemShortKeys.contains(info->id)) {
                info->sectionName = tr("System");
                info->sectionKey = info->sectionName;
                m_systemInfos << info;
                continue;
            }
            if (windowFilter.contains(info->id)) {
                info->sectionName = tr("Window");
                info->sectionKey = info->sectionName;
                m_windowInfos << info;
                continue;
            }
            if (workspaceFilter.contains(info->id)) {
                info->sectionName = tr("Workspace");
                info->sectionKey = info->sectionName;
                m_workspaceInfos << info;
                continue;
            }
            if (assistiveToolsFilter.contains(info->id)) {
                info->sectionName = tr("AssistiveTools");
                info->sectionKey = info->sectionName;
                m_assistiveToolsInfos << info;
                continue;
            }
            if (type == 1) {
                info->sectionName = tr("Custom");
                info->sectionKey = info->sectionName;
                m_customInfos << info;
            }
        }
    }

    std::sort(m_systemInfos.begin(), m_systemInfos.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return systemShortKeys.indexOf(s1->id) < systemShortKeys.indexOf(s2->id);
    });
    // More efficient implementation using std::find_if
    auto it = std::find_if(m_systemInfos.begin(), m_systemInfos.end(),
        [](const ShortcutInfo* info) { return info->id == "previewWorkspace"; });
    m_windowSwitchStateInfos.clear();
    if (it != m_systemInfos.end()) {
        int index = std::distance(m_systemInfos.begin(), it);
        (*it)->index = index;
        m_windowSwitchStateInfos << *it;
        if (!m_windowSwitchState) {
            m_systemInfos.erase(it);  // More efficient than removeOne
        }
    }

    std::sort(m_windowInfos.begin(), m_windowInfos.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return windowFilter.indexOf(s1->id) < windowFilter.indexOf(s2->id);
    });

    std::sort(m_workspaceInfos.begin(), m_workspaceInfos.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return workspaceFilter.indexOf(s1->id) < workspaceFilter.indexOf(s2->id);
    });

    std::sort(m_assistiveToolsInfos.begin(), m_assistiveToolsInfos.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return assistiveToolsFilter.indexOf(s1->id) < assistiveToolsFilter.indexOf(s2->id);
    });

    // Rebuild m_customInfos in original order first
    QList<ShortcutInfo*> orderedCustomInfos;
    for (const QString &id : customInfoIds) {
        auto it = std::find_if(m_customInfos.begin(), m_customInfos.end(),
            [id](ShortcutInfo *info) { return info->id == id && info->type == 1; });
        if (it != m_customInfos.end()) {
            orderedCustomInfos << *it;
        }
    }
    // Add new custom infos that weren't in original list
    for (auto info : m_customInfos) {
        if (info->type == 1 && !customInfoIds.contains(info->id)) {
            orderedCustomInfos << info;
        }
    }
    m_customInfos = orderedCustomInfos;

    Q_EMIT listChanged(m_systemInfos, InfoType::System);
    Q_EMIT listChanged(m_windowInfos, InfoType::Window);
    Q_EMIT listChanged(m_workspaceInfos, InfoType::Workspace);
    Q_EMIT listChanged(m_assistiveToolsInfos, InfoType::AssistiveTools);
    Q_EMIT listChanged(m_appInfos, InfoType::App);
    Q_EMIT listChanged(m_customInfos, InfoType::Custom);

    // Wayland: items are in m_groupInfos, not the legacy fixed lists above.
    // listChanged has no consumers on Wayland, so emit categoryMetaChanged
    // to trigger ShortcutListModel::reset() — the view updates immediately
    // with the data we just populated, even if ListCategories hasn't arrived
    // yet. A later ListCategories reply will re-sort groups into their final
    // order when setCategoryMeta fires categoryMetaChanged again.
    if (!m_groupInfos.isEmpty())
        Q_EMIT categoryMetaChanged();
}

void ShortcutModel::onCustomInfo(const QString &json)
{
    QJsonObject   obj  = QJsonDocument::fromJson(json.toStdString().c_str()).object();
    ShortcutInfo *info = new ShortcutInfo();
    info->type         = obj["Type"].toInt();
    QString accels     = obj["Accels"].toArray().at(0).toString();

    info->accels = accels;

    info->name    = obj["Name"].toString();
    info->pinyin = toPinyin(info->name);
    info->id      = obj["Id"].toString();
    info->command = obj["Exec"].toString();
    info->sectionName = tr("Custom");
    info->sectionKey = info->sectionName;   // X11: mirror translated name

    m_infos.append(info);
    m_customInfos.append(info);
    Q_EMIT addCustomInfo(info);
}

void ShortcutModel::onKeyBindingChanged(const QString &value)
{
    const QJsonObject &obj       = QJsonDocument::fromJson(value.toStdString().c_str()).object();
    const QString     &update_id = obj["Id"].toString();
    const int     &update_type = obj["Type"].toInt();
    const bool hasSectionField = obj.contains("Section");
    const bool useDynamicGrouping = hasSectionField || !m_groupInfos.isEmpty() || !m_categoryMeta.isEmpty();
    if (update_id.isEmpty())
        return;

    auto res = std::find_if(m_infos.begin(), m_infos.end(), [ = ] (const ShortcutInfo *info)->bool{
        return info->id == update_id && info->type == update_type;
    });

    if (res != m_infos.end()) {
        const QString oldSectionKey = (*res)->sectionKey;
        fillShortcutInfoFromJson(*res, obj);
        const bool sectionChanged = oldSectionKey != (*res)->sectionKey;
        if (useDynamicGrouping) {
            if (!oldSectionKey.isEmpty() && oldSectionKey != (*res)->sectionKey)
                m_groupInfos[oldSectionKey].removeOne(*res);
            if (!(*res)->sectionKey.isEmpty() && !m_groupInfos[(*res)->sectionKey].contains(*res))
                m_groupInfos[(*res)->sectionKey].append(*res);
        }

        bool searchUpdated = false;
        for (ShortcutInfo *searchInfo : m_searchList) {
            if (searchInfo && searchInfo->id == update_id && searchInfo->type == update_type) {
                fillShortcutInfoFromJson(searchInfo, obj);
                searchUpdated = true;
            }
        }

        Q_EMIT shortcutChanged((*res));
        if (searchUpdated)
            Q_EMIT searchFinished(m_searchList);
        if (!m_groupInfos.isEmpty() && sectionChanged)
            Q_EMIT categoryMetaChanged();
        return;
    }

    ShortcutInfo *info = new ShortcutInfo();
    fillShortcutInfoFromJson(info, obj);

    if (!info->sectionKey.isEmpty()) {
        m_infos.append(info);
        m_groupInfos[info->sectionKey].append(info);
        if (info->type == Custom)
            m_customInfos.append(info);
        invalidateSystemShortcutNamesCache();
        Q_EMIT addCustomInfo(info);
        Q_EMIT categoryMetaChanged();
        return;
    }

    if (info->type != Custom) {
        delete info;
        return;
    }

    info->sectionName = tr("Custom");
    const QString customKey = useDynamicGrouping ? customCategoryKey() : QString();
    info->sectionKey = customKey.isEmpty() ? info->sectionName : customKey;
    m_infos.append(info);
    if (!customKey.isEmpty())
        m_groupInfos[info->sectionKey].append(info);
    m_customInfos.append(info);
    Q_EMIT addCustomInfo(info);
    if (!customKey.isEmpty())
        Q_EMIT categoryMetaChanged();
}

void ShortcutModel::onWindowSwitchChanged(bool value)
{
    if (m_windowSwitchState != value) {
        m_windowSwitchState = value;
        if (m_windowSwitchState) {
            for (int i = m_windowSwitchStateInfos.size() - 1; i >= 0; i--) {
                m_systemInfos.insert(m_windowSwitchStateInfos.at(i)->index, m_windowSwitchStateInfos.at(i));
            }
        } else {
            for (int i = 0; i < m_windowSwitchStateInfos.size(); i++) {
                m_systemInfos.removeOne(m_windowSwitchStateInfos.at(i));
            }
        }
        
        // 系统快捷键列表发生变化，清理缓存
        invalidateSystemShortcutNamesCache();
        
        Q_EMIT windowSwitchChanged(m_windowSwitchState);
    }
}

 bool ShortcutModel::getWindowSwitch()
 {
     return m_windowSwitchState;
 }

 QStringList ShortcutModel::formatKeys(const QString &shortcut)
 {
     if (shortcut.isEmpty())
         return QStringList{ShortcutModel::tr("None")};

     QString accels = shortcut;
     accels = accels.replace("<", "");
     accels = accels.replace(">", "-");
     // Wayland keystroke capture produces Qt PortableText ("Ctrl+Alt+T",
     // "Meta+P") instead of the X11 XKB form. Normalize both so the split
     // below produces one chip per token.
     // TODO(Wayland '+' key): this replace also eats a literal '+' KEY
     // (qtAccelToXkb keeps it, e.g. "<Control>+"), so a '+'-containing combo
     // renders with an empty chip. Harmless on X11 (the XKB form names that key
     // "plus", never a literal '+'), and niche on Wayland; left as-is to keep
     // this shared helper untouched. Fix would map the literal '+' to "plus".
     accels = accels.replace("+", "-");
     accels = accels.replace("Meta", "Super");
     accels = accels.replace("_L", "");
     accels = accels.replace("_R", "");
     accels = accels.replace("Control", "Ctrl");

     QStringList keylist = accels.split("-");

     QStringList newList;
     for (int i = 0; i < keylist.size(); ++i) {
         const QString &value = DisplaykeyMap.value(keylist.value(i));
         newList << (value.isEmpty() ? keylist.value(i) : value);
     }

     return newList;
}

int ShortcutModel::indexOfShortcut(ShortcutInfo *info)
{
    if (!info)
        return -1;

    // Wayland dynamic grouping: search groups in display order.
    if (!m_groupInfos.isEmpty()) {
        int row = 0;
        for (const QString &key : groupOrder()) {
            const auto &list = m_groupInfos[key];
            const int idx = list.indexOf(info);
            if (idx >= 0)
                return row + idx;
            row += list.size();
        }
        return -1;
    }

    const QList<ShortcutInfo*> *lists[] = {
        &m_systemInfos, &m_windowInfos, &m_workspaceInfos,
        &m_assistiveToolsInfos, &m_appInfos, &m_customInfos
    };

    int row = 0;
    for (const auto *list : lists) {
        const int idx = list->indexOf(info);
        if (idx >= 0)
            return row + idx;
        row += list->size();
    }

    return -1;
}

void ShortcutModel::setCategoryMeta(const QList<CategoryMeta> &meta)
{
    m_categoryMeta.clear();
    m_customCategoryKey.clear();
    for (const auto &m : meta) {
        m_categoryMeta.insert(m.key, m);
        if (m.isCustom)
            m_customCategoryKey = m.key;
    }
    if (!m_customCategoryKey.isEmpty()) {
        auto &customGroup = m_groupInfos[m_customCategoryKey];
        for (ShortcutInfo *info : std::as_const(m_customInfos)) {
            if (!info || customGroup.contains(info))
                continue;
            info->sectionKey = m_customCategoryKey;
            if (info->sectionName.isEmpty())
                info->sectionName = m_categoryMeta.value(m_customCategoryKey).displayName;
            customGroup.append(info);
        }
    }
    invalidateSystemShortcutNamesCache();
    Q_EMIT categoryMetaChanged();
}

QString ShortcutModel::sectionDisplayName(const QString &key) const
{
    // Service metadata is authoritative; fall back to the per-item resolved
    // text carried in the group, then to the raw key.
    auto it = m_categoryMeta.constFind(key);
    if (it != m_categoryMeta.constEnd() && !it.value().displayName.isEmpty())
        return it.value().displayName;

    auto git = m_groupInfos.constFind(key);
    if (git != m_groupInfos.constEnd() && !git.value().isEmpty()) {
        const QString &name = git.value().first()->sectionName;
        if (!name.isEmpty())
            return name;
    }
    return key;
}

QString ShortcutModel::customCategoryKey() const
{
    // Wayland: from service metadata (setCategoryMeta).
    if (!m_customCategoryKey.isEmpty())
        return m_customCategoryKey;
    // X11 fallback: the custom group's key is the section key of any
    // type==Custom item (legacy items carry sectionKey == sectionName).
    for (const auto *info : m_infos) {
        if (info->type == Custom && !info->sectionKey.isEmpty())
            return info->sectionKey;
    }
    return QString();
}

QList<QString> ShortcutModel::groupOrder() const
{
    // Order by service-supplied order; keys not in the metadata (e.g. items
    // arriving before metadata) append by first-seen. The custom group is
    // already last via its high order value from the service.
    QList<QString> known, unknown;
    for (const auto &m : m_categoryMeta) {
        if (!m_groupInfos.contains(m.key))
            continue;
        known.append(m.key);
    }
    std::sort(known.begin(), known.end(),
              [this](const QString &a, const QString &b) {
        return m_categoryMeta.value(a).order < m_categoryMeta.value(b).order;
    });
    for (auto it = m_groupInfos.constBegin(); it != m_groupInfos.constEnd(); ++it) {
        if (!m_categoryMeta.contains(it.key()))
            unknown.append(it.key());
    }
    // Sort unknown keys for deterministic ordering across runs (QHash
    // iteration is seed-dependent). Known keys are already sorted above.
    unknown.sort();
    return known + unknown;
}

ShortcutInfo *ShortcutModel::currentInfo() const
{
    return m_currentInfo;
}

void ShortcutModel::setCurrentInfo(ShortcutInfo *currentInfo)
{
    m_currentInfo = currentInfo;
}

ShortcutInfo *ShortcutModel::findInfoIf(std::function<bool (ShortcutInfo *)> cb)
{
    auto res = std::find_if(m_infos.begin(), m_infos.end(), cb);
    if (res != m_infos.end()) {
        return *res;
    }

    return nullptr;
}

ShortcutInfo *ShortcutModel::getInfo(const QString &shortcut)
{
    if (shortcut.isEmpty())
        return nullptr;

    return findInfoIf([ = ] (const ShortcutInfo *info)->bool{
        return !QString::compare(info->accels, shortcut, Qt::CaseInsensitive); //判断是否相等，相等则返回0
    });
}

void ShortcutModel::setSearchResult(const QString &searchResult)
{
    qDeleteAll(m_searchList);
    m_searchList.clear();

    QList<ShortcutInfo *> systemInfoList;
    QList<ShortcutInfo *> windowInfoList;
    QList<ShortcutInfo *> workspaceInfoList;
    QList<ShortcutInfo *> customInfoList;
    QList<ShortcutInfo *> speechInfoList;

    QJsonArray array = QJsonDocument::fromJson(searchResult.toStdString().c_str()).array();
    for (auto value : array) {
        QJsonObject obj  = value.toObject();
        if (!obj["Section"].toString().isEmpty()) {
            ShortcutInfo *info = new ShortcutInfo();
            fillShortcutInfoFromJson(info, obj);
            m_searchList << info;
            continue;
        }

        int         type = obj["Type"].toInt();
        ShortcutInfo *info = new ShortcutInfo();
        info->type         = type;
        info->accels       = obj["Accels"].toArray().first().toString();
        info->name    = obj["Name"].toString();
        info->id      = obj["Id"].toString();
        info->command = obj["Exec"].toString();

        if (type != MEDIAKEY) {
            if (systemFilter.contains(info->id)) {
                systemInfoList << info;
                continue;
            }
            if (windowFilter.contains(info->id)) {
                windowInfoList << info;
                continue;
            }
            if (workspaceFilter.contains(info->id)) {
                workspaceInfoList << info;
                continue;
            }
            if (assistiveToolsFilter.contains(info->id)) {
                speechInfoList << info;
                continue;
            }

            if (type == 1) {
                customInfoList << info;
            }else{
                delete info;
                info = nullptr;
            }

        } else {
            qDebug() << "not search is:" << info->name;
            delete info;
            info = nullptr;
        }
    }

    std::sort(systemInfoList.begin(), systemInfoList.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return systemFilter.indexOf(s1->id) < systemFilter.indexOf(s2->id);
    });
    std::sort(windowInfoList.begin(), windowInfoList.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return windowFilter.indexOf(s1->id) < windowFilter.indexOf(s2->id);
    });
    std::sort(workspaceInfoList.begin(), workspaceInfoList.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return workspaceFilter.indexOf(s1->id) < workspaceFilter.indexOf(s2->id);
    });
    m_searchList.append(systemInfoList);
    m_searchList.append(windowInfoList);
    m_searchList.append(workspaceInfoList);
    m_searchList.append(speechInfoList);
    m_searchList.append(customInfoList);
    int i = 0;
    for (auto search : m_searchList) {
        qDebug() << "search" << ++i << " is: " << search->name;
    }

    Q_EMIT searchFinished(m_searchList);
}

bool ShortcutModel::searchResultContains(const QString &id)
{
    auto res = std::find_if(m_searchList.begin(), m_searchList.end(), [ = ] (const ShortcutInfo *info)->bool{
        return info->id == id;
    });

    return res != m_infos.end();
}

QStringList ShortcutModel::getSystemShortcutNames() const
{
    QStringList names;

    // 合并所有系统相关的快捷键列表
    QList<QList<ShortcutInfo *>> systemLists = {
        m_systemInfos,
        m_windowInfos,
        m_workspaceInfos,
        m_assistiveToolsInfos
    };

    for (const auto &list : systemLists) {
        for (const auto *info : list) {
            if (info && !info->name.trimmed().isEmpty()) {
                names.append(info->name.trimmed());
            }
        }
    }

    // Wayland: items live in m_groupInfos rather than the legacy fixed lists.
    // Collect names from all groups except the user-editable (Custom) one so
    // that name-conflict detection works for system/app/workspace shortcuts.
    if (names.isEmpty() && !m_groupInfos.isEmpty()) {
        const QString customKey = customCategoryKey();
        for (auto it = m_groupInfos.constBegin(); it != m_groupInfos.constEnd(); ++it) {
            if (!customKey.isEmpty() && it.key() == customKey)
                continue;
            for (const auto *info : it.value()) {
                if (info && !info->name.trimmed().isEmpty())
                    names.append(info->name.trimmed());
            }
        }
    }

    return names;
}

bool ShortcutModel::containsSystemShortcutName(const QString &name) const
{
    // 处理边界情况
    if (name.trimmed().isEmpty()) {
        return false;
    }
    
    // 使用成员变量缓存提高查找效率
    if (m_systemNamesCache.isEmpty()) {
        // 构建缓存
        const QStringList systemNames = getSystemShortcutNames();
        m_systemNamesCache = QSet<QString>(systemNames.begin(), systemNames.end());
    }
    
    return m_systemNamesCache.contains(name.trimmed());
}

void ShortcutModel::invalidateSystemShortcutNamesCache() const
{
    // 清空缓存，强制下次访问时重新构建
    m_systemNamesCache.clear();
}

ShortcutListModel::ShortcutListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

void ShortcutListModel::setSouceModel(ShortcutModel *model)
{
    if (m_model == model)
        return;
    m_model = model;
}

ShortcutModel *ShortcutListModel::souceModel()
{
    return m_model;
}

void ShortcutListModel::reset()
{
    beginResetModel();
    endResetModel();
}

void ShortcutListModel::onUpdateShortcut(ShortcutInfo *info)
{
    if (!m_model || !info)
        return;

    int row = m_model->indexOfShortcut(info);
    if (row >= 0)
    {
        QModelIndex modelIndex = index(row);
        Q_EMIT dataChanged(modelIndex, modelIndex, {
            Qt::DisplayRole,
            SearchedTextRole,
            KeySequenceRole,
            CommandRole,
            AccelsRole,
            SectionNameRole,
            SectionKeyRole
        });
    }
}

int ShortcutListModel::rowCount(const QModelIndex &) const
{
    if (!m_model)
        return 0;

    return m_model->count();
}

QVariant ShortcutListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_model->count())
        return QVariant();

    int corners = NoneCorner;
    ShortcutInfo *info = m_model->shortcutAt(index.row(), &corners);
    if (!info)
        return QVariant();

    auto displayKeys = ShortcutModel::formatKeys(info->accels);

    switch (role) {
    case Qt::DisplayRole:
        return info->name;
    case SearchedTextRole:
        return info->name + info->pinyin + FieldSeparator + displayKeys.join(FieldSeparator);
    case IdRole:
        return info->id;
    case TypeRole:
        return info->type;
    case KeySequenceRole:
        return displayKeys;
    case CommandRole:
        return info->command;
    case AccelsRole:
        return info->accels;
    case SectionNameRole:
        return info->sectionName;
    case SectionKeyRole:
        return info->sectionKey;
    case CornersRole:
        return corners;
    case IsCustomRole:
        return info->type == ShortcutModel::Custom;
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> ShortcutListModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names[SearchedTextRole] = "searchedText";
    names[IdRole] = "id";
    names[TypeRole] = "type";
    names[KeySequenceRole] = "keySequence";
    names[CommandRole] = "command";
    names[SectionNameRole] = "sectionName";
    names[SectionKeyRole] = "sectionKey";
    names[AccelsRole] = "accels";
    names[CornersRole] = "corners";
    names[IsCustomRole] = "isCustom";

    return names;
}
