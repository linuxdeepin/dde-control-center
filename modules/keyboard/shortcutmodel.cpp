#include "shortcutmodel.h"
#include "shortcutitem.h"
#include <QThreadPool>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QDBusInterface>
#include <QDebug>

namespace dcc
{
namespace keyboard
{

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

    QStringList systemFilter;
    systemFilter << "terminal" << "terminal-quake" << "screenshot" << "screenshot-delayed" << "screenshot-window" << "deepin-screen-recorder"
                 << "switch-group" << "switch-group-backward" << "preview-workspace" << "expose-windows" << "expose-all-windows"
                 << "launcher" << "switch-applications" << "switch-applications-backward" << "show-desktop" << "file-manager"
                 << "lock-screen" << "logout" << "switch-layout" << "wm-switcher";

    QStringList windowFilter;
    windowFilter  << "maximize" << "unmaximize" << "minimize" << "begin-move" << "begin-resize" << "close";

    QStringList workspaceFilter;
    workspaceFilter << "switch-to-workspace-left" << "switch-to-workspace-right"
                    << "move-to-workspace-left" << "move-to-workspace-right";

    QJsonArray array = QJsonDocument::fromJson(info.toStdString().c_str()).array();

    for (const QString i : systemFilter) {
        foreach(QJsonValue value, array) {
            QJsonObject obj = value.toObject();
            int type = obj["Type"].toInt();
            if (type != MEDIAKEY && obj["Id"].toString() == i) {
                ShortcutInfo *info = new ShortcutInfo();
                info->type = type;
                QString accels = obj["Accels"].toArray().at(0).toString();
                if (accels.isEmpty()) {
                    accels = tr("None");
                }
                info->accels = accels;

                info->name = obj["Name"].toString();
                info->id = obj["Id"].toString();
                m_infos.append(info);
                m_systemInfos.append(info);
                break;
            }
        }
    }

    for (const QString i : windowFilter) {
        foreach(QJsonValue value, array) {
            QJsonObject obj = value.toObject();
            int type = obj["Type"].toInt();
            if (type != MEDIAKEY && obj["Id"].toString() == i) {
                ShortcutInfo *info = new ShortcutInfo();
                info->type = type;
                QString accels = obj["Accels"].toArray().at(0).toString();
                if (accels.isEmpty()) {
                    accels = tr("None");
                }
                info->accels = accels;

                info->name = obj["Name"].toString();
                info->id = obj["Id"].toString();
                m_infos.append(info);
                m_windowInfos.append(info);
                break;
            }
        }
    }

    for (const QString i : workspaceFilter) {
        foreach(QJsonValue value, array) {
            QJsonObject obj = value.toObject();
            int type = obj["Type"].toInt();
            if (type != MEDIAKEY && obj["Id"].toString() == i) {
                ShortcutInfo *info = new ShortcutInfo();
                info->type = type;
                QString accels = obj["Accels"].toArray().at(0).toString();
                if (accels.isEmpty()) {
                    accels = tr("None");
                }
                info->accels = accels;

                info->name = obj["Name"].toString();
                info->id = obj["Id"].toString();
                m_infos.append(info);
                m_workspaceInfos.append(info);
                break;
            }
        }
    }

    foreach(QJsonValue value, array) {
        QJsonObject obj = value.toObject();
        int type = obj["Type"].toInt();
        if (type != MEDIAKEY && type == 1) {
            ShortcutInfo *info = new ShortcutInfo();
            info->type = type;
            QString accels = obj["Accels"].toArray().at(0).toString();
            if (accels.isEmpty()) {
                accels = tr("None");
            }
            info->accels = accels;

            info->name = obj["Name"].toString();
            info->id = obj["Id"].toString();
            info->command = obj["Exec"].toString();
            m_infos.append(info);
            m_customInfos.append(info);
        }
    }

    emit listChanged(m_systemInfos, InfoType::System);
    emit listChanged(m_windowInfos, InfoType::Window);
    emit listChanged(m_workspaceInfos, InfoType::Workspace);
    emit listChanged(m_customInfos, InfoType::Custom);
}

void ShortcutModel::onCustomInfo(const QString &json)
{
    QJsonObject obj = QJsonDocument::fromJson(json.toStdString().c_str()).object();
    ShortcutInfo *info = new ShortcutInfo();
    info->type = obj["Type"].toInt();
    QString accels = obj["Accels"].toArray().at(0).toString();
    if (accels.isEmpty()) {
        accels = tr("None");
    }
    info->accels = accels;

    info->name = obj["Name"].toString();
    info->id = obj["Id"].toString();
    info->command = obj["Exec"].toString();
    m_infos.append(info);
    m_customInfos.append(info);
    emit addCustomInfo(info);
}

ShortcutInfo::ShortcutInfo()
{
    item = NULL;
}

}
}
