#include "shortcutmodel.h"
#include "shortcutitem.h"
#include <QThreadPool>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QDBusInterface>
#include <QDebug>

namespace dcc {
namespace keyboard{

ShortcutModel::ShortcutModel(QObject *parent) : QObject(parent)
{

}

ShortcutModel::~ShortcutModel()
{
    m_systemInfos.clear();
    m_windowInfos.clear();
    m_workspaceInfos.clear();
    qDeleteAll(m_infos.begin(), m_infos.end());
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
    if(m_infos.contains(info))
    {
        m_infos.removeOne(info);
    }
    if(m_customInfos.contains(info))
    {
        m_customInfos.removeOne(info);
    }

    delete info;
}

void ShortcutModel::onParseInfo(const QString &info)
{
    m_systemInfos.clear();
    m_windowInfos.clear();
    m_workspaceInfos.clear();
    qDeleteAll(m_infos.begin(), m_infos.end());

    QStringList systemFilter;
    systemFilter << "launcher" << "show-desktop" << "lock-screen" << "file-manager" << "screenshot"
         << "screenshot-window" << "screenshot-delayed"
         << "terminal"
        << "terminal-quake"
        << "logout" << "switch-layout"
        << "preview-workspace" << "expose-windows" << "expose-all-windows"
        << "switch-group" << "switch-group-backward" << "switch-applications"
        << "switch-applications-backward" << "wm-switcher"
        << "switch-applications-backward";

//    QDBusInterface dbus("com.deepin.daemon.InputDevice.InputDevices",
//                        "/com/deepin/daemon/InputDevice/TouchPad",
//                        "com.deepin.daemon.InputDevice.TouchPad");
//    if(dbus.property("Exist").toBool())
//        systemFilter << "disable-touchpad";

    QStringList windowFilter;
    windowFilter << "close" << "maximize" << "unmaximize" << "begin-move"
             << "begin-resize";

    QStringList workspaceFilter;
    workspaceFilter << "switch-to-workspace-left" << "switch-to-workspace-right"
             << "move-to-workspace-left" << "move-to-workspace-right";

    QJsonArray array = QJsonDocument::fromJson(info.toStdString().c_str()).array();
    foreach(QJsonValue value, array)
    {
        ShortcutInfo *info = new ShortcutInfo();

        QJsonObject obj = value.toObject();
        info->type = obj["Type"].toInt();
        QString accels = obj["Accels"].toArray().at(0).toString();
        if(accels.isEmpty())
            accels = tr("None");
        info->accels = accels;

        info->name = obj["Name"].toString();
        info->id = obj["Id"].toString();
        m_infos.append(info);
        if(systemFilter.contains(info->id))
        {
            m_systemInfos.append(info);
        }
        else if(windowFilter.contains(info->id))
        {
            m_windowInfos.append(info);
        }
        else if(workspaceFilter.contains(info->id))
        {
            m_workspaceInfos.append(info);
        }
        else if(info->type == 1)
        {
            m_customInfos.append(info);
        }
    }
    emit parseFinish();
}

void ShortcutModel::onCustomInfo(const QString &json)
{
    QJsonObject obj = QJsonDocument::fromJson(json.toStdString().c_str()).object();
    ShortcutInfo *info = new ShortcutInfo();
    info->type = obj["Type"].toInt();
    QString accels = obj["Accels"].toArray().at(0).toString();
    if(accels.isEmpty())
        accels = tr("None");
    info->accels = accels;

    info->name = obj["Name"].toString();
    info->id = obj["Id"].toString();
    m_infos.append(info);
    m_customInfos.append(info);
    emit addCustonInfo(info);
}

ShortcutInfo::ShortcutInfo()
{
    item = NULL;
}

}
}
