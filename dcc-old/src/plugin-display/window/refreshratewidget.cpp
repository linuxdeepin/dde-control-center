//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#include "refreshratewidget.h"
#include "operation/displaymodel.h"

#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

RefreshRateWidget::RefreshRateWidget(int comboxWidth, QWidget *parent)
    : SettingsItem(parent)
    , m_contentLayout(new QHBoxLayout(this))
    , m_refreshCombox(new QComboBox(this))
    , m_model(nullptr)
    , m_monitor(nullptr)
    , m_refreshItemModel(new QStandardItemModel(this))
{
    m_refreshLabel = new QLabel(tr("Refresh Rate"), this);

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
        disconnect(m_monitor, &Monitor::currentModeChanged, this, &RefreshRateWidget::OnCurrentModeChanged);
    }

    m_monitor = monitor;

    initRefreshRate();

    connect(m_monitor, &Monitor::modelListChanged, this, &RefreshRateWidget::initRefreshRate);
    connect(m_monitor, &Monitor::currentModeChanged, this, &RefreshRateWidget::OnCurrentModeChanged);
}

void RefreshRateWidget::OnCurrentModeChanged(const Resolution &mode)
{
    // 按主线逻辑，当mode=0时，在x11环境下需要特殊处理
    // 在wayland环境下，直接跳过
    if (qEnvironmentVariable("XDG_SESSION_TYPE").contains("x11")) {
        // 规避mode == 0
        if (mode.id() == 0) {
            return;
        }
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
        if (item->data(IdRole).toUInt() == mode.id()) {
            m_refreshCombox->setCurrentIndex(item->row());
            break;
        }
    }
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
        if (Monitor::isSameResolution(mode, m_monitor->bestMode())) {
            if (Monitor::isSameRatefresh(mode, m_monitor->bestMode())) {
                ref += QString(" (%1)").arg(tr("Recommended"));
            }
        } else if (first) {
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
        auto r = item->data(IdRole).toUInt();
        if (m_monitor->currentMode().id() != r) {
            Q_EMIT requestSetResolution(m_monitor, r);
        }
    });
}
