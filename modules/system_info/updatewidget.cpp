#include "updatewidget.h"
#include "separatorwidget.h"
#include "constants.h"
#include "dbus/appupdateinfo.h"

#include <QVBoxLayout>
#include <QResizeEvent>
#include <QDBusPendingCallWatcher>
#include <QMap>

#include <libdui/dseparatorhorizontal.h>
#include <libdui/dthememanager.h>

#include "dbus/dbusupdatejob.h"

UpdateWidget::UpdateWidget(QWidget *parent)
    : QWidget(parent)
{

    D_THEME_INIT_WIDGET(UpdateWidget);

    MirrorInfo::registerMetaType();
    AppUpdateInfo::registerMetaType();

    m_updateCountTips = new QLabel;
    m_updateCountTips->setObjectName("Tips");
    m_updateSizeTips = new QLabel;
    m_updateSizeTips->setObjectName("Tips");
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
    tipsLayout->setSpacing(0);
    tipsLayout->setMargin(0);

    QHBoxLayout *updateInfoLayout = new QHBoxLayout;
    updateInfoLayout->addLayout(tipsLayout);
    updateInfoLayout->addStretch();
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
            loadUpgradeData(dbusJob);
        } else {
            // TODO/FIXME: not handled job
            qWarning() << "not handled job: " << dbusJob->packageId() << dbusJob->status() << dbusJob->type();
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

    // updatable nums change
    emit updatableNumsChanged(m_appsList->count());
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

    if (m_dbusSystemUpgrade->status() == "end")
        loadAppList();
}

void UpdateWidget::removeJob()
{
    ApplictionItemWidget *appItemWidget = qobject_cast<ApplictionItemWidget *>(sender());
    if (!appItemWidget)
        return;

    m_appsList->removeWidget(m_appsList->indexOf(appItemWidget));
    emit updatableNumsChanged(m_appsList->count());
}

void UpdateWidget::updateInfo(const int updatableAppsNum)
{
    qDebug() << "updatable apps num: " << updatableAppsNum;

    if (!updatableAppsNum)
    {
        m_updateCountTips->setText(tr("No update avaliable."));
        m_updateSizeTips->clear();
        m_appsList->hide();
        m_appSeparator->hide();
        m_updateSizeTips->hide();
    } else {
        m_appsList->show();
        m_appSeparator->show();
        m_updateSizeTips->show();
        m_updateCountTips->setText(QString(tr("You have %1 softwares need update")).arg(updatableAppsNum));

        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_dbusJobManagerInter->PackagesDownloadSize(m_dbusJobManagerInter->upgradableApps()), this);
        connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
            m_updateSizeTips->setText(QString(tr("Total download size: %1")).arg(formatCap(watcher->reply().arguments().first().toLongLong(), 1000)));
            watcher->deleteLater();
        });


        ApplictionItemWidget *item = qobject_cast<ApplictionItemWidget *>(m_appsList->getWidget(m_appsList->count() - 1));
        if (item)
            item->hideSeparator();
    }
}

void UpdateWidget::systemUpgrade()
{
    // TODO: if no system update avaliable
//    if (m_updateSizeTips->isVisible())

    m_updateProgress->setValue(0);
    m_updateProgress->show();
    m_updateButton->hide();

    const int count = m_appsList->count();
    for (int i(0); i != count; ++i)
    {
        ApplictionItemWidget *item = qobject_cast<ApplictionItemWidget *>(m_appsList->getWidget(i));
        item->disableUpdate();
    }

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_dbusJobManagerInter->DistUpgrade(), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
        const QDBusPendingReply<QDBusObjectPath> &reply = *watcher;
        const QDBusObjectPath &path = reply;

        DBusUpdateJob *job = new DBusUpdateJob("com.deepin.lastore", path.path(), QDBusConnection::systemBus(), this);

        if (job->isValid())
            loadUpgradeData(job);
        else
            job->deleteLater();

        watcher->deleteLater();
    });
}

void UpdateWidget::loadUpgradeData(DBusUpdateJob *newJob)
{
    if (m_dbusSystemUpgrade)
        m_dbusSystemUpgrade->deleteLater();

    m_dbusSystemUpgrade = newJob;

    if (m_dbusSystemUpgrade->status() == "end")
        return;

    if (m_dbusSystemUpgrade->status() == "ready")
        m_dbusJobManagerInter->StartJob(m_dbusSystemUpgrade->id());

    connect(m_dbusSystemUpgrade, &DBusUpdateJob::ProgressChanged, this, &UpdateWidget::updateUpgradeProcess);
    connect(m_dbusSystemUpgrade, &DBusUpdateJob::StatusChanged, this, &UpdateWidget::updateUpgradeState);

    updateUpgradeProcess();
    updateUpgradeState();
}

void UpdateWidget::toggleUpdateState()
{
    qDebug() << "toggleUpdateState";
}

