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
#include "appnotifywidget.h"
#include "notificationitem.h"
#include "widgets/settingsgroup.h"
#include "modules/notification/notificationmodel.h"
#include "modules/notification/model/appitemmodel.h"

#include <DSwitchButton>
#include <DLabel>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QJsonObject>

DWIDGET_USE_NAMESPACE
using namespace dcc::widgets;
using namespace dcc::notification;
using namespace DCC_NAMESPACE::notification;

AppNotifyWidget::AppNotifyWidget(int &index, NotificationModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_index(index)
    , m_btnAllowNotify(new DSwitchButton)
    , m_itemNotifySound(new NotificationItem)
    , m_itemLockShowNotify(new NotificationItem)
    , m_itemShowInNotifyCenter(new NotificationItem)
    , m_itemShowNotifyPreview(new NotificationItem)
{
    initUI();
    initConnect();
}

void AppNotifyWidget::setModel(NotificationModel *model)
{
    m_model = model;
}

void AppNotifyWidget::initUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *hLayoutAllowNotify = new QHBoxLayout;
    DLabel *lblAllowNotify = new DLabel(m_model->getAppModel(m_index)->getAppName());
    DFontSizeManager::instance()->bind(lblAllowNotify, DFontSizeManager::T5, QFont::DemiBold);
    hLayoutAllowNotify->addWidget(lblAllowNotify, Qt::AlignLeft);
    hLayoutAllowNotify->addWidget(m_btnAllowNotify, Qt::AlignRight);
    mainLayout->addLayout(hLayoutAllowNotify);

    m_lblTip = new DLabel(tr("Show notifications from %1 on desktop and in the notification center.")
                                .arg(m_model->getAppModel(m_index)->getAppName()));
    DFontSizeManager::instance()->bind(m_lblTip, DFontSizeManager::T8);
    m_lblTip->adjustSize();
    m_lblTip->setWordWrap(true);
    m_lblTip->setContentsMargins(10, 5, 10, 5);
    mainLayout->addWidget(m_lblTip);

    m_settingsGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    m_settingsGrp->setContentsMargins(0, 0, 0, 0);
    m_settingsGrp->layout()->setMargin(0);
    m_settingsGrp->setSpacing(1);

    m_itemNotifySound = new NotificationItem;
    m_itemNotifySound->setTitle(tr("Play a sound"));
    m_settingsGrp->appendItem(m_itemNotifySound);
    m_itemLockShowNotify = new NotificationItem;
    m_itemLockShowNotify->setTitle(tr("Show messages on lockscreen"));
    m_settingsGrp->appendItem(m_itemLockShowNotify);
    m_itemShowInNotifyCenter = new NotificationItem;
    m_itemShowInNotifyCenter->setTitle(tr("Show in notification center"));
    m_settingsGrp->appendItem(m_itemShowInNotifyCenter);
    m_itemShowNotifyPreview = new NotificationItem;
    m_itemShowNotifyPreview->setTitle(tr("Show message preview"));
    m_settingsGrp->appendItem(m_itemShowNotifyPreview);
    mainLayout->addWidget(m_settingsGrp);

    m_settingsGrp->setVisible(m_model->getAppModel(m_index)->isAllowNotify());
    m_lblTip->setVisible(m_model->getAppModel(m_index)->isAllowNotify());
}

void AppNotifyWidget::initConnect()
{
    //set connects: model to this
    auto appModel = m_model->getAppModel(m_index);
    connect(appModel, &AppItemModel::allowNotifyChanged, this, [this](bool state) {
        m_btnAllowNotify->setChecked(state);
    });
    m_btnAllowNotify->setChecked(appModel->isAllowNotify());
    connect(appModel, &AppItemModel::lockShowNotifyChanged, this, [this](bool state) {
        m_itemLockShowNotify->setState(state);
    });
    m_itemLockShowNotify->setState(appModel->isLockShowNotify());
    connect(appModel, &AppItemModel::notifySoundChanged, this, [this](bool state) {
        m_itemNotifySound->setState(state);
    });
    m_itemNotifySound->setState(appModel->isNotifySound());
    connect(appModel, &AppItemModel::showNotifyPreviewChanged, this, [this](bool state) {
        m_itemShowNotifyPreview->setState(state);
    });
    m_itemShowNotifyPreview->setState(appModel->isShowNotifyPreview());
    connect(appModel, &AppItemModel::showInNotifyCenterChanged, this, [this](bool state) {
        m_itemShowInNotifyCenter->setState(state);
    });
    m_itemShowInNotifyCenter->setState(appModel->isShowInNotifyCenter());

    //set connects: this to module
    connect(m_btnAllowNotify, &DSwitchButton::checkedChanged, this, [ = ](bool state) {
        appModel->setAllowNotify(state);
        m_lblTip->setVisible(state);
        m_settingsGrp->setVisible(state);

        Q_EMIT requestSetAppSetting(appModel->getActName(), appModel->convertQJson());
    });
    connect(m_itemLockShowNotify, &NotificationItem::stateChanged, this, [ = ](bool state) {
        appModel->setLockShowNotify(state);

        Q_EMIT requestSetAppSetting(appModel->getActName(), appModel->convertQJson());
    });
    connect(m_itemNotifySound, &NotificationItem::stateChanged, this, [ = ](bool state) {
        appModel->setNotifySound(state);

        Q_EMIT requestSetAppSetting(appModel->getActName(), appModel->convertQJson());
    });
    connect(m_itemShowNotifyPreview, &NotificationItem::stateChanged, this, [ = ](bool state) {
        appModel->setShowNotifyPreview(state);

        Q_EMIT requestSetAppSetting(appModel->getActName(), appModel->convertQJson());
    });
    connect(m_itemShowInNotifyCenter, &NotificationItem::stateChanged, this, [ = ](bool state) {
        appModel->setShowInNotifyCenter(state);

        Q_EMIT requestSetAppSetting(appModel->getActName(), appModel->convertQJson());
    });
}
