#include "shortcutmodel.h"
#include <QThreadPool>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>
#include <QDBusInterface>

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

QList<ShortcutInfo *> ShortcutModel::infos() const
{
    return m_infos;
}

#include <QDebug>
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
            info->used = true;
        }
        else if(windowFilter.contains(info->id))
        {
            m_windowInfos.append(info);
            info->used = true;
        }
        else if(workspaceFilter.contains(info->id))
        {
            m_workspaceInfos.append(info);
            info->used = true;
        }
    }
    emit parseFinish();
}

void ShortcutModel::onAdded(const QString &in0, int in1)
{
    qDebug()<<Q_FUNC_INFO<<in0<<in1;
}

void ShortcutModel::onDeleted(const QString &in0, int in1)
{
    qDebug()<<Q_FUNC_INFO<<in0<<in1;
}

void ShortcutModel::onKeyEvent(bool in0, const QString &in1)
{
    qDebug()<<Q_FUNC_INFO<<in0<<in1;
}

ShortcutInfo::ShortcutInfo()
{
    used = false;
}
