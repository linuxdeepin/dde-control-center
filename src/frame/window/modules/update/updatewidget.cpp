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
#include "updatectrlwidget.h"
#include "updatesettings.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QStackedLayout>

using namespace dcc::update;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

UpdateWidget::UpdateWidget(QWidget *parent)
    : QWidget(parent)
    , m_bottomLabel(new Dtk::Widget::DImageButton)
    , m_layout(new QVBoxLayout)
    , m_listview(new QListView)
    , m_model(nullptr)
    , m_work(nullptr)
    , m_centerLayout(new QStackedLayout)
{
    QWidget *centerWidget = new QWidget;
    centerWidget->setFixedSize(200, 60);
    centerWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_listview->setFlow(QListView::LeftToRight);

    auto model = new QStandardItemModel(this);
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-calendar"), tr("检查更新")));
    model->appendRow(new QStandardItem(QIcon::fromTheme("dde-file-manager"), tr("更新设置"))),
          m_listview->setModel(model);
    m_defaultIndex = model->index(0, default_listview_index);

    connect(m_listview, &QListView::pressed, this, &UpdateWidget::onTopListviewCliecked);

    QHBoxLayout *centerLayout = new QHBoxLayout;
    centerLayout->addWidget(m_listview);

    centerWidget->setLayout(centerLayout);

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    topLayout->addWidget(centerWidget);
    m_layout->addLayout(topLayout);

    m_layout->addLayout(m_centerLayout);

    m_bottomLabel->setNormalPic(":/datetime/themes/light/icons/add_normal.png");
    m_bottomLabel->setHoverPic(":/datetime/themes/light/icons/add_hover.png");
    m_bottomLabel->setPressPic(":/datetime/themes/light/icons/add_press.png");
    m_bottomLabel->setCheckable(true);

    QHBoxLayout *labelLayout = new QHBoxLayout;
    labelLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    labelLayout->addWidget(m_bottomLabel);
    m_layout->addLayout(labelLayout);

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
}

void UpdateWidget::setModel(const UpdateModel *model, const UpdateWorker *work)
{
    m_model = const_cast<UpdateModel *>(model);
    m_work = const_cast<UpdateWorker *>(work);

    UpdateCtrlWidget *updateWidget = new UpdateCtrlWidget(m_model);
    connect(updateWidget, &UpdateCtrlWidget::requestDownloadUpdates, m_work, &UpdateWorker::downloadAndDistUpgrade);
    connect(updateWidget, &UpdateCtrlWidget::requestPauseDownload, m_work, &UpdateWorker::pauseDownload);
    connect(updateWidget, &UpdateCtrlWidget::requestResumeDownload, m_work, &UpdateWorker::resumeDownload);
    connect(updateWidget, &UpdateCtrlWidget::requestInstallUpdates, m_work, &UpdateWorker::distUpgrade);

    UpdateSettings *updateSetting = new UpdateSettings(m_model);
    m_work->refreshMirrors();
    updateSetting = new UpdateSettings(m_model);
    connect(updateSetting, &UpdateSettings::requestSetAutoUpdate, m_work, &UpdateWorker::setAutoDownloadUpdates);
    connect(updateSetting, &UpdateSettings::requestShowMirrorsView, this, &UpdateWidget::pushMirrorsView);
    connect(updateSetting, &UpdateSettings::requestSetAutoCleanCache, m_work, &UpdateWorker::setAutoCleanCache);
    connect(updateSetting, &UpdateSettings::requestSetAutoCheckUpdates, m_work, &UpdateWorker::setAutoCheckUpdates);
#ifndef DISABLE_SYS_UPDATE_SOURCE_CHECK
    connect(updateSetting, &UpdateSettings::requestSetSourceCheck, m_work, &UpdateWorker::setSourceCheck);
#endif
    connect(updateSetting, &UpdateSettings::requestEnableSmartMirror, m_work, &UpdateWorker::setSmartMirror);

    m_centerLayout->setMargin(0);
    m_centerLayout->setSpacing(0);
    m_centerLayout->addWidget(updateWidget);
    m_centerLayout->addWidget(updateSetting);
}

void UpdateWidget::setDefaultState()
{
    m_listview->setCurrentIndex(m_defaultIndex);
    m_listview->pressed(m_defaultIndex);
}

void UpdateWidget::showCheckUpdate()
{
    qDebug() << Q_FUNC_INFO;

    m_work->checkForUpdates();

    m_centerLayout->setCurrentIndex(0);

    // prohibit dde-offline-upgrader from showing while this page is showing.
    QDBusConnection::sessionBus().registerService(OfflineUpgraderService);
}

void UpdateWidget::showUpdateSetting()
{
    qDebug() << Q_FUNC_INFO;

    m_work->checkNetselect();

    m_centerLayout->setCurrentIndex(1);
}

void UpdateWidget::onTopListviewCliecked(const QModelIndex &index)
{
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
}
