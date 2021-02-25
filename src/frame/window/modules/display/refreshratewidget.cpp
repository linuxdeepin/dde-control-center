/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "refreshratewidget.h"
#include "modules/display/displaymodel.h"

#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE

RefreshRateWidget::RefreshRateWidget(int comboxWidth, QWidget *parent)
    : SettingsItem(parent)
    , m_contentLayout(new QHBoxLayout)
    , m_refreshLabel(new QLabel(tr("Refresh Rate")))
    , m_refreshCombox(new QComboBox)
    , m_model(nullptr)
    , m_monitor(nullptr)
    , m_refreshItemModel(new QStandardItemModel)
{
    addBackground();
    setMinimumHeight(48);
    m_contentLayout->setContentsMargins(10, 0, 10, 0);
    m_contentLayout->addWidget(m_refreshLabel);
    m_contentLayout->addWidget(m_refreshCombox);
    m_refreshCombox->setFocusPolicy(Qt::NoFocus);
    m_refreshCombox->setMinimumWidth(comboxWidth);
    m_refreshCombox->setMinimumHeight(36);
    m_refreshCombox->setModel(m_refreshItemModel);
    setLayout(m_contentLayout);
}

void RefreshRateWidget::setModel(DisplayModel *model, Monitor *monitor)
{
    m_model = model;
    m_refreshCombox->setEnabled(m_model->resolutionRefreshEnable());

    connect(m_model, &DisplayModel::monitorListChanged, this, &RefreshRateWidget::initRefreshRate);
    connect(m_model, &DisplayModel::displayModeChanged, this, &RefreshRateWidget::initRefreshRate);
    connect(m_model, &DisplayModel::resolutionRefreshEnableChanged, m_refreshCombox, &QComboBox::setEnabled);

    setMonitor(monitor);
}

void RefreshRateWidget::setMonitor(Monitor *monitor)
{
    if (monitor == nullptr || m_monitor == monitor) {
        return;
    }

    // 先断开信号，设置数据再连接信号
    if (m_monitor != nullptr) {
        disconnect(m_monitor, &Monitor::modelListChanged, this, &RefreshRateWidget::initRefreshRate);
        disconnect(m_monitor, &Monitor::currentModeChanged, this, nullptr);
    }

    m_monitor = monitor;

    initRefreshRate();

    connect(m_monitor, &Monitor::modelListChanged, this, &RefreshRateWidget::initRefreshRate);
    connect(m_monitor, &Monitor::currentModeChanged, this, [=](const Resolution &mode) {
        // 规避mode == 0
        if (mode.id() == 0) {
            return;
        }

        auto w = 0;
        auto h = 0;
        // 当前分辨率宽度和高度
        if (m_refreshCombox->currentIndex() >= 0 && m_refreshItemModel->rowCount() >= 0) {
            auto item = m_refreshItemModel->item(m_refreshCombox->currentIndex());
            w = item->data(WidthRole).toInt();
            h = item->data(HeightRole).toInt();
        }

        // 无刷新率,分辨率宽度或高度改变则重新加载刷新率
        if (m_refreshCombox->currentIndex() < 0 || m_refreshItemModel->rowCount() < 0 || w != mode.width() || h != mode.height()) {
            initRefreshRate();
        }

        for (int idx = 0; idx < m_refreshItemModel->rowCount(); ++idx) {
            auto item = m_refreshItemModel->item(idx);
            if (item->data(IdRole).toInt() == mode.id()) {
                m_refreshCombox->setCurrentIndex(item->row());
                break;
            }
        }
    });
}

void RefreshRateWidget::initRefreshRate()
{
    if (m_monitor == nullptr) {
        return;
    }

    // 先断开信号，设置数据再连接信号
    if (m_refreshItemModel != nullptr) {
        disconnect(m_refreshCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, nullptr);
        m_refreshItemModel->clear();
    }

    auto modeList = m_monitor->modeList();
    Resolution preMode;
    bool first = true;
    for (auto mode : modeList) {
        if (!Monitor::isSameResolution(mode, m_monitor->currentMode()))
            continue;

        if (m_model->displayMode() == MERGE_MODE) {
            bool isComm = true;
            for (auto monitor : m_model->monitorList()) {
                if (!monitor->hasResolutionAndRate(mode)) {
                    isComm = false;
                    break;
                }
            }

            if (!isComm) {
                continue;
            }
        }

        auto rate = mode.rate();
        DStandardItem *item = new DStandardItem;
        auto ref = QString::number(rate, 'g', 4) + tr("Hz");
        if (mode == m_monitor->bestMode() || first) {
            ref += QString(" (%1)").arg(tr("Recommended"));
            first = false;
        }

        item->setText(ref);
        item->setData(QVariant(mode.id()), IdRole);
        item->setData(QVariant(mode.rate()), RateRole);
        item->setData(QVariant(mode.width()), WidthRole);
        item->setData(QVariant(mode.height()), HeightRole);
        m_refreshItemModel->appendRow(item);

        if (mode == m_monitor->currentMode()) {
            m_refreshCombox->setCurrentIndex(item->row());
        }
    }

    connect(m_refreshCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int idx) {
        auto item = m_refreshItemModel->item(idx);
        auto r = item->data(IdRole).toInt();
        if (m_monitor->currentMode().id() != r) {
            Q_EMIT requestSetResolution(m_monitor, r);
        }
    });
}
