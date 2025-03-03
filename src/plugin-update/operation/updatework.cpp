// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#include "updatework.h"

#include <qdbuspendingreply.h>
#include <qdbusreply.h>

#include <DDBusSender>

#include <QJsonDocument>
#include <QJsonObject>

UpdateWork::UpdateWork(UpdateModel* model, QObject *parent)
    : QObject{ parent }
    , m_model(model)
    , m_updateInter(new UpdateDBusProxy(this))
    , m_updateInterface(new QDBusInterface("org.deepin.UpdateManager1", "/org/deepin/UpdateManager1", "org.deepin.UpdateManager1", QDBusConnection::systemBus()))
    , m_distUpgradeJob(nullptr)
    , m_checkUpdateJob(nullptr)
{
    initData();
    initConnection();
}

void UpdateWork::checkUpgrade()
{
    QDBusPendingCall call = m_updateInter->UpdateSource();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] (QDBusPendingCallWatcher* watcher){
        if (!call.isError()) {
            QDBusReply<QDBusObjectPath> reply = call.reply();
            const QString jobPath = reply.value().path();
            setCheckUpdatesJob(jobPath);
        } else {
            qDebug() << "checkUpgrade dbus error :"<< watcher->error().message();
            m_model->setLastCheckUpdateErrorMsg(watcher->error().message());
            m_model->setCheckUpdateState(UpdateModel::CheckUpdateState::checkFailed);
        }
    });
}

void UpdateWork::setCheckUpdatesJob(const QString &jobPath)
{
    createCheckUpdateJob(jobPath);
}

void UpdateWork::createCheckUpdateJob(const QString &jobPath)
{
    if (m_checkUpdateJob != nullptr) {
        return;
    }

    qDebug() << "createCheckUpdateJob : " << jobPath;
    m_checkUpdateJob = new UpdateJobDBusProxy(jobPath, this);

    connect(m_checkUpdateJob,
            &UpdateJobDBusProxy::StatusChanged,
            this,
            &UpdateWork::onCheckUpdateStatusChanged);

    connect(m_checkUpdateJob,
            &UpdateJobDBusProxy::ProgressChanged,
            m_model,
            &UpdateModel::setCheckUpdateProgress,
            Qt::QueuedConnection);

    m_checkUpdateJob->ProgressChanged(m_checkUpdateJob->progress());
    m_checkUpdateJob->StatusChanged(m_checkUpdateJob->status());
}

void UpdateWork::createDistUpgradeJob(const QString &jobPath)
{

    if (m_distUpgradeJob != nullptr) {
        return;
    }
    qDebug() << "createDistUpgradeJob : ============= " << jobPath;
    m_distUpgradeJob = new UpdateJobDBusProxy(jobPath, this);

    connect(m_distUpgradeJob,
            &UpdateJobDBusProxy::StatusChanged,
            this,
            &UpdateWork::onDistUpgradeStatusChanged);

    connect(m_distUpgradeJob,
            &UpdateJobDBusProxy::ProgressChanged,
            m_model,
            &UpdateModel::setDistUpgradeProgress,
            Qt::QueuedConnection);

    m_distUpgradeJob->ProgressChanged(m_distUpgradeJob->progress());
    m_distUpgradeJob->StatusChanged(m_distUpgradeJob->status());
}

void UpdateWork::onCheckUpdateStatusChanged(const QString &value)
{
    qDebug() << "[setCheckUpdatesJob]status is: " << value;
    if (value == "failed" || value.isEmpty()) {
        qWarning() << "check for updates job failed";
        if (m_checkUpdateJob != nullptr) {
            m_updateInter->CleanJob(m_checkUpdateJob->id());
            deleteJob(m_checkUpdateJob);
        }
        m_model->setCheckUpdateState(UpdateModel::CheckUpdateState::checkFailed);
    } else if (value == "running" || value == "ready") {
        m_model->setCheckUpdateState(UpdateModel::CheckUpdateState::checking);
    } else if (value == "success" || value == "succeed") {
        m_model->setCheckUpdateState(UpdateModel::CheckUpdateState::checked);
    } else if (value == "end") {
        deleteJob(m_checkUpdateJob);
        m_model->setShowUpdateCtl(m_model->upgradable());
    }
}

void UpdateWork::onDistUpgradeStatusChanged(const QString &value)
{
    qDebug() << "[setDistUpgradeJob]status is: " << value;
    if (value == "failed" || value.isEmpty()) {
        qWarning() << "check for updates job failed";
        if (m_distUpgradeJob != nullptr) {
            m_updateInter->CleanJob(m_distUpgradeJob->id());
            deleteJob(m_distUpgradeJob);
        }
        m_model->setDistUpgradeState(UpdateModel::job_failed);
    } else if (value == "ready") {
        m_model->setDistUpgradeState(UpdateModel::job_ready);
    } else if (value == "running") {
        m_model->setDistUpgradeState(UpdateModel::job_running);
    } else if (value == "success" || value == "succeed") {
        m_model->setDistUpgradeState(UpdateModel::job_successd);
    } else if (value == "end") {
        deleteJob(m_distUpgradeJob);
    }
}

void UpdateWork::deleteJob(UpdateJobDBusProxy* dbusJob)
{
    if (!dbusJob) {
        delete dbusJob;
        dbusJob = nullptr;
    }
}

void UpdateWork::onActionBtnClicked()
{
    qInfo() << " onActionBtnClicked : " << m_model->getUpdateState();
    if (m_model->distUpgradeState() == UpdateModel::UpdateJobState::job_successd) {
        DDBusSender()
            .service("com.deepin.dde.shutdownFront")
            .interface("com.deepin.dde.shutdownFront")
            .path("/com/deepin/dde/shutdownFront")
            .method("Restart")
            .call();
        return;
    }

    upgrade();
}

void UpdateWork::upgrade()
{
    QDBusPendingCall call = m_updateInter->DistUpgrade();
    QDBusPendingCallWatcher *watcher =
            new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] (QDBusPendingCallWatcher* watcher){
        if (!call.isError()) {
            QDBusReply<QDBusObjectPath> reply = call.reply();
            const QString jobPath = reply.value().path();
            createDistUpgradeJob(jobPath);
        } else {
            qDebug() << "upgrade dbus error :"<< watcher->error().message();
            m_model->setDistUpgradeState(UpdateModel::job_failed);
        }
    });
}

void UpdateWork::showUpdateCtl(bool isShow)
{
    m_model->setShowUpdateCtl(isShow && m_model->checkUpdateState() == UpdateModel::CheckUpdateState::checked);
}

void UpdateWork::checkProcessStop()
{
    m_model->setCheckProcessRunning(false);
}

void UpdateWork::setSmartMirror(bool enable)
{
    m_updateInter->SetEnable(enable);
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
    Q_UNUSED(processValue); // Mark as unused
}

void UpdateWork::onUpdateStatusChanged(const QString &updateStatus)
{
    m_model->setUpdateStatus(parseUpdateStatus(updateStatus));
}

UpdateStatus* UpdateWork::parseUpdateStatus(const QString &updateStatus)
{
    UpdateStatus* status = new UpdateStatus(this); // Create a UpdateStatus instance on the heap

    QJsonDocument jsonDoc = QJsonDocument::fromJson(updateStatus.toUtf8());
    if (!jsonDoc.isNull() && jsonDoc.isObject()) {
        QJsonObject jsonObj = jsonDoc.object();

        status->setABStatus(jsonObj["ABStatus"].toString());
        status->setABError(jsonObj["ABError"].toString());
        status->setTriggerBackingUpType(jsonObj["TriggerBackingUpType"].toInt());
        status->setTriggerBackupFailedType(jsonObj["TriggerBackupFailedType"].toInt());

        UpdateStatusData* statusData = status->statusData();
        QJsonObject statusDataObj = jsonObj["UpdateStatus"].toObject();
        if (statusData) {
            statusData->setSecurityUpgrade(statusDataObj["security_upgrade"].toString());
            statusData->setSystemUpgrade(statusDataObj["system_upgrade"].toString());
            statusData->setUnknowUpgrade(statusDataObj["unknown_upgrade"].toString());
            qDebug() << "Parsed statusData:" << statusData->unknowUpgrade() << statusData->systemUpgrade() << statusData->securityUpgrade();
        }

        qDebug() << "Parsed UpdateStatus:" << status->ABStatus() << status->ABError();
    } else {
        qWarning() << "Failed to parse updateStatus JSON";
    }

    return status; // Return the pointer
}

void UpdateWork::initData()
{
    onJobListChanged(m_updateInter->jobList());
    QString updateStatus = m_updateInter->updateStatus();
    qDebug() << "UpdateWork initData: " << updateStatus;

    UpdateStatus* status = parseUpdateStatus(updateStatus); // Get the instance by pointer
    if (status) {
        m_model->setUpdateStatus(status); // Example: Set it in the model using dereference
    } else {
        // Handle the case where status is nullptr
        qWarning() << "Failed to obtain valid UpdateStatus";
    }

    UpdateStatusData* statusData = status->statusData();
    m_model->setShowUpdateCtl(statusData->securityUpgrade() == "upgrading" || statusData->systemUpgrade() == "upgrading" || statusData->systemUpgrade() == "upgrading");

    m_model->updateDistUpgraedUI();
    m_model->setSmartMirrorSwitch(m_updateInter->enable());
}

void UpdateWork::initConnection()
{
    connect(m_updateInter, &UpdateDBusProxy::UpdateStatusChanged, this, &UpdateWork::onUpdateStatusChanged);
    connect(m_updateInter, &UpdateDBusProxy::EnableChanged, m_model, &UpdateModel::setSmartMirrorSwitch);
    connect(m_updateInter, &UpdateDBusProxy::JobListChanged, this, &UpdateWork::onJobListChanged);
}

void UpdateWork::onJobListChanged(const QList<QDBusObjectPath> &jobs)
{
    for (const auto &job : jobs) {
        m_jobPath = job.path();

        UpdateJobDBusProxy jobInter(m_jobPath, this);

        const QString &id = jobInter.id();
        if (!jobInter.isValid())
            continue;

        qDebug() << " onJobListChanged, id : " << id
                               << " , m_jobPath : " << m_jobPath;
        if ((id == "update_source" || id == "custom_update") && m_checkUpdateJob == nullptr) {
            setCheckUpdatesJob(m_jobPath);
        } else if (id == "dist_upgrade" && m_distUpgradeJob == nullptr) {
            createDistUpgradeJob(m_jobPath);
        }
    }
}
