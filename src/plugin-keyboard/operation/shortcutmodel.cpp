//SPDX-FileCopyrightText: 2018 - 2024 UnionTech Software Technology Co., Ltd.
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
                            "terminal-quake",
                            "global-search",
                            "screenshot",
                            "screenshot-delayed",
                            "screenshot-fullscreen",
                            "screenshot-window",
                            "screenshot-scroll",
                            "screenshot-ocr",
                            "deepin-screen-recorder",
                            "switch-group",
                            "switch-group-backward",
                            "preview-workspace",
                            "launcher",
                            "switch-applications",
                            "switch-applications-backward",
                            "show-desktop",
                            "file-manager",
                            "lock-screen",
                            "logout",
                            "wm-switcher",
                            "system-monitor",
                            "color-picker",
                            "clipboard",
                            "switch-monitors"
};

const QStringList &windowFilter = {"maximize",
                                   "unmaximize",
                                   "minimize",
                                   "begin-move",
                                   "begin-resize",
                                   "close",
                                   "toggle-to-left",
                                   "toggle-to-right"
};

const QStringList &workspaceFilter = {"switch-to-workspace-left",
                                      "switch-to-workspace-right",
                                      "move-to-workspace-left",
                                      "move-to-workspace-right"};

const QStringList &assistiveToolsFilter = {"text-to-speech",
                                           "speech-to-text",
                                           "translation",
                                            "view-zoom-in",
                                            "view-zoom-out",
                                            "view-actual-size"};

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
    return pinyin(name, TS_NoneTone).join("_") + "_" + firstLetters(name).join("_");
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

QList<ShortcutInfo *> ShortcutModel::customInfo() const
{
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

    auto getCorners = [](QList<ShortcutInfo *>&list, int index) {
        if (index == 0)
            return TopCorner;
        else if (index == list.count() - 1)
            return BottomCorner;
        else
            return NoneCorner;
    };

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

    Q_EMIT delCustomInfo(info);

    delete info;
    info = nullptr;
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
    m_customInfos.clear();

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

        if (type != MEDIAKEY) {
            if (systemShortKeys.contains(info->id)) {
                info->sectionName = tr("System");
                m_systemInfos << info;
                continue;
            }
            if (windowFilter.contains(info->id)) {
                info->sectionName = tr("Window");
                m_windowInfos << info;
                continue;
            }
            if (workspaceFilter.contains(info->id)) {
                info->sectionName = tr("Workspace");
                m_workspaceInfos << info;
                continue;
            }
            if (assistiveToolsFilter.contains(info->id)) {
                info->sectionName = tr("AssistiveTools");
                m_assistiveToolsInfos << info;
                continue;
            }
            if (type == 1) {
                info->sectionName = tr("Custom");
                m_customInfos << info;
            }
        }
    }

    std::sort(m_systemInfos.begin(), m_systemInfos.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return systemShortKeys.indexOf(s1->id) < systemShortKeys.indexOf(s2->id);
    });
    // More efficient implementation using std::find_if
    auto it = std::find_if(m_systemInfos.begin(), m_systemInfos.end(),
        [](const ShortcutInfo* info) { return info->id == "preview-workspace"; });
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
    Q_EMIT listChanged(m_customInfos, InfoType::Custom);
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

    m_infos.append(info);
    m_customInfos.append(info);
    Q_EMIT addCustomInfo(info);
}

void ShortcutModel::onKeyBindingChanged(const QString &value)
{
    const QJsonObject &obj       = QJsonDocument::fromJson(value.toStdString().c_str()).object();
    const QString     &update_id = obj["Id"].toString();
    const int     &update_type = obj["Type"].toInt();
    auto res = std::find_if(m_infos.begin(), m_infos.end(), [ = ] (const ShortcutInfo *info)->bool{
        return info->id == update_id && info->type == update_type;
    });

    if (res != m_infos.end()) {
        (*res)->type = obj["Type"].toInt();
        (*res)->accels  = obj["Accels"].toArray().first().toString();
        (*res)->name    = obj["Name"].toString();
        (*res)->command = obj["Exec"].toString();

        Q_EMIT shortcutChanged((*res));
    }
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
        return info->name + info->pinyin + "_" + displayKeys.join("_");
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
    names[SectionNameRole] = "section";
    names[AccelsRole] = "accels";
    names[CornersRole] = "corners";
    names[IsCustomRole] = "isCustom";

    return names;
}
