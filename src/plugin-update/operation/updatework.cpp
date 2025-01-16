// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "updatework.h"

#include <qdbuspendingreply.h>
#include <DDBusSender>
#include <DGuiApplicationHelper>

UpdateWork::UpdateWork(UpdateModel* model, QObject *parent)
    : QObject{ parent }
    , m_model(model)
    , m_updateInterface(new QDBusInterface("org.deepin.UpdateManager1", "/org/deepin/UpdateManager1", "org.deepin.UpdateManager1", QDBusConnection::systemBus()))
{
    initData();
    initConnection();
}

void UpdateWork::checkUpgrade()
{
    m_model->setCheckUpdateState(UpdateModel::CheckUpdateState::checking);
    m_model->setCheckProcessRunning(true);
    QDBusPendingCall call = m_updateInterface->asyncCall("checkUpgrade");
    QDBusPendingCallWatcher *watcherCheckUpdate = new QDBusPendingCallWatcher(call, this);
    connect(watcherCheckUpdate, &QDBusPendingCallWatcher::finished, this,[this] (QDBusPendingCallWatcher* watcherCheckUpdate) {
        if (watcherCheckUpdate == nullptr) {
            return;
        }
        if (watcherCheckUpdate->isError()) {
            qDebug() << "checkUpgrade dbus error :"<< watcherCheckUpdate->error().message();
            m_model->setLastCheckUpdateErrorMsg(watcherCheckUpdate->error().message());
            m_model->setCheckUpdateState(UpdateModel::CheckUpdateState::checkFailed);
        } else {
            m_model->setLastCheckUpdateErrorMsg("");
            m_model->setCheckUpdateState(UpdateModel::CheckUpdateState::checked);
        }

        delete watcherCheckUpdate;
        watcherCheckUpdate = nullptr;
    });
}

void UpdateWork::onActionBtnClicked()
{
    qInfo() << " onActionBtnClicked : " << m_model->getUpdateState();
    if (m_model->getUpdateState() == "success") {
        if (Dtk::Gui::DGuiApplicationHelper::testAttribute(Dtk::Gui::DGuiApplicationHelper::IsWaylandPlatform)) {
            QStringList notificationActions;
            notificationActions.append("reboot");
            notificationActions.append(tr("Restart Now"));
            notificationActions.append("cancel");
            notificationActions.append(tr("Restart Later"));
            QVariantMap map;
            map.insert("deepin-dde-shell-action-reboot", "{\"pluginId\":\"org.deepin.ds.dde-shutdown\",\"method\":\"requestShutdown\",\"arguments\":[\"Restart\" ]}");
            QDBusMessage message = QDBusMessage::createMethodCall("org.deepin.dde.Notification1", "/org/deepin/dde/Notification1", "org.deepin.dde.Notification1", "Notify");
            message << QString("dde-control-center") << static_cast<uint>(0)
                    << QString("preferences-system") << QString(tr("System update completed"))
                    << QString(tr("Determine whether to restart.")) << notificationActions
                    << map << 0;
            QDBusConnection::sessionBus().asyncCall(message);
        } else {
            DDBusSender()
                    .service("com.deepin.dde.shutdownFront")
                    .interface("com.deepin.dde.shutdownFront")
                    .path("/com/deepin/dde/shutdownFront")
                    .method("Restart")
                    .call();
        }
        return;
    }

    upgrade();
}

void UpdateWork::upgrade()
{
    m_updateInterface->asyncCall("upgrade");
}

void UpdateWork::showUpdateCtl(bool isShow)
{
    m_model->setShowUpdateCtl(isShow && m_model->checkUpdateState() == UpdateModel::CheckUpdateState::checked);
}

void UpdateWork::checkProcessStop()
{
    m_model->setCheckProcessRunning(false);
}

void UpdateWork::onPropertiesChanged(const QDBusMessage &message)
{
    QList<QVariant> arguments = message.arguments();
    if (3 != arguments.count()) {
        return;
    }
    QString interfaceName = message.arguments().at(0).toString();
    qInfo()  << " onPropertiesChanged interfaceName:  " << interfaceName;
    if (interfaceName == UpdateManagerInterface) {
        QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
        QStringList keys = changedProps.keys();
        for (int i = 0; i < keys.size(); i++) {
            qInfo() << "org.deepin.UpdateManager1 key : " << keys.at(i);
            if (keys.at(i) == "upgradable") {
                qInfo() << "upgradable value  : "  << changedProps.values().at(i).toBool();
                m_model->setUpgradable(changedProps.values().at(i).toBool());
            } else if (keys.at(i) == "state") {
                qInfo() << "state value : "  << changedProps.values().at(i).toString();
                m_model->setUpdateState(changedProps.values().at(i).toString());
            }
        }
    }
}

void UpdateWork::onProcess(QPair<QString, double> processValue)
{
}

void UpdateWork::initData()
{
    bool upgradable = m_updateInterface->property("upgradable").toBool();
    QString state = m_updateInterface->property("state").toString();

    qDebug() << " init  upgradable , state " << upgradable << state;
    m_model->setUpgradable(upgradable);
    m_model->setUpdateStateTips(tr("An update has been detected."));
    m_model->setUpdateState(state);
}

void UpdateWork::initConnection()
{
    QDBusConnection::systemBus().connect(UpdateManagerService, UpdateManagerPath, UpdateManagerInterface, "progress",  this, SLOT(onProcess(QPair<QString,double>)));
    QDBusConnection::systemBus().connect(UpdateManagerService, UpdateManagerPath, PropertiesInterface, PropertiesChanged, "sa{sv}as", this, SLOT(onPropertiesChanged(QDBusMessage)));
}
