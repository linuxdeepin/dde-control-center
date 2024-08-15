//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "systemnotifywidget.h"
#include "notificationitem.h"
#include "timeslotitem.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "src/plugin-notification/operation/notificationmodel.h"
#include "src/plugin-notification/operation/model/sysitemmodel.h"

#include <DLineEdit>
#include <DSwitchButton>
#include <DLabel>
#include <DTipLabel>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QCheckBox>
#include <QDebug>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

SystemNotifyWidget::SystemNotifyWidget(SysItemModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_btnDisturbMode(new DSwitchButton)
    , m_itemTimeSlot(new TimeSlotItem)
    , m_itemLockScreen(new NotificationItem)
{
    initUI();
    initConnect();
}

void SystemNotifyWidget::setModel(SysItemModel *model)
{
    m_model = model;
}

void SystemNotifyWidget::initUI()
{
    this->setAccessibleName("SystemNotifyWidget");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(mainLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *hLayoutDisturbMode = new QHBoxLayout;
    hLayoutDisturbMode->setContentsMargins(8, 0, 10, 0);
    //~ contents_path /notification/System Notifications
    //~ child_page System Notifications
    DLabel *lblDisturbMode = new DLabel(tr("Do Not Disturb"));
    DFontSizeManager::instance()->bind(lblDisturbMode, DFontSizeManager::T5, QFont::DemiBold);
    hLayoutDisturbMode->addWidget(lblDisturbMode, Qt::AlignLeft);
    hLayoutDisturbMode->addWidget(m_btnDisturbMode, Qt::AlignRight);
    mainLayout->addLayout(hLayoutDisturbMode);

    DTipLabel *lblTip = new DTipLabel(tr("App notifications will not be shown on desktop and the sounds will be silenced, but you can view all messages in the notification center."));
    DFontSizeManager::instance()->bind(lblTip, DFontSizeManager::T8);
    lblTip->adjustSize();
    lblTip->setWordWrap(true);
    lblTip->setContentsMargins(10, 5, 10, 5);
    lblTip->setAlignment(Qt::AlignLeft);
    mainLayout->addWidget(lblTip);

    m_settingsGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    m_settingsGrp->setContentsMargins(0, 0, 0, 0);
    m_settingsGrp->layout()->setMargin(0);
    m_settingsGrp->setSpacing(1);

    m_itemTimeSlot = new TimeSlotItem;
    m_settingsGrp->appendItem(m_itemTimeSlot);
    m_itemTimeSlot->setFixedHeight(48);

    m_itemLockScreen = new NotificationItem;
    m_itemLockScreen->setTitle(tr("When the screen is locked"));
    m_settingsGrp->appendItem(m_itemLockScreen);
    mainLayout->addWidget(m_settingsGrp);
    mainLayout->addStretch();

    m_settingsGrp->setVisible(m_btnDisturbMode->isChecked());
}

void SystemNotifyWidget::initConnect()
{
    connect(m_model, &SysItemModel::disturbModeChanged, this, [this](bool state) {
        m_btnDisturbMode->setChecked(state);
        m_settingsGrp->setVisible(state);
    });
    m_btnDisturbMode->setChecked(m_model->isDisturbMode());
    m_settingsGrp->setVisible(m_model->isDisturbMode());
    connect(m_model, &SysItemModel::timeSlotChanged, this, [this](bool state) {
        m_itemTimeSlot->setState(state);
    });
    m_itemTimeSlot->setState(m_model->isTimeSlot());
    connect(m_model, &SysItemModel::timeStartChanged, this, [this](QString time) {
        m_itemTimeSlot->setTimeStart(QTime::fromString(time, "hh:mm"));
    });
    m_itemTimeSlot->setTimeStart(QTime::fromString(m_model->timeStart(), "hh:mm"));
    connect(m_model, &SysItemModel::timeEndChanged, this, [this](QString time) {
        m_itemTimeSlot->setTimeEnd(QTime::fromString(time, "hh:mm"));
    });
    m_itemTimeSlot->setTimeEnd(QTime::fromString(m_model->timeEnd(), "hh:mm"));
    connect(m_model, &SysItemModel::lockScreenChanged, this, [this](bool state) {
        m_itemLockScreen->setState(state);
    });
    m_itemLockScreen->setState(m_model->isLockScreen());

    //set connects: this to module
    connect(m_btnDisturbMode, &DSwitchButton::checkedChanged, this, [ = ](bool state) {
        m_settingsGrp->setVisible(state);
        Q_EMIT requestSetSysSetting(SysItemModel::DNDMODE, state);
    });
    connect(m_itemTimeSlot, &TimeSlotItem::stateChanged, this, [ = ](bool state) {
        Q_EMIT requestSetSysSetting(SysItemModel::OPENBYTIMEINTERVAL, state);
    });
    connect(m_itemTimeSlot, &TimeSlotItem::timeStartChanged, this, [ = ](QTime time) {
        Q_EMIT requestSetSysSetting(SysItemModel::STARTTIME, time.toString("hh:mm"));
    });
    connect(m_itemTimeSlot, &TimeSlotItem::timeEndChanged, this, [ = ](QTime time) {
        Q_EMIT requestSetSysSetting(SysItemModel::ENDTIME, time.toString("hh:mm"));
    });
    connect(m_itemLockScreen, &NotificationItem::stateChanged, this, [ = ](bool state) {
        Q_EMIT requestSetSysSetting(SysItemModel::LOCKSCREENOPENDNDMODE, state);
    });
}
