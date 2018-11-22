/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "shortcutmodel.h"
#include <QDBusInterface>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QThreadPool>
#include "shortcutitem.h"

static const QStringList systemFilter = { "terminal",
                                          "terminal-quake",
                                          "screenshot",
                                          "screenshot-delayed",
                                          "screenshot-fullscreen",
                                          "screenshot-window",
                                          "deepin-screen-recorder",
                                          "switch-group",
                                          "switch-group-backward",
                                          "preview-workspace",
                                          "expose-windows",
                                          "expose-all-windows",
                                          "launcher",
                                          "switch-applications",
                                          "switch-applications-backward",
                                          "show-desktop",
                                          "file-manager",
                                          "lock-screen",
                                          "logout",
                                          "wm-switcher",
                                          "system-monitor",
                                          "color-picker" };

QStringList windowFilter = { "maximize",   "unmaximize",   "minimize",
                             "begin-move", "begin-resize", "close" };

QStringList workspaceFilter = { "switch-to-workspace-left", "switch-to-workspace-right",
                                "move-to-workspace-left", "move-to-workspace-right" };

namespace dcc {
namespace keyboard {

ShortcutModel::ShortcutModel(QObject *parent) : QObject(parent)
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
}

void ShortcutModel::onParseInfo(const QString &info)
{
    qDeleteAll(m_infos);

    m_infos.clear();
    m_systemInfos.clear();
    m_windowInfos.clear();
    m_workspaceInfos.clear();
    m_customInfos.clear();

    QJsonArray array = QJsonDocument::fromJson(info.toStdString().c_str()).array();

    Q_FOREACH (QJsonValue value, array) {
        QJsonObject obj  = value.toObject();
        int         type = obj["Type"].toInt();

        ShortcutInfo *info = new ShortcutInfo();
        info->type         = type;
        info->accels       = obj["Accels"].toArray().first().toString();
        ;
        info->name    = obj["Name"].toString();
        info->id      = obj["Id"].toString();
        info->command = obj["Exec"].toString();

        m_infos << info;

        if (type != MEDIAKEY) {
            if (systemFilter.contains(info->id)) {
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

            if (type == 1) m_customInfos << info;
        }
    }

    qSort(m_systemInfos.begin(), m_systemInfos.end(), [=] (ShortcutInfo *s1, ShortcutInfo *s2) {
        return systemFilter.indexOf(s1->id) < systemFilter.indexOf(s2->id);
    });

    qSort(m_windowInfos.begin(), m_windowInfos.end(), [=] (ShortcutInfo *s1, ShortcutInfo *s2) {
        return windowFilter.indexOf(s1->id) < windowFilter.indexOf(s2->id);
    });

    qSort(m_workspaceInfos.begin(), m_workspaceInfos.end(),
          [=] (ShortcutInfo *s1, ShortcutInfo *s2) {
              return workspaceFilter.indexOf(s1->id) < workspaceFilter.indexOf(s2->id);
          });

    Q_EMIT listChanged(m_systemInfos, InfoType::System);
    Q_EMIT listChanged(m_windowInfos, InfoType::Window);
    Q_EMIT listChanged(m_workspaceInfos, InfoType::Workspace);
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
    const QString &    update_id = obj["Id"].toString();

    for (ShortcutInfo *info : m_infos) {
        if (info->id == update_id) {
            info->type = obj["Type"].toInt();
            ;
            info->accels  = obj["Accels"].toArray().first().toString();
            info->name    = obj["Name"].toString();
            info->command = obj["Exec"].toString();

            Q_EMIT shortcutChanged(info);
            break;
        }
    }
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
    for (ShortcutInfo *info : m_infos) {
        if (QString::compare(info->accels, shortcut, Qt::CaseInsensitive) == 0) return info;
    }

    return nullptr;
}

ShortcutInfo::ShortcutInfo()
{
    item = NULL;
}
}
}
