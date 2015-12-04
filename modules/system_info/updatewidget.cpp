#include "updatewidget.h"
#include "separatorwidget.h"
#include "constants.h"
#include "dbus/appupdateinfo.h"
#include "dbus/dbusupdatejob.h"

#include <QVBoxLayout>
#include <QResizeEvent>
#include <QDBusPendingCallWatcher>
#include <QMap>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dthememanager.h>

UpdateWidget::UpdateWidget(QWidget *parent)
    : QWidget(parent)
{

    D_THEME_INIT_WIDGET(UpdateWidget);

    MirrorInfo::registerMetaType();
    AppUpdateInfo::registerMetaType();

    m_updateCountTips = new QLabel;
    m_updateCountTips->setWordWrap(true);
    m_updateCountTips->setObjectName("Tips");
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
    m_updateProgress = new DCircleProgress;
    m_updateProgress->setObjectName("UpgradeProcess");
    m_updateProgress->setFixedSize(32, 32);
    m_updateProgress->setLineWidth(2);
    m_updateProgress->setValue(0);
    m_updateProgress->hide();
    m_appsList = new DListWidget;
    m_appsList->setMinimumHeight(0);
    m_appsList->setFixedWidth(DCC::ModuleContentWidth);
    m_appsList->setItemSize(DCC::ModuleContentWidth, 50);
    m_appsList->setEnableVerticalScroll(true);
    m_appsList->setObjectName("AppList");
    m_appsList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_appSeparator = new DSeparatorHorizontal;
    m_dbusUpdateInter = new DBusLastoreUpdater("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this);
    m_dbusJobManagerInter = new DBusUpdateJobManager("com.deepin.lastore", "/com/deepin/lastore", QDBusConnection::systemBus(), this);

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
    mainLayout->addLayout(updateInfoLayout);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(m_appsList);
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
    connect(this, &UpdateWidget::updatableNumsChanged, this, &UpdateWidget::updateInfo);
//    connect(m_dbusJobManagerInter, &DBusUpdateJobManager::UpgradableAppsChanged, this, &UpdateWidget::loadAppList);
    connect(m_dbusUpdateInter, &DBusLastoreUpdater::UpdatableAppsChanged, this, &UpdateWidget::loadAppList);
    connect(m_dbusUpdateInter, &DBusLastoreUpdater::UpdatablePackagesChanged, this, &UpdateWidget::loadAppList);
//    connect(m_checkUpdateBtn, &DImageButton::clicked, this, &UpdateWidget::loadAppList);
//    connect(m_checkUpdateBtn, &DImageButton::clicked, m_dbusJobManagerInter, &DBusUpdateJobManager::UpdateSource);
    connect(m_checkUpdateBtn, &DImageButton::clicked, this, &UpdateWidget::checkUpdate);
}

void UpdateWidget::resizeEvent(QResizeEvent *e)
{
    m_appsList->setMaximumHeight(e->size().height() - 55);
}

void UpdateWidget::loadAppList()
{
    m_appsList->clear();
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
            loadUpgradeJob(dbusJob);
        } else {
            // TODO/FIXME: not handled job
            qWarning() << "not handled job: " << dbusJob->packageId() << dbusJob->status() << dbusJob->type();

//            m_dbusJobManagerInter->CleanJob(dbusJob->id());
        }
    }

    // TODO: lang
    QList<AppUpdateInfo> updateInfoList = m_dbusUpdateInter->ApplicationUpdateInfos("zh_CN").value();
    ApplictionItemWidget *appItemWidget;

    for (const AppUpdateInfo &info : updateInfoList)
    {
        appItemWidget = new ApplictionItemWidget;
        appItemWidget->setAppUpdateInfo(info);
        if (jobMap.contains(info.m_packageId))
            appItemWidget->connectToJob(jobMap.value(info.m_packageId));

        m_appsList->addWidget(appItemWidget);

        connect(appItemWidget, &ApplictionItemWidget::jobFinished, this, &UpdateWidget::removeJob);
    }

    m_updatableAppsList = m_dbusUpdateInter->updatableApps();
    m_updatablePackagesList = m_dbusUpdateInter->updatablePackages();

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

    m_updateProgress->setValue(percent);
    m_updateProgress->setText(QString("%1").arg(percent));

    qDebug() << "progress: " << progress << percent;
}

void UpdateWidget::updateUpgradeState()
{
    if (!m_dbusSystemUpgrade || !m_dbusSystemUpgrade->isValid())
        return;

    qDebug() << "state: " << m_dbusSystemUpgrade->type() << m_dbusSystemUpgrade->status() << m_dbusSystemUpgrade->id();

    const QString status = m_dbusSystemUpgrade->status();

    if (status == "succeed" || status == "end")
        return loadAppList();

    if (status == "failed")
        refreshProgress(Fail);
}

void UpdateWidget::removeJob()
{
    ApplictionItemWidget *appItemWidget = qobject_cast<ApplictionItemWidget *>(sender());
    if (!appItemWidget)
        return;

    m_appsList->removeWidget(m_appsList->indexOf(appItemWidget));

    // TODO:FIXME: nums ?
    emit updatableNumsChanged(m_appsList->count(), 1);
}

void UpdateWidget::updateInfo(const int apps, const int packages)
{
    const bool upgrading = m_upgradeStatus != NotStart;
    qDebug() << "updatable apps num: " << apps << packages << "upgrading = " << upgrading << m_upgradeStatus;

    if (m_upgradeStatus == CheckUpdate)
        return;

    // no update
    if (!apps && !packages)
    {
        m_checkingIndicator->show();
        m_updateButton->hide();
        m_updateCountTips->setText(tr("Click to check update."));
        m_updateSizeTips->clear();
        m_appsList->hide();
        m_appSeparator->hide();
        m_updateSizeTips->hide();
    }
    else
    {
        m_checkingIndicator->hide();
        m_updateButton->show();
        m_appsList->show();
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
    if (packages != apps)
    {
        ApplictionItemWidget *sysItem = new ApplictionItemWidget;
        sysItem->setAppName("Deepin");
        sysItem->setAppIcon(QPixmap(":/images/images/deepin.svg"));
        sysItem->setAppVer(tr("Patches"));
        sysItem->disableUpdate();
        m_appsList->insertWidget(0, sysItem);
        m_updateCountTips->setText(tr("Some patches need to be updated"));
    }

    if (apps && packages != apps)
        m_updateCountTips->setText(QString(tr("Some patches and %1 software need to be updated")).arg(apps));

    // hide last separator
    ApplictionItemWidget *item = qobject_cast<ApplictionItemWidget *>(m_appsList->getWidget(m_appsList->count() - 1));
    if (item)
        item->hideSeparator();

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
    if (m_upgradeStatus == CheckUpdate)
        return;
    m_upgradeStatus = CheckUpdate;
    m_checkingIndicator->setLoading(true);

//    m_updateCountTips->setText();

    // TODO: check update
    qDebug() << "check update";
    QDBusPendingReply<QDBusObjectPath> reply = m_dbusJobManagerInter->UpdateSource();
    reply.waitForFinished();
    qDebug() << "check update finished" << reply.value().path() << reply.error();

    const QString jobPath = reply.value().path();

    m_dbusCheckupdate = new DBusUpdateJob("com.deepin.lastore", jobPath, QDBusConnection::systemBus(), this);
    connect(m_dbusCheckupdate, &DBusUpdateJob::StatusChanged, this, &UpdateWidget::checkUpdateStateChanged);
}

void UpdateWidget::refreshProgress(UpdateWidget::UpgradeState state)
{
    if (m_upgradeStatus == state)
        return;

    qDebug() << "upgrade status = " << state;
    m_upgradeStatus = state;

    switch (state)
    {
    case Running:   m_updateProgress->topLabel()->clear();      break;
    case Fail:      m_updateProgress->topLabel()->setPixmap(QPixmap(":/images/images/start.png"));         break;
    default:        qDebug() << "upgrade " << __FILE__ << __LINE__;
    }
}

void UpdateWidget::restartUpgrade()
{
    refreshProgress(Running);
    m_dbusJobManagerInter->StartJob(m_dbusSystemUpgrade->id());
}

void UpdateWidget::checkUpdateStateChanged()
{
    if (!m_dbusCheckupdate)
        return;

    const QString &stat = m_dbusCheckupdate->status();
    qDebug() << stat << m_upgradeStatus;

    if (stat == "end")
    {
        // TODO:
        if (m_upgradeStatus == CheckUpdate)
        {
            m_upgradeStatus = NotStart;
            m_checkingIndicator->setLoading(false);
            m_checkingIndicator->setRotate(0);

            updateInfo(m_updatableAppsList.count(), m_updatablePackagesList.count());
        }
//            m_updateCountTips->setText(tr("imde"));

        m_dbusCheckupdate->deleteLater();
        m_dbusCheckupdate = nullptr;
    }
}

void UpdateWidget::systemUpgrade()
{
    // TODO: if no system update avaliable
//    if (m_updateSizeTips->isVisible())

    m_updateProgress->setValue(0);
    m_updateProgress->show();
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
        m_upgradeStatus = NotStart;
        return;
    }
    m_upgradeStatus = Running;

    // disable apps update
    disableAppsUpgrade();

    m_updateProgress->setValue(0);
    m_updateProgress->show();
    m_updateButton->hide();

    connect(m_dbusSystemUpgrade, &DBusUpdateJob::ProgressChanged, this, &UpdateWidget::updateUpgradeProcess);
    connect(m_dbusSystemUpgrade, &DBusUpdateJob::StatusChanged, this, &UpdateWidget::updateUpgradeState);

//    if (status == "ready")
//    {
//        m_dbusJobManagerInter->StartJob(m_dbusSystemUpgrade->id());
//        refreshProgress(Running);
//    }

    if (status == "failed")
        refreshProgress(Fail);

//    updateUpgradeProcess();
//    updateUpgradeState();
}

void UpdateWidget::toggleUpdateState()
{
    switch (m_upgradeStatus)
    {
    case Fail:      restartUpgrade();       break;
    default:        qDebug() << "toggleUpgrade: " << m_upgradeStatus << __FILE__ << __LINE__;
    }
}

void UpdateWidget::disableAppsUpgrade()
{
    const int count = m_appsList->count();
    for (int i(0); i != count; ++i)
    {
        ApplictionItemWidget *item = qobject_cast<ApplictionItemWidget *>(m_appsList->getWidget(i));
        item->disableUpdate();
    }
}
