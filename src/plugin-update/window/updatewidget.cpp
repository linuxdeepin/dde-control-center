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

#include "updatemodel.h"
#include "updatework.h"
#include "widgets/settingsgroup.h"

#include <DSysInfo>

#include <QHBoxLayout>
#include <QLabel>
#include <QStackedLayout>
#include <QStandardItemModel>
#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

UpdateWidget::UpdateWidget(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_model(nullptr)
    , m_work(nullptr)
    , m_centerLayout(new QVBoxLayout)
    , m_label(new QLabel)
    , m_lastoreHeartBeatTimer(new QTimer)
    , m_updateState(UpdatesStatus::Default)
{
    m_layout->setMargin(0);

    QWidget *recentHistoryWidget = new QWidget;
    recentHistoryWidget->setAccessibleName("Update_Widget");
    QVBoxLayout *bottomLayout = new QVBoxLayout;
    recentHistoryWidget->setLayout(bottomLayout);

    bottomLayout->setMargin(0);
    bottomLayout->setSpacing(0);
    bottomLayout->addWidget(m_label, 0, Qt::AlignCenter);

    m_layout->addWidget(recentHistoryWidget);

    m_label->setVisible(false);
    setLayout(m_layout);

    m_lastoreHeartBeatTimer->setInterval(60000);
    m_lastoreHeartBeatTimer->start();
    connect(m_lastoreHeartBeatTimer,
            &QTimer::timeout,
            this,
            &UpdateWidget::requestLastoreHeartBeat);
}

UpdateWidget::~UpdateWidget()
{
    delete m_centerLayout;
    m_centerLayout = nullptr;

    if (m_lastoreHeartBeatTimer != nullptr) {
        if (m_lastoreHeartBeatTimer->isActive()) {
            m_lastoreHeartBeatTimer->stop();
        }
        delete m_lastoreHeartBeatTimer;
        m_lastoreHeartBeatTimer = nullptr;
    }
}

void UpdateWidget::setModel(const UpdateModel *model, const UpdateWorker *work)
{
    m_model = const_cast<UpdateModel *>(model);
    m_work = const_cast<UpdateWorker *>(work);
    qRegisterMetaType<ClassifyUpdateType>("ClassifyUpdateType");

    UpdateCtrlWidget *updateWidget = new UpdateCtrlWidget(m_model);
    updateWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(this, &UpdateWidget::showUpdateCtrl, updateWidget, &UpdateCtrlWidget::onShowUpdateCtrl);
    connect(updateWidget,
            &UpdateCtrlWidget::notifyUpdateState,
            this,
            &UpdateWidget::onNotifyUpdateState);
    connect(updateWidget, &UpdateCtrlWidget::requestUpdates, this, &UpdateWidget::requestUpdates);
    connect(updateWidget,
            &UpdateCtrlWidget::requestUpdateCtrl,
            this,
            &UpdateWidget::requestUpdateCtrl);
    connect(updateWidget,
            &UpdateCtrlWidget::requestOpenAppStroe,
            this,
            &UpdateWidget::requestOpenAppStroe);
    connect(updateWidget, &UpdateCtrlWidget::requestFixError, this, &UpdateWidget::requestFixError);
    updateWidget->setSystemVersion(m_systemVersion);

    m_layout->addWidget(updateWidget);
}

void UpdateWidget::setSystemVersion(QString version)
{
    qDebug() << Q_FUNC_INFO
             << QString("%1 %2").arg(tr("Current Edition")).arg(version.toLatin1().data());

    if (m_systemVersion != version) {
        m_systemVersion = version;
    }

    m_label->setText(QString("%1 %2").arg(tr("Current Edition")).arg(m_systemVersion));
}

void UpdateWidget::showCheckUpdate()
{
    const UpdatesStatus &status = m_model->status();
    qDebug() << Q_FUNC_INFO << " current update status : " << status;

    if (status == Checking) {
        m_label->setVisible(true);
    }
    Q_EMIT showUpdateCtrl();
}

void UpdateWidget::showUpdateSetting()
{
    qDebug() << Q_FUNC_INFO;
    m_work->checkNetselect();
}

void UpdateWidget::displayUpdateContent(UpdateType index)
{
    QLayoutItem *item;
    while ((item = m_centerLayout->layout()->takeAt(0)) != nullptr) {
        item->widget()->deleteLater();
        delete item;
        item = nullptr;
    }

    switch (static_cast<UpdateType>(index)) {
    case UpdateCheck:
        showCheckUpdate();
        break;
    case UpdateSetting:
    case UpdateSettingMir:
        showUpdateSetting();
        break;
    default:
        break;
    }
}

void UpdateWidget::onNotifyUpdateState(int state)
{
    if (m_updateState == static_cast<UpdatesStatus>(state)) {
        return;
    } else {
        m_updateState = static_cast<UpdatesStatus>(state);
    }

    m_label->setVisible(false);

    switch (m_updateState) {
    case UpdatesStatus::Default:
    case Checking:
        m_label->setVisible(true);
        break;
    case Updated:
    case UpdateSucceeded:
    case NeedRestart:
        // 更新历史,暂时屏蔽显示入口,待后期再觉得是否需要此功能
        //        m_historyBtn->setVisible(true);
        break;
    case UpdatesAvailable:
    case Downloading:
    case DownloadPaused:
    case Downloaded:
    case Installing:
    case UpdateFailed:
        // now donothing
        break;
    default:
        break;
    }
}
