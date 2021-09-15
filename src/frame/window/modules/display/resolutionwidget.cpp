/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "resolutionwidget.h"
#include "modules/display/displaymodel.h"

#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;
DWIDGET_USE_NAMESPACE

ResolutionWidget::ResolutionWidget(int comboxWidth, QWidget *parent)
    : SettingsItem(parent)
    , m_contentLayout(new QHBoxLayout(this))
    , m_resolutionCombox(new QComboBox(this))
    , m_model(nullptr)
    , m_monitor(nullptr)
    , m_resoItemModel(new QStandardItemModel)
{
    //初始化列表无法进行静态翻译
    //~ contents_path /display/Resolution
    m_resolutionLabel = new QLabel(tr("Resolution"), this);

    setAccessibleName("ResolutionWidget");
    addBackground();
    setMinimumHeight(48);
    m_contentLayout->setContentsMargins(10, 0, 10, 0);
    m_contentLayout->addWidget(m_resolutionLabel);
    m_contentLayout->addWidget(m_resolutionCombox);
    m_resolutionCombox->setFocusPolicy(Qt::NoFocus);
    m_resolutionCombox->setMinimumWidth(comboxWidth);
    m_resolutionCombox->setMinimumHeight(36);
    m_resolutionCombox->setModel(m_resoItemModel);
    setLayout(m_contentLayout);
}

void ResolutionWidget::setModel(DisplayModel *model, Monitor *monitor)
{
    m_model = model;
    m_resolutionCombox->setEnabled(m_model->resolutionRefreshEnable());

    connect(m_model, &DisplayModel::monitorListChanged, this, &ResolutionWidget::initResolution);
    connect(m_model, &DisplayModel::displayModeChanged, this, &ResolutionWidget::initResolution);
    connect(m_model, &DisplayModel::resolutionRefreshEnableChanged, m_resolutionCombox, &QComboBox::setEnabled);

    setMonitor(monitor);
}

void ResolutionWidget::setMonitor(Monitor *monitor)
{
    if (monitor == nullptr || m_monitor == monitor) {
        return;
    }

    // 先断开信号，设置数据再连接信号
    if (m_monitor != nullptr) {
        disconnect(m_monitor, &Monitor::modelListChanged, this, &ResolutionWidget::initResolution);
        disconnect(m_monitor, &Monitor::currentModeChanged, this, nullptr);
    }

    m_monitor = monitor;

    initResolution();

    connect(m_monitor, &Monitor::modelListChanged, this, &ResolutionWidget::initResolution);
    connect(m_monitor, &Monitor::currentModeChanged, this, [=](const Resolution &mode) {
        // 规避mode == 0
        if (mode.id() == 0) {
            return;
        }

        for (int idx = 0; idx < m_resoItemModel->rowCount(); ++idx) {
            auto item = m_resoItemModel->item(idx);
            auto w = item->data(WidthRole).toInt();
            auto h = item->data(HeightRole).toInt();
            if (w == mode.width() && h == mode.height() && m_resolutionCombox->currentIndex() != item->row()) {
                m_resolutionCombox->setCurrentIndex(item->row());
                break;
            }
        }
    });
}

void ResolutionWidget::initResolution()
{
    if (m_monitor == nullptr) {
        return;
    }

    // 先断开信号，设置数据再连接信号
    if (m_resoItemModel != nullptr) {
        disconnect(m_resolutionCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, nullptr);
        m_resoItemModel->clear();
    }

    auto modeList = m_monitor->modeList();
    auto curMode = m_monitor->currentMode();

    Resolution preMode;
    for (auto mode : modeList) {
        if (Monitor::isSameResolution(preMode, mode)) {
            continue;
        }

        preMode = mode;
        if (m_model->displayMode() == MERGE_MODE) {
            bool isComm = true;
            for (auto monitor : m_model->monitorList()) {
                if (!monitor->hasResolution(mode)) {
                    isComm = false;
                    break;
                }
            }

            if (!isComm) {
                continue;
            }
        }

        auto *item = new DStandardItem;
        item->setData(QVariant(mode.id()), IdRole);
        item->setData(QVariant(mode.rate()), RateRole);
        item->setData(QVariant(mode.width()), WidthRole);
        item->setData(QVariant(mode.height()), HeightRole);

        auto res = QString::number(mode.width()) + "×" + QString::number(mode.height());
        if (Monitor::isSameResolution(mode, m_monitor->bestMode())) {
            item->setText(res + QString(" (%1)").arg(tr("Recommended")));
            m_resoItemModel->insertRow(0, item);
        } else {
            item->setText(res);
            m_resoItemModel->appendRow(item);
        }

        if (Monitor::isSameResolution(curMode, mode)) {
            m_resolutionCombox->setCurrentIndex(item->row());
        }
    }

    connect(m_resolutionCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int idx) {
        auto item = m_resoItemModel->item(idx);
        auto r = item->data(IdRole).toInt();
        auto w = item->data(WidthRole).toInt();
        auto h = item->data(HeightRole).toInt();

        // 选中分辨率和当前分别率相同
        if (m_monitor->currentMode().width() == w && m_monitor->currentMode().height() == h) {
            return;
        }

        // 优先设置最好模式
        if (m_monitor->bestMode().width() == w && m_monitor->bestMode().height() == h) {
            Q_EMIT requestSetResolution(m_monitor, m_monitor->bestMode().id());
            return;
        }

        for (auto mode : m_monitor->modeList()) {
            if (mode.width() == w && mode.height() == h) {
                Q_EMIT requestSetResolution(m_monitor, mode.id());
                return;
            }
        }
        Q_EMIT requestSetResolution(m_monitor, r);
    });
}
