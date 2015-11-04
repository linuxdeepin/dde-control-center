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

    m_updateCountTips = new QLabel;
    m_updateCountTips->setObjectName("Tips");
    m_updateSizeTips = new QLabel;
    m_updateSizeTips->setObjectName("Tips");
    m_updateButton = new DImageButton;
    m_updateButton->setNormalPic(":/images/images/upgrade_normal.png");
    m_updateButton->setHoverPic(":/images/images/upgrade_hover.png");
    m_updateButton->setPressPic(":/images/images/upgrade_press.png");
    m_updateProgress = new DCircleProgress;
    m_updateProgress->setFixedSize(32, 32);
    m_updateProgress->setLineWidth(2);
    m_appsList = new DListWidget;
    m_appsList->setFixedWidth(DCC::ModuleContentWidth);
    m_appsList->setItemSize(DCC::ModuleContentWidth, 50);
    m_appsList->setEnableVerticalScroll(true);
    m_appsList->setObjectName("AppList");

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
    updateInfoLayout->setContentsMargins(10, 8, 10, 8);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(updateInfoLayout);
    mainLayout->addWidget(m_appsList);
    mainLayout->addStretch();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

//    loadAppList();
    QTimer::singleShot(500, this, SLOT(loadAppList()));
    setLayout(mainLayout);
    setFixedWidth(DCC::ModuleContentWidth);

    connect(m_updateButton, &DImageButton::clicked, this, &UpdateWidget::systemUpgrade);
    connect(m_updateProgress, &DCircleProgress::clicked, this, &UpdateWidget::toggleUpdateState);
}

QString UpdateWidget::formatCap(qulonglong cap)
{
    QString type[] = {"B", "KB", "MB", "GB", "TB"};

    if (cap < qulonglong(1024))
        return QString::number(cap) + type[0];
    if (cap < qulonglong(1024) * 1024)
        return QString::number(double(cap) / 1024, 'f', 2) + type[1];
    if (cap < qulonglong(1024) * 1024 * 1024)
        return QString::number(double(cap) / 1024 / 1024, 'f', 2) + type[2];
    if (cap < qulonglong(1024) * 1024 * 1024 * 1024)
        return QString::number(double(cap) / 1024 / 1024 / 1024, 'f', 2) + type[3];

    return QString::number(double(cap) / 1024 / 1024 / 1024 / 1024, 'f', 2) + type[4];
}

void UpdateWidget::resizeEvent(QResizeEvent *e)
{
    m_appsList->setMaximumHeight(e->size().height() - 55);
}

void UpdateWidget::loadAppList()
{
    m_dbusUpdateInter = new DBusLastoreManager("org.deepin.lastore", "/org/deepin/lastore", QDBusConnection::systemBus(), this);
    m_dbusJobManagerInter = new DBusUpdateJobManager("org.deepin.lastore", "/org/deepin/lastore", QDBusConnection::systemBus(), this);

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

    // updatable nums change
    emit updatableNumsChanged(m_appsList->count());

    // updateTipsInfo  download size, download count ...
    const QStringList &updatableApps = m_dbusUpdateInter->updatableApps1();
    m_updateCountTips->setText(QString("You have %1 softwares need update").arg(updatableApps.count()));

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(m_dbusJobManagerInter->PackagesDownloadSize(updatableApps), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, watcher] {
        m_updateSizeTips->setText(QString("Total download size: %1").arg(watcher->reply().arguments().first().toLongLong()));
        watcher->deleteLater();
    });
}

void UpdateWidget::systemUpgrade()
{
    qDebug() << "system upgrade";
}

void UpdateWidget::toggleUpdateState()
{
    qDebug() << "toggleUpdateState";
}

