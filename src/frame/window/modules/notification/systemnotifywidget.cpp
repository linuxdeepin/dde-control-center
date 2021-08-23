/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#include "systemnotifywidget.h"
#include "notificationitem.h"
#include "timeslotitem.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"
#include "modules/notification/notificationmodel.h"
#include "modules/notification/model/sysitemmodel.h"
#include "window/utils.h"

#include <DLineEdit>
#include <DSwitchButton>
#include <DLabel>
#include <DTipLabel>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QCheckBox>
#include <QDebug>

DWIDGET_USE_NAMESPACE
using namespace dcc::widgets;
using namespace dcc::notification;
using namespace DCC_NAMESPACE::notification;

SystemNotifyWidget::SystemNotifyWidget(SysItemModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_btnDisturbMode(new DSwitchButton)
    , m_btnShowInDock(new SwitchWidget(tr("Show icon on Dock")))
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
    mainLayout->setContentsMargins(ThirdPageContentsMargins);

    setLayout(mainLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *hLayoutDisturbMode = new QHBoxLayout;
    hLayoutDisturbMode->setContentsMargins(8, 0, 10, 0);
    //~ contents_path /notification/Do Not Disturb
    //~ child_page Do Not Disturb
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

    m_btnShowInDock->addBackground();
    m_btnShowInDock->layout()->setContentsMargins(10, 0, 10, 0);
    mainLayout->addWidget(m_btnShowInDock);
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
    connect(m_model, &SysItemModel::showInDockChanged, this, [this](bool state) {
        m_btnShowInDock->setChecked(state);
    });
    m_btnShowInDock->setChecked(m_model->isShowInDock());
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
    connect(m_btnShowInDock, &SwitchWidget::checkedChanged, this, [ = ](bool state) {
        Q_EMIT requestSetSysSetting(SysItemModel::SHOWICON, state);
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
