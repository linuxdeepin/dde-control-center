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

    m_dbusUpdateInter = new DBusLastoreManager("org.deepin.lastore", "/org/deepin/lastore", QDBusConnection::systemBus(), this);
    m_dbusJobManagerInter = new DBusUpdateJobManager("org.deepin.lastore", "/org/deepin/lastore", QDBusConnection::systemBus(), this);

    m_updateCountTips = new QLabel;
    m_updateCountTips->setObjectName("Tips");
    m_updateSizeTips = new QLabel;
    m_updateSizeTips->setObjectName("Tips");
    m_updateProgress = new DCircleProgress;
    m_updateProgress->setFixedSize(35, 35);
    m_updateProgress->setLineWidth(2);
    m_appsList = new DListWidget;
//    m_appsList->setSelectionMode(QListWidget::NoSelection);
//    m_appsList->setFrameStyle(QFrame::NoFrame);
    m_appsList->setFixedWidth(DCC::ModuleContentWidth);
    m_appsList->setItemSize(DCC::ModuleContentWidth, 50);
    m_appsList->setEnableVerticalScroll(true);
    m_appsList->setObjectName("AppList");
//    m_appsList->setStyleSheet(QString("background-color:#252627;"));
//    m_appItems = new QMap<QListWidgetItem *, ApplictionItemWidget *>;

    QVBoxLayout *tipsLayout = new QVBoxLayout;
    tipsLayout->addWidget(m_updateCountTips);
    tipsLayout->addWidget(m_updateSizeTips);
    tipsLayout->setSpacing(0);
    tipsLayout->setMargin(0);

    QHBoxLayout *updateInfoLayout = new QHBoxLayout;
    updateInfoLayout->addLayout(tipsLayout);
    updateInfoLayout->addStretch();
    updateInfoLayout->addWidget(m_updateProgress);
    updateInfoLayout->setSpacing(0);
    updateInfoLayout->setContentsMargins(10, 8, 10, 8);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(updateInfoLayout);
//    mainLayout->addWidget(new HSeparatorWidget);
    mainLayout->addWidget(m_appsList);
    mainLayout->addStretch();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    loadAppList();
    updateTipsInfo();
    setLayout(mainLayout);
    setFixedWidth(DCC::ModuleContentWidth);
    //    setFixedHeight(200);
}

void UpdateWidget::resizeEvent(QResizeEvent *e)
{
    m_appsList->setMaximumHeight(e->size().height() - 55);
}

void UpdateWidget::loadAppList()
{
    // load JobList
    QMap<QString, QDBusObjectPath> jobMap;
    QList<QDBusObjectPath> jobList = m_dbusJobManagerInter->jobList();
    DBusUpdateJob *dbusJob;
    for (QDBusObjectPath &job : jobList)
    {
        dbusJob = new DBusUpdateJob("org.deepin.lastore", job.path(), QDBusConnection::systemBus(), this);
        jobMap.insert(dbusJob->packageId(), job);
        qDebug() << "fond job: " << dbusJob->packageId();
    }

    // TODO: lang
    QList<AppUpdateInfo> updateInfoList = m_dbusUpdateInter->ApplicationUpdateInfos1("zh_CN").value();
    ApplictionItemWidget *appItemWidget;

    for (const AppUpdateInfo &info : updateInfoList)
    {
        appItemWidget = new ApplictionItemWidget;
        appItemWidget->setAppUpdateInfo(info);
        if (jobMap.contains(info.m_packageId))
            appItemWidget->connectToJob(jobMap.value(info.m_packageId));

        m_appsList->addWidget(appItemWidget);
    }
}

void UpdateWidget::updateTipsInfo()
{
    const QStringList &updatableApps = m_dbusUpdateInter->updatableApps1();

    m_updateCountTips->setText(QString("You have %1 softwares need update").arg(updatableApps.count()));
//    m_updateSizeTips->setText(QString("Total download size: %1").arg(m_dbusJobInter->PackagesDownloadSize(updatableApps)));

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_dbusJobManagerInter->PackagesDownloadSize(updatableApps), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
        m_updateSizeTips->setText(QString("Total download size: %1").arg(watcher->reply().arguments().first().toLongLong()));
        watcher->deleteLater();
    });
}

