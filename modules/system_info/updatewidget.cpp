/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "updatewidget.h"
#include "separatorwidget.h"
#include "constants.h"
#include "dbus/appupdateinfo.h"
#include "dbus/dbusupdatejob.h"

#include <QVBoxLayout>
#include <QResizeEvent>
#include <QScrollArea>
#include <QDBusPendingCallWatcher>
#include <QMap>

#include <dseparatorhorizontal.h>
#include <dthememanager.h>

UpdateWidget::UpdateWidget(QWidget *parent)
    : QWidget(parent)
{

    D_THEME_INIT_WIDGET(UpdateWidget);

    MirrorInfo::registerMetaType();
    AppUpdateInfo::registerMetaType();

    m_tipsWidget = new QWidget;
    m_tipsWidget->setVisible(false);
    m_updateStatTips = new QLabel;
    m_updateStatTips->setObjectName("Tips");
    m_updateCountTips = new QLabel;
    m_updateCountTips->setWordWrap(true);
    m_updateCountTips->setObjectName("Tips");
    m_updateCountTips->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    m_updateSizeTips = new QLabel;
    m_updateSizeTips->setObjectName("Tips");
    m_checkUpdateBtn = new DImageButton;
    m_checkUpdateBtn->setNormalPic(":/images/images/check_update_normal.svg");
    m_checkUpdateBtn->setHoverPic(":/images/images/check_update_hover.svg");
    m_checkUpdateBtn->setPressPic(":/images/images/check_update_press.svg");
    m_checkUpdateBtn->setStyleSheet("background-color:transparent;");
    m_checkingIndicator = new DLoadingIndicator;
    m_checkingIndicator->setWidgetSource(m_checkUpdateBtn);
    m_checkingIndicator->setFixedSize(32, 32);
    m_checkingIndicator->setSmooth(true);
    m_updateButton = new DImageButton;
    m_updateButton->setNormalPic(":/images/images/upgrade_normal.png");
    m_updateButton->setHoverPic(":/images/images/upgrade_hover.png");
    m_updateButton->setPressPic(":/images/images/upgrade_press.png");
    m_updateProgress = new UpdateProgress;
    m_updateProgress->setObjectName("UpgradeProcess");
    m_updateProgress->setFixedSize(33, 33);
    m_updateProgress->setLineWidth(2);
    m_updateProgress->setValue(0);
    m_updateProgress->hide();
    m_appSeparator = new DSeparatorHorizontal;
    m_dbusUpdateInter = new DBusLastoreUpdater("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this);
    m_dbusJobManagerInter = new DBusUpdateJobManager("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this);
    m_appsVBox = new DVBoxWidget;
    m_appsVBox->setFixedHeight(1000);
    m_appsScrollArea = new QScrollArea(this);
    m_appsScrollArea->setFrameStyle(QFrame::NoFrame);
    m_appsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_appsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_appsScrollArea->setStyleSheet("background-color:transparent;");
    m_appsScrollArea->setWidget(m_appsVBox);

    QVBoxLayout *tipsWidgetLayout = new QVBoxLayout;
    tipsWidgetLayout->addSpacing(10);
    tipsWidgetLayout->addWidget(m_updateStatTips);
    tipsWidgetLayout->addSpacing(10);
    tipsWidgetLayout->addWidget(new HSeparatorWidget);
    tipsWidgetLayout->setSpacing(0);
    tipsWidgetLayout->setContentsMargins(15, 0, 15, 0);
    m_tipsWidget->setLayout(tipsWidgetLayout);

    QVBoxLayout *tipsLayout = new QVBoxLayout;
    tipsLayout->addWidget(m_updateCountTips);
    tipsLayout->addWidget(m_updateSizeTips);
    tipsLayout->setSpacing(5);
    tipsLayout->setMargin(0);

    QHBoxLayout *updateInfoLayout = new QHBoxLayout;
    updateInfoLayout->addLayout(tipsLayout);
    updateInfoLayout->addStretch();
    updateInfoLayout->addWidget(m_checkingIndicator);
    updateInfoLayout->addWidget(m_updateButton);
    updateInfoLayout->addWidget(m_updateProgress);
    updateInfoLayout->setSpacing(0);
    updateInfoLayout->setContentsMargins(15, 8, 18, 8);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_tipsWidget);
    mainLayout->addLayout(updateInfoLayout);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(m_appsScrollArea);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    QWidget *interalWidget = new QWidget;
    interalWidget->setLayout(mainLayout);
    QPalette p(QColor("#1a1b1b"));
    interalWidget->setPalette(p);
    interalWidget->setAutoFillBackground(true);

    QVBoxLayout *mainVLayout = new QVBoxLayout;
    mainVLayout->addWidget(interalWidget);
    mainVLayout->addWidget(m_appSeparator);
    mainVLayout->addStretch(1);
    mainVLayout->setSpacing(0);
    mainVLayout->setMargin(0);

    QTimer::singleShot(500, this, SLOT(loadAppList()));
    setLayout(mainVLayout);
    setFixedWidth(DCC::ModuleContentWidth);

    connect(m_updateButton, &DImageButton::clicked, this, &UpdateWidget::systemUpgrade);
    connect(m_updateProgress, &DCircleProgress::clicked, this, &UpdateWidget::toggleUpdateState);
    connect(this, &UpdateWidget::updatableNumsChanged, this, &UpdateWidget::updateInfo, Qt::QueuedConnection);
//    connect(m_dbusJobManagerInter, &DBusUpdateJobManager::UpgradableAppsChanged, this, &UpdateWidget::loadAppList);
//    connect(m_dbusUpdateInter, &DBusLastoreUpdater::UpdatableAppsChanged, this, &UpdateWidget::loadAppList, Qt::QueuedConnection);
//    connect(m_dbusUpdateInter, &DBusLastoreUpdater::UpdatablePackagesChanged, this, &UpdateWidget::loadAppList, Qt::QueuedConnection);
//    connect(m_checkUpdateBtn, &DImageButton::clicked, this, &UpdateWidget::loadAppList);
//    connect(m_checkUpdateBtn, &DImageButton::clicked, m_dbusJobManagerInter, &DBusUpdateJobManager::UpdateSource);
    connect(m_checkUpdateBtn, &DImageButton::clicked, this, &UpdateWidget::checkUpdate);
}

void UpdateWidget::resizeEvent(QResizeEvent *e)
{
    m_appsVBox->setMaximumHeight(e->size().height() - 75);
}

void UpdateWidget::loadAppList()
{
    qDebug() << "reload app list" << ", stat = " << m_upgradeStatus;

    QLayoutItem *item;
    while((item = m_appsVBox->layout()->takeAt(0)) != NULL) {
        item->widget()->deleteLater();
        delete item;
    }
    m_updateProgress->hide();
    m_updateButton->show();

    // load JobList
    QMap<QString, DBusUpdateJob *> jobMap;
    QList<QDBusObjectPath> jobList = m_dbusJobManagerInter->jobList();
    DBusUpdateJob *dbusJob;
    for (QDBusObjectPath &job : jobList)
    {
        dbusJob = new DBusUpdateJob("com.deepin.lastore", job.path(), QDBusConnection::systemBus(), this);
        qDebug() << "fond job: " << dbusJob->packageId() << dbusJob->status() << dbusJob->type();

        if (dbusJob->type() == "update") {
            jobMap.insert(dbusJob->packageId(), dbusJob);
        } else if (dbusJob->type() == "dist_upgrade") {
            // system upgrade job
            refreshProgress(SysUpGrading);
            loadUpgradeJob(dbusJob);
        } else if (dbusJob->type() == "update_source") {
            refreshProgress(SysCheckUpdate);
            loadCheckUpdateJob(dbusJob);
        } else {
            // TODO/FIXME: not handled job
            qWarning() << "not handled job: " << dbusJob->packageId() << dbusJob->status() << dbusJob->type();
        }
    }

    QList<AppUpdateInfo> updateInfoList = getUpdateInfoList();

//    QList<AppUpdateInfo> updateInfoList = m_dbusUpdateInter->ApplicationUpdateInfos(QLocale().name()).value();
    ApplictionItemWidget *appItemWidget;

    for (AppUpdateInfo &info : updateInfoList)
    {
        qDebug() << "add app: " << info.m_name << m_upgradeStatus;

        appItemWidget = new ApplictionItemWidget;
        appItemWidget->setAppUpdateInfo(info);
        if (m_upgradeStatus == SysUpGrading || m_upgradeStatus == SysFail || info.m_packageId == "dde")
            appItemWidget->disableUpdate();
        if (jobMap.contains(info.m_packageId))
            appItemWidget->connectToJob(jobMap.value(info.m_packageId));

        m_appsVBox->layout()->addWidget(appItemWidget);

        connect(appItemWidget, &ApplictionItemWidget::jobFinished, this, &UpdateWidget::removeJob);
    }

    m_updatableAppsList = updatableApps();
    m_updatablePackagesList = updatablePackages();

    qDebug() << "updatableApps: " << m_updatableAppsList;
    qDebug() << "updatablePackages: " << m_updatablePackagesList;

    // updatable nums change
    emit updatableNumsChanged(m_updatableAppsList.count(), m_updatablePackagesList.count());
}

void UpdateWidget::updateUpgradeProcess()
{
    if (!m_dbusSystemUpgrade || !m_dbusSystemUpgrade->isValid())
        return;

    const double progress = m_dbusSystemUpgrade->progress();
    const int percent = int(100 * progress);

    m_updateProgress->showLoading();
    m_updateProgress->setValue(percent);
    m_updateProgress->setText(QString("%1").arg(percent));

    qDebug() << "progress: " << progress << percent;
}

void UpdateWidget::updateUpgradeState()
{
    if (!m_dbusSystemUpgrade || !m_dbusSystemUpgrade->isValid())
        return;

    const QString status = m_dbusSystemUpgrade->status();
    const QString type = m_dbusSystemUpgrade->type();
    const QString id = m_dbusSystemUpgrade->id();

    qDebug() << "state: " << type << status << id;

    // TODO/FIXME: 当Job为end状态时，马上会被销毁，所以把status为空的状态当做end状态处理
    if (status == "succeed" || status == "end" || status.isEmpty())
    {
        refreshProgress(NotStart);
        return loadAppList();
    }

    if (status == "failed")
        refreshProgress(SysFail);
}

void UpdateWidget::removeJob()
{
    ApplictionItemWidget *appItemWidget = qobject_cast<ApplictionItemWidget *>(sender());
    if (!appItemWidget)
        return;

    m_appsVBox->layout()->removeWidget(appItemWidget);

    // TODO:FIXME: nums ?
    emit updatableNumsChanged(m_appsVBox->layout()->count(), 1);
}

void UpdateWidget::updateInfo(const int apps, const int packages)
{
    const bool upgrading = m_upgradeStatus != NotStart;
    qDebug() << "updatable apps num: " << apps << packages << "upgrading = " << upgrading << m_upgradeStatus;

    if (m_upgradeStatus == SysCheckUpdate)
        return;

    // no update
    if (!apps && !packages)
    {
        m_updateStatTips->setText(tr("Your system is up to date"));
        m_tipsWidget->show();
        m_checkingIndicator->show();
        m_updateButton->hide();
        m_updateCountTips->setText(tr("Click to view available  updates"));
        m_updateSizeTips->clear();
        m_appsVBox->hide();
        m_appSeparator->hide();
        m_updateSizeTips->hide();
    }
    else
    {
        m_tipsWidget->hide();
        m_checkingIndicator->hide();
        m_updateButton->show();
        m_appsVBox->show();
        m_appSeparator->show();
        m_updateSizeTips->show();

        // have app or package update, calculate download size
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_dbusJobManagerInter->PackagesDownloadSize(m_updatablePackagesList), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
            qlonglong size = watcher->reply().arguments().first().toLongLong();

            if (size != -1)
                m_updateSizeTips->setText(QString(tr("Total download size: %1")).arg(formatCap(size, 1000)));
            else
                m_updateSizeTips->setText(tr("Unknown"));

            watcher->deleteLater();
        });
    }

    // app update
    if (apps)
    {
        m_updateCountTips->setText(QString(tr("%1 software need to be updated")).arg(apps));
    }

    // have package update, insert "Deepin system upgrade item"
    // insert only apps num equal m_appList count, if these item already inserted, count == apps + 1
    // NOTE-hualet, 2016-7-20: system update is now represented by package dde,
    // so there's no need to take care of it specially.
    /*
    if (packages != apps && apps == m_appsVBox->layout()->count())
    {
        ApplictionItemWidget *sysItem = new ApplictionItemWidget;
        sysItem->setAppName("Deepin");
        sysItem->setAppIcon(QPixmap(":/images/images/deepin.svg"));
        sysItem->setAppVer(tr("Patches"));
        sysItem->disableUpdate();
        m_appsVBox->layout()->insertWidget(0, sysItem);
        m_updateCountTips->setText(tr("Some patches need to be updated"));
    }
    */

    if (apps && packages != apps)
        m_updateCountTips->setText(QString(tr("Some patches and %1 software need to be updated")).arg(apps));

    // hide last separator
    if(m_appsVBox->layout()->count() > 0) {
        ApplictionItemWidget *item = qobject_cast<ApplictionItemWidget *>(m_appsVBox->layout()->itemAt(m_appsVBox->layout()->count() - 1)->widget());
        if (item)
            item->hideSeparator();
    }

    // hide when upgrading
    if (upgrading)
    {
        m_updateButton->hide();
        m_checkingIndicator->hide();
    }

//    if (m_upgradeStatus == NotStart)
//    {
//        m_updateButton->show();
//    }
}

void UpdateWidget::checkUpdate()
{
    if (m_upgradeStatus == SysCheckUpdate)
        return;
    refreshProgress(SysCheckUpdate);

    QDBusPendingReply<QDBusObjectPath> reply = m_dbusJobManagerInter->UpdateSource();
    reply.waitForFinished();
    qDebug() << "check update finished" << reply.value().path() << reply.error();

    const QString jobPath = reply.value().path();

    m_dbusCheckupdate = new DBusUpdateJob("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);
    loadCheckUpdateJob(m_dbusCheckupdate);
}

void UpdateWidget::loadCheckUpdateJob(DBusUpdateJob *updateJob)
{
    if (m_dbusCheckupdate)
        qWarning() << "check update job already exist!!!!!";

    m_dbusCheckupdate = updateJob;

    m_appSeparator->hide();
    m_appsVBox->hide();
    m_tipsWidget->show();
    m_updateButton->hide();
    m_updateSizeTips->hide();
    m_checkingIndicator->setLoading(true);
    m_updateCountTips->setText(tr("Checking for updates"));
    m_updateStatTips->setText(tr("Check for updates, please wait"));

    connect(m_dbusCheckupdate, &DBusUpdateJob::StatusChanged, this, &UpdateWidget::checkUpdateStateChanged);
    checkUpdateStateChanged();
}

void UpdateWidget::refreshProgress(UpdateWidget::UpgradeState state)
{
    if (m_upgradeStatus == state)
        return;

    qDebug() << "upgrade status = " << state;
    m_upgradeStatus = state;

    switch (state)
    {
    case NotStart:
    case SysFail:
        m_updateProgress->hideLoading();
        m_updateProgress->topLabel()->setPixmap(QPixmap(":/images/images/start.png"));
        break;
    case SysUpGrading:
        m_updateProgress->topLabel()->clear();
        m_updateProgress->showLoading();
        disableAppsUpgrade();
        break;
    default:            qDebug() << "refresh Progress" << state << m_upgradeStatus;
    }
}

void UpdateWidget::restartUpgrade()
{
    refreshProgress(SysUpGrading);
    m_dbusJobManagerInter->StartJob(m_dbusSystemUpgrade->id());
}

void UpdateWidget::checkUpdateStateChanged()
{
    if (!m_dbusCheckupdate)
        return;

    const QString &stat = m_dbusCheckupdate->status();
    qDebug() << stat << m_upgradeStatus;

    if (/*stat.isEmpty() || */stat == "end" || stat == "failed")
    {
        // TODO:
        if (m_upgradeStatus == SysCheckUpdate)
        {
            refreshProgress(NotStart);
            m_checkingIndicator->setLoading(false);
            m_checkingIndicator->setRotate(0);

            updateInfo(m_updatableAppsList.count(), m_updatablePackagesList.count());

            // no updates
            if (!m_updatableAppsList.count() && !m_updatablePackagesList.count())
            {
//                if (stat == "end")
//                    m_updateCountTips->setText(tr("No updates avaliable"));
//                else
//                    m_updateCountTips->setText(tr("Check update failed"));
            }
        }
//            m_updateCountTips->setText(tr("imde"));

        m_dbusCheckupdate->deleteLater();
        m_dbusCheckupdate = nullptr;
    }
}

QList<AppUpdateInfo> UpdateWidget::getUpdateInfoList() const
{
    QList<AppUpdateInfo> infos;

    QFile updateInfos("/var/lib/lastore/update_infos.json");
    if (updateInfos.open(QFile::ReadOnly)) {
        QByteArray data = updateInfos.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray packages = doc.array();

        for (QJsonValue val : packages) {
            QJsonObject pack = val.toObject();
            QString packageName = pack["Package"].toString();
            QString metadataDir = "/lastore/metadata/" + packageName;

            if (QFile::exists(metadataDir)) {
                AppUpdateInfo info;
                info.m_packageId = packageName;
                info.m_currentVersion = pack["CurrentVersion"].toString();
                info.m_avilableVersion = pack["LastVersion"].toString();
                info.m_icon = metadataDir + "/meta/icons/" + packageName + ".svg";

                QFile manifest(metadataDir + "/meta/manifest.json");
                if (manifest.open(QFile::ReadOnly)) {
                    QByteArray data = manifest.readAll();
                    QJsonDocument doc = QJsonDocument::fromJson(data);
                    QJsonObject object = doc.object();
                    QJsonObject locales = object["locales"].toObject();
                    QJsonObject locale = locales[QLocale::system().name()].toObject();
                    QJsonObject changelog = locale["changelog"].toObject();

                    info.m_name = locale["name"].toString();

                    for (QString version : changelog.keys()) {
                        // TODO: valid version compare mechanism.
                        if (version.compare(info.m_currentVersion, Qt::CaseInsensitive) > 0) {
                            if (info.m_changelog.isNull() || info.m_changelog.isEmpty()) {
                                info.m_changelog = info.m_changelog + changelog.value(version).toString();
                            } else {
                                info.m_changelog = info.m_changelog + '\n' + changelog.value(version).toString();
                            }
                        }
                    }
                }

                infos << info;
            }
        }
    }

    return infos;
}

QStringList UpdateWidget::updatableApps() const
{
    QStringList apps;
    QStringList pkgs = updatablePackages();

    for (const QString& pkg : pkgs) {
        if (QFile::exists("/lastore/metadata/" + pkg)) {
            apps << pkg;
        }
    }

    return apps;
}

QStringList UpdateWidget::updatablePackages() const
{
    QStringList pkgs;

    QFile updateInfos("/var/lib/lastore/update_infos.json");
    if (updateInfos.open(QFile::ReadOnly)) {
        QByteArray data = updateInfos.readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray packages = doc.array();

        for (QJsonValue val : packages) {
            QJsonObject pack = val.toObject();

            pkgs << pack["Package"].toString();
        }
    }

    return pkgs;
}

void UpdateWidget::systemUpgrade()
{
    if (m_upgradeStatus != NotStart)
        return;

    m_updateProgress->setValue(0);
    m_updateProgress->show();
    m_updateProgress->showLoading();
    m_updateButton->hide();
    m_checkingIndicator->hide();

    // sys upgrade
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_dbusJobManagerInter->DistUpgrade(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
        const QDBusPendingReply<QDBusObjectPath> &reply = *watcher;
        const QDBusObjectPath &path = reply;

        qDebug() << "start upgrade job: " << path.path() << reply.error();
        DBusUpdateJob *job = new DBusUpdateJob("com.deepin.lastore", path.path(), QDBusConnection::systemBus(), this);

        if (job->isValid())
            loadUpgradeJob(job);
        else
            job->deleteLater();

        watcher->deleteLater();
    });
}

void UpdateWidget::loadUpgradeJob(DBusUpdateJob *newJob)
{
    if (m_dbusSystemUpgrade)
        m_dbusSystemUpgrade->deleteLater();

    qDebug() << "load upgrade job " << newJob->id() << newJob->status();

    m_dbusSystemUpgrade = newJob;
    const QString &status = m_dbusSystemUpgrade->status();
    if (status == "success" || status == "end")
    {
        refreshProgress(NotStart);
        return;
    }

    refreshProgress(SysUpGrading);

    m_updateProgress->setValue(0);
    m_updateProgress->show();
    m_updateProgress->showLoading();
    m_updateButton->hide();

    connect(m_dbusSystemUpgrade, &DBusUpdateJob::ProgressChanged, this, &UpdateWidget::updateUpgradeProcess);
    connect(m_dbusSystemUpgrade, &DBusUpdateJob::StatusChanged, this, &UpdateWidget::updateUpgradeState);

//    if (status == "ready")
//    {
//        m_dbusJobManagerInter->StartJob(m_dbusSystemUpgrade->id());
//        refreshProgress(Running);
//    }

    updateUpgradeProcess();

    if (status == "failed")
        refreshProgress(SysFail);

//    updateUpgradeState();
}

void UpdateWidget::toggleUpdateState()
{
    switch (m_upgradeStatus)
    {
    case SysFail:      restartUpgrade();       break;
    default:        qDebug() << "toggleUpgrade: " << m_upgradeStatus << __FILE__ << __LINE__;
    }
}

void UpdateWidget::disableAppsUpgrade()
{
    qDebug() << "disable Apps upgrade, size = " << m_appsVBox->layout()->count();

    const int count = m_appsVBox->layout()->count();
    for (int i(0); i != count; ++i)
    {
        ApplictionItemWidget *item = qobject_cast<ApplictionItemWidget *>(m_appsVBox->layout()->itemAt(i)->widget());
        item->disableUpdate();
    }
}
