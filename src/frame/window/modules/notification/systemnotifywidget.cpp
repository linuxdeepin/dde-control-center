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
#include <QJsonObject>

#include <QDebug>

DWIDGET_USE_NAMESPACE
using namespace dcc::widgets;
using namespace dcc::notification;
using namespace DCC_NAMESPACE::notification;

SystemNotifyWidget::SystemNotifyWidget(NotificationModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_btnDisturbMode(new DSwitchButton)
    , m_btnShowInDock(new SwitchWidget(tr("Show icon on Dock")))
    , m_itemTimeSlot(new TimeSlotItem)
    , m_itemFullScreen(new NotificationItem)
    , m_itemProjector(new NotificationItem)
    , m_itemLockScreen(new NotificationItem)
{
    initUI();
    initConnect();
}

void SystemNotifyWidget::setModel(NotificationModel *model)
{
    m_model = model;
}

void SystemNotifyWidget::initUI()
{
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

    m_settingsGrp->setVisible(m_btnDisturbMode->isChecked());
}

void SystemNotifyWidget::initConnect()
{
    //set connects: model to this
    auto systemModel = m_model->getSystemModel();
    connect(systemModel, &SysItemModel::disturbModeChanged, this, [this](bool state) {
        m_btnDisturbMode->setChecked(state);
        m_settingsGrp->setVisible(state);
    });
    m_btnDisturbMode->setChecked(systemModel->isDisturbMode());
    m_settingsGrp->setVisible(systemModel->isDisturbMode());
    connect(systemModel, &SysItemModel::showInDockChanged, this, [this](bool state) {
        m_btnShowInDock->setChecked(state);
    });
    m_btnShowInDock->setChecked(systemModel->isShowInDock());
    connect(systemModel, &SysItemModel::timeSlotChanged, this, [this](bool state) {
        m_itemTimeSlot->setState(state);
    });
    m_itemTimeSlot->setState(systemModel->isTimeSlot());
    connect(systemModel, &SysItemModel::timeStartChanged, this, [this](QTime time) {
        m_itemTimeSlot->setTimeStart(time);
    });
    m_itemTimeSlot->setTimeStart(systemModel->timeStart());
    connect(systemModel, &SysItemModel::timeEndChanged, this, [this](QTime time) {
        m_itemTimeSlot->setTimeEnd(time);
    });
    m_itemTimeSlot->setTimeEnd(systemModel->timeEnd());
    connect(systemModel, &SysItemModel::fullScreenChanged, this, [this](bool state) {
        m_itemFullScreen->setState(state);
    });
    m_itemFullScreen->setState(systemModel->isFullScreen());
    connect(systemModel, &SysItemModel::projectorChanged, this, [this](bool state) {
        m_itemProjector->setState(state);
    });
    m_itemProjector->setState(systemModel->isProjector());
    connect(systemModel, &SysItemModel::lockScreenChanged, this, [this](bool state) {
        m_itemLockScreen->setState(state);
    });
    m_itemLockScreen->setState(systemModel->isLockScreen());

    //set connects: this to module
    connect(m_btnDisturbMode, &DSwitchButton::checkedChanged, this, [ = ](bool state) {
        m_settingsGrp->setVisible(state);

        systemModel->setDisturbMode(state);

        Q_EMIT requestSetSysSetting(systemModel->convertQJson());
    });
    connect(m_btnShowInDock, &SwitchWidget::checkedChanged, this, [ = ](bool state) {
        systemModel->setShowInDock(state);

        Q_EMIT requestSetSysSetting(systemModel->convertQJson());
    });
    connect(m_itemTimeSlot, &TimeSlotItem::stateChanged, this, [ = ](bool state) {
        systemModel->setTimeSlot(state);

        Q_EMIT requestSetSysSetting(systemModel->convertQJson());
    });
    connect(m_itemTimeSlot, &TimeSlotItem::timeStartChanged, this, [ = ](QTime time) {
        systemModel->setTimeStart(time);

        Q_EMIT requestSetSysSetting(systemModel->convertQJson());
    });
    connect(m_itemTimeSlot, &TimeSlotItem::timeEndChanged, this, [ = ](QTime time) {
        systemModel->setTimeEnd(time);

        Q_EMIT requestSetSysSetting(systemModel->convertQJson());
    });
    connect(m_itemFullScreen, &NotificationItem::stateChanged, this, [ = ](bool state) {
        systemModel->setFullScreen(state);

        Q_EMIT requestSetSysSetting(systemModel->convertQJson());
    });
    connect(m_itemProjector, &NotificationItem::stateChanged, this, [ = ](bool state) {
        systemModel->setProjector(state);

        Q_EMIT requestSetSysSetting(systemModel->convertQJson());
    });
    connect(m_itemLockScreen, &NotificationItem::stateChanged, this, [ = ](bool state) {
        systemModel->setLockScreen(state);

        Q_EMIT requestSetSysSetting(systemModel->convertQJson());
    });
}
