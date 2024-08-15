//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#include <QtMath>

#include "rotatewidget.h"
#include "operation/displaymodel.h"

#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>

using namespace DCC_NAMESPACE;
RotateWidget::RotateWidget(int comboxWidth, QWidget *parent)
    : SettingsItem(parent)
    , m_contentLayout(new QHBoxLayout(this))
    , m_rotateCombox(new QComboBox(this))
    , m_model(nullptr)
    , m_monitor(nullptr)
{
    //初始化列表无法进行静态翻译
    //~ contents_path /display/Rotation
    m_rotateLabel = new QLabel(tr("Rotation"), this);

    setAccessibleName("RotateWidget");
    addBackground();
    setMinimumHeight(48);
    m_contentLayout->setContentsMargins(10, 0, 10, 0);
    m_contentLayout->addWidget(m_rotateLabel);
    m_contentLayout->addWidget(m_rotateCombox);
    m_rotateCombox->setFocusPolicy(Qt::NoFocus);
    m_rotateCombox->setMinimumWidth(comboxWidth);
    m_rotateCombox->setMinimumHeight(36);
    setLayout(m_contentLayout);

    QStringList rotateList {tr("Standard"), tr("90°"), tr("180°"), tr("270°")};
    for (int idx = 0; idx < rotateList.size(); ++idx) {
        m_rotateCombox->addItem(rotateList[idx], qPow(2, idx));
    }
}

void RotateWidget::setModel(DisplayModel *model, Monitor *monitor)
{
    m_model = model;

    connect(m_model, &DisplayModel::displayModeChanged, this, &RotateWidget::initRotate);

    setMonitor(monitor);
}

void RotateWidget::setMonitor(Monitor *monitor)
{
    if (monitor == nullptr || m_monitor == monitor) {
        return;
    }

    // 先断开信号，设置数据再连接信号
    if (m_monitor != nullptr) {
        disconnect(m_monitor, &Monitor::rotateChanged, this, &RotateWidget::initRotate);
    }

    m_monitor = monitor;

    initRotate();

    connect(m_monitor, &Monitor::rotateChanged, this, &RotateWidget::initRotate);
}

void RotateWidget::initRotate()
{
    if (m_monitor == nullptr) {
        return;
    }

    // 先断开信号，设置数据再连接信号
    disconnect(m_rotateCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, nullptr);

    auto rotate = m_monitor->rotate();
    m_rotateCombox->setCurrentIndex(m_rotateCombox->findData(rotate));

    connect(m_rotateCombox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [=](int idx) {
        Q_UNUSED(idx)
        Q_EMIT requestSetRotate(m_monitor, m_rotateCombox->currentData().value<int>());
    });
}
