/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "updatewidget.h"
#include "modules/update/updatemodel.h"
#include "modules/update/updatework.h"
#include "modules/update/updateitem.h"
#include "widgets/settingsgroup.h"
#include "updatesettings.h"
#include "updatehistorybutton.h"
#include "recenthistoryapplist.h"

#include <types/appupdateinfolist.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QStackedLayout>

using namespace dcc::update;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateWidget::UpdateWidget(QWidget *parent)
    : QWidget(parent)
    , m_bottomLabel(new Dtk::Widget::DImageButton)
    , m_layout(new QVBoxLayout)
    , m_listview(new QListView)
    , m_model(nullptr)
    , m_work(nullptr)
    , m_centerWidget(new QWidget)
    , m_centerLayout(new QVBoxLayout)
    , m_label(new QLabel)
    , m_historyBtn(new UpdateHistoryButton)
    , m_updateState(UpdatesStatus::Default)
    , m_updateHistoryText(new QLabel(tr("Recently updated")))//最近更新
    , m_applistGroup(new SettingsGroup)
    , m_recentHistoryApplist(new RecentHistoryApplist)
{
    QWidget *centerWidget = new QWidget;
    centerWidget->setFixedSize(360, 40);
    centerWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_listview->setFlow(QListView::LeftToRight);
    m_listview->setFixedHeight(40);

    auto model = new QStandardItemModel(this);
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-calendar"), tr("Update Checked")));//检查更新
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-file-manager"), tr("Update Setting")));//更新设置
    m_listview->setModel(model);
    m_defaultIndex = model->index(0, default_listview_index);

    connect(m_listview, &QListView::pressed, this, &UpdateWidget::onTopListviewCliecked);

    QHBoxLayout *centerLayout = new QHBoxLayout;
    centerLayout->addWidget(m_listview);
    centerWidget->setLayout(centerLayout);
    m_layout->addWidget(centerWidget, 0, Qt::AlignTop | Qt::AlignCenter);

    m_centerWidget->setLayout(m_centerLayout);
    m_centerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_layout->addWidget(m_centerWidget);

    QWidget *recentHistoryWidget = new QWidget;
    QVBoxLayout *bottomLayout = new QVBoxLayout;
    recentHistoryWidget->setLayout(bottomLayout);

    bottomLayout->setMargin(0);
    bottomLayout->setSpacing(0);
    bottomLayout->addWidget(m_label, 0, Qt::AlignCenter);
    bottomLayout->addWidget(m_historyBtn, 0, Qt::AlignCenter);
    bottomLayout->addWidget(m_updateHistoryText, 0, Qt::AlignCenter);
    bottomLayout->addWidget(m_recentHistoryApplist);

    m_layout->addWidget(recentHistoryWidget);

    m_historyBtn->setVisible(false);
    m_updateHistoryText->setVisible(false);
    m_recentHistoryApplist->setVisible(false);
    m_recentHistoryApplist->setContentWidget(m_applistGroup);

    m_layout->addWidget(m_recentHistoryApplist);

    setLayout(m_layout);
}

UpdateWidget::~UpdateWidget()
{

}

void UpdateWidget::initialize()
{
    QTimer::singleShot(0, this, [this] {
        setDefaultState();
    });

    connect(m_historyBtn, &UpdateHistoryButton::notifyBtnRelease, this, [ = ](bool state) {
        resetUpdateCheckState();

        if (state) {
            m_historyBtn->setLabelText(tr("Return"));//返回
            m_updateHistoryText->setVisible(true);
            m_applistGroup->setVisible(true);
            m_recentHistoryApplist->setVisible(true);
            onAppendApplist(getTestApplistInfo());
        } else {
            m_centerWidget->setVisible(true);
            m_applistGroup->setVisible(false);
        }
    });
}

void UpdateWidget::setModel(const UpdateModel *model, const UpdateWorker *work)
{
    m_model = const_cast<UpdateModel *>(model);
    m_work = const_cast<UpdateWorker *>(work);
}

void UpdateWidget::setDefaultState()
{
    m_listview->setCurrentIndex(m_defaultIndex);
    m_listview->pressed(m_defaultIndex);
}

void UpdateWidget::setSystemVersion(QString version)
{
    qDebug() << Q_FUNC_INFO << QString("%1 : %2").arg(tr("Current Edition")).arg(version.toLatin1().data());

    if (m_systemVersion != version) {
        m_systemVersion = version.remove('"');
    }

    m_label->setText(QString("%1 :  V%2").arg(tr("Current Edition")).arg(version.remove('"')));
}

void UpdateWidget::resetUpdateCheckState(bool state)
{
    m_label->setVisible(false);
    m_historyBtn->setVisible(state);
    m_centerWidget->setVisible(false);
    m_updateHistoryText->setVisible(false);
    m_historyBtn->setLabelText(tr("Update History"));
    m_recentHistoryApplist->setVisible(false);
}

void UpdateWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

//Used to test in "Recent update"
QList<AppUpdateInfo> UpdateWidget::getTestApplistInfo()
{
    QList<AppUpdateInfo> applist;

    AppUpdateInfo date5;
    date5.m_avilableVersion = "深度截图";
    date5.m_changelog = "深度截图";
    date5.m_currentVersion = "15.11.18";
    date5.m_icon = "/lastore/metadata/deepin-screenshot/meta/icons/deepin-screenshot.svg";
    date5.m_name = "Deepin";
    date5.m_packageId = "dde";

    applist.append(date5);

    AppUpdateInfo date;
    for (int i = 0; i < 10; i++) {
        date.m_avilableVersion = "雷鸟邮件";
        date.m_changelog = "雷鸟邮件wubowen test now I'm in deepin work on update module";
        date.m_currentVersion = "15.11.19";
        date.m_icon = "/lastore/metadata/thunderbird/meta/icons/thunderbird.svg";
        date.m_name = "Deepin";
        date.m_packageId = "dde";
        applist.append(date);
    }

    return applist;
}

void UpdateWidget::showCheckUpdate()
{
    qDebug() << Q_FUNC_INFO;

    m_work->checkForUpdates();

    UpdateCtrlWidget *updateWidget = new UpdateCtrlWidget(m_model);
    updateWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(updateWidget, &UpdateCtrlWidget::requestDownloadUpdates, m_work, &UpdateWorker::downloadAndDistUpgrade);
    connect(updateWidget, &UpdateCtrlWidget::requestPauseDownload, m_work, &UpdateWorker::pauseDownload);
    connect(updateWidget, &UpdateCtrlWidget::requestResumeDownload, m_work, &UpdateWorker::resumeDownload);
    connect(updateWidget, &UpdateCtrlWidget::requestInstallUpdates, m_work, &UpdateWorker::distUpgrade);
    connect(updateWidget, &UpdateCtrlWidget::notifyUpdateState, this, &UpdateWidget::onNotifyUpdateState);
    updateWidget->setSystemVersion(m_systemVersion);

    m_centerLayout->addWidget(updateWidget);
    updateWidget->show();

    // prohibit dde-offline-upgrader from showing while this page is showing.
    QDBusConnection::sessionBus().registerService(OfflineUpgraderService);
}

void UpdateWidget::showUpdateSetting()
{
    qDebug() << Q_FUNC_INFO;
    resetUpdateCheckState(false);
    m_centerWidget->setVisible(true);

    m_work->checkNetselect();

    UpdateSettings *updateSetting = new UpdateSettings(m_model);
#ifndef DISABLE_SYS_UPDATE_MIRRORS
    m_work->refreshMirrors();
#endif
    updateSetting = new UpdateSettings(m_model);
    updateSetting->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(updateSetting, &UpdateSettings::requestSetAutoUpdate, m_work, &UpdateWorker::setAutoDownloadUpdates);
    connect(updateSetting, &UpdateSettings::requestShowMirrorsView, this, &UpdateWidget::pushMirrorsView);
    connect(updateSetting, &UpdateSettings::requestSetAutoCleanCache, m_work, &UpdateWorker::setAutoCleanCache);
    connect(updateSetting, &UpdateSettings::requestSetAutoCheckUpdates, m_work, &UpdateWorker::setAutoCheckUpdates);
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    connect(updateSetting, &UpdateSettings::requestSetSourceCheck, m_work, &UpdateWorker::setSourceCheck);
#endif
    connect(updateSetting, &UpdateSettings::requestEnableSmartMirror, m_work, &UpdateWorker::setSmartMirror);

    m_centerLayout->addWidget(updateSetting);
    updateSetting->show();
}

void UpdateWidget::onTopListviewCliecked(const QModelIndex &index)
{
    if (m_currentIndex == index) return;

    QLayoutItem *item;
    while ((item = m_centerLayout->layout()->takeAt(0)) != nullptr) {
        item->widget()->deleteLater();
        delete item;
    }

    switch (static_cast<updateType>(index.row())) {
    case CHECK_UPDATE:
        showCheckUpdate();
        break;
    case UPDATE_SETTING:
        showUpdateSetting();
        break;
    default:
        break;
    }

    m_currentIndex = index;
}

void UpdateWidget::onNotifyUpdateState(int state)
{
    if (m_updateState == static_cast<UpdatesStatus>(state)) {
        return;
    } else {
        m_updateState = static_cast<UpdatesStatus>(state);
    }

    m_label->setVisible(false);
    m_historyBtn->setVisible(false);

    switch (m_updateState) {
    case Checking:
        m_label->setVisible(true);
        break;
    case Updated:
    case UpdateSucceeded:
    case NeedRestart:
        m_historyBtn->setVisible(true);
        break;
    case UpdatesAvailable:
    case Downloading:
    case DownloadPaused:
    case Downloaded:
    case Installing:
    case UpdateFailed:
    case NoNetwork:
    case NoSpace:
    case DeependenciesBrokenError:
        //now donothing
        break;
    default:
        break;
    }
}

void UpdateWidget::onAppendApplist(const QList<AppUpdateInfo> &infos)
{
    qDebug() << infos.count();
    QLayoutItem *item;
    while ((item = m_applistGroup->layout()->takeAt(0)) != nullptr) {
        item->widget()->deleteLater();
        delete item;
    }

    for (const AppUpdateInfo &info : infos) {
        UpdateItem *item = new UpdateItem();
        item->setAppInfo(info);

        m_applistGroup->appendItem(item);
    }
}
