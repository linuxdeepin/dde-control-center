//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
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

const QStringList &windowFilter = {"maximize",
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

using namespace dccV25;
DCORE_USE_NAMESPACE
ShortcutModel::ShortcutModel(QObject *parent)
    : QObject(parent)
    , m_windowSwitchState(false)
{
    if (qApp->screens().count() > 1) {
        systemFilter.append("switch-monitors");
    }
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

void ShortcutModel::delInfo(ShortcutInfo *info)
{
    if (m_infos.contains(info)) {
        m_infos.removeOne(info);
    }
    if (m_customInfos.contains(info)) {
        m_customInfos.removeOne(info);
    }

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
    } else if (false == m_windowSwitchState) {
        QStringList systemFilterServer = systemFilter;
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
        info->name    = obj["Name"].toString();
        info->id      = obj["Id"].toString();
        info->command = obj["Exec"].toString();

        m_infos << info;

        if (type != MEDIAKEY) {
            if (systemShortKeys.contains(info->id)) {
                m_systemInfos << info;
                continue;
            }
            if (windowFilter.contains(info->id)) {
                m_windowInfos << info;
                continue;
            }
            if (workspaceFilter.contains(info->id)) {
                m_workspaceInfos << info;
                continue;
            }
            if (assistiveToolsFilter.contains(info->id)) {
                m_assistiveToolsInfos << info;
                continue;
            }
            if (type == 1) {
                m_customInfos << info;
            }
        }
    }

    std::sort(m_systemInfos.begin(), m_systemInfos.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return systemShortKeys.indexOf(s1->id) < systemShortKeys.indexOf(s2->id);
    });

    std::sort(m_windowInfos.begin(), m_windowInfos.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return windowFilter.indexOf(s1->id) < windowFilter.indexOf(s2->id);
    });

    std::sort(m_workspaceInfos.begin(), m_workspaceInfos.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return workspaceFilter.indexOf(s1->id) < workspaceFilter.indexOf(s2->id);
    });

    std::sort(m_assistiveToolsInfos.begin(), m_assistiveToolsInfos.end(), [ = ](ShortcutInfo *s1, ShortcutInfo *s2) {
        return assistiveToolsFilter.indexOf(s1->id) < assistiveToolsFilter.indexOf(s2->id);
    });

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
    info->id      = obj["Id"].toString();
    info->command = obj["Exec"].toString();
    m_infos.append(info);
    m_customInfos.append(info);
    Q_EMIT addCustomInfo(info);
}

void ShortcutModel::onKeyBindingChanged(const QString &value)
{
    const QJsonObject &obj       = QJsonDocument::fromJson(value.toStdString().c_str()).object();
    const QString     &update_id = obj["Id"].toString();
    auto res = std::find_if(m_infos.begin(), m_infos.end(), [ = ] (const ShortcutInfo *info)->bool{
        return info->id == update_id;
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
    }
}

 bool ShortcutModel::getWindowSwitch()
 {
     return m_windowSwitchState;
 }

ShortcutInfo *ShortcutModel::currentInfo() const
{
    return m_currentInfo;
}

void ShortcutModel::setCurrentInfo(ShortcutInfo *currentInfo)
{
    m_currentInfo = currentInfo;
}

ShortcutInfo *ShortcutModel::getInfo(const QString &shortcut)
{
    auto res = std::find_if(m_infos.begin(), m_infos.end(), [ = ] (const ShortcutInfo *info)->bool{
        return !QString::compare(info->accels, shortcut, Qt::CaseInsensitive); //判断是否相等，相等则返回0
    });

    if (res != m_infos.end()) {
        return *res;
    }

    return nullptr;
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
