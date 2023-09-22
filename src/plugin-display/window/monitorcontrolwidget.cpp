//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "monitorcontrolwidget.h"
#include "monitorsground.h"
#include "operation/displaymodel.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QIcon>
#include <DIconTheme>

DGUI_USE_NAMESPACE
using namespace DCC_NAMESPACE;

MonitorControlWidget::MonitorControlWidget(int activateHeight, QWidget *parent)
    : QFrame(parent)
    , m_screensGround(new MonitorsGround(activateHeight,this))
    , m_recognize(new QPushButton(DIconTheme::findQIcon("dcc_recognize"), tr("Identify")))
    , m_gather(new QPushButton(DIconTheme::findQIcon("dcc_gather"), tr("Gather Windows")))
    , m_effectiveReminder(new QLabel(this))
{
    m_screensGround->setAccessibleName("screensGround");

    m_recognize->setFocusPolicy(Qt::NoFocus);
    m_recognize->setMinimumWidth(106);
    m_recognize->setMinimumHeight(36);
    m_gather->setFocusPolicy(Qt::NoFocus);
    m_gather->setMinimumWidth(106);
    m_gather->setMinimumHeight(36);
    m_effectiveReminder->setAlignment(Qt::AlignCenter);

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addStretch();
    btnsLayout->addWidget(m_recognize);
    btnsLayout->setSpacing(20);
    btnsLayout->addWidget(m_gather);
    btnsLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(20);
    mainLayout->addWidget(m_screensGround);
    mainLayout->addWidget(m_effectiveReminder);
    mainLayout->addLayout(btnsLayout);

    setLayout(mainLayout);

    connect(m_recognize, &QPushButton::clicked, this, &MonitorControlWidget::requestRecognize);
    connect(m_gather, &QPushButton::clicked, this, [=] {
        Q_EMIT requestGatherWindows(QCursor::pos());
    });
    connect(m_screensGround, &MonitorsGround::requestApplySettings, this, &MonitorControlWidget::requestSetMonitorPosition);
    connect(m_screensGround, &MonitorsGround::showsecondaryScreen, this, &MonitorControlWidget::requestShowsecondaryScreen);
    connect(m_screensGround, &MonitorsGround::requestMonitorPress, this, &MonitorControlWidget::requestMonitorPress);
    connect(m_screensGround, &MonitorsGround::requestMonitorRelease, this, &MonitorControlWidget::requestMonitorRelease);
    connect(m_screensGround, &MonitorsGround::setEffectiveReminderVisible, this, &MonitorControlWidget::onSetEffectiveReminderVisible);
}

void MonitorControlWidget::setModel(DisplayModel *model, Monitor *moni)
{
    m_screensGround->setModel(model, moni);
}

void MonitorControlWidget::setMergeMode(bool val)
{
    m_screensGround->setMergeMode(val);
}

void MonitorControlWidget::setScreensMerged(const int mode)
{
    m_recognize->setVisible(mode != SINGLE_MODE);
    m_gather->setVisible(mode == EXTEND_MODE);
    m_gather->setEnabled(mode == EXTEND_MODE);
}

void MonitorControlWidget::onSetEffectiveReminderVisible(bool visible, int nEffectiveTime)
{
    if(visible)
        m_effectiveReminder->setText(tr("Screen rearrangement will take effect in %1s after changes").arg(nEffectiveTime));
    else
        m_effectiveReminder->setText("");

}

void MonitorControlWidget::onGatherEnabled(const bool enable)
{
    m_gather->setEnabled(enable);
}
