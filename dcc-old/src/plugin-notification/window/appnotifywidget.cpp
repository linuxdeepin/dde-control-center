//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "appnotifywidget.h"
#include "notificationitem.h"
#include "widgets/settingsgroup.h"
#include "src/plugin-notification/operation/notificationmodel.h"
#include "src/plugin-notification/operation/model/appitemmodel.h"

#include <DSwitchButton>
#include <DLabel>
#include <DFontSizeManager>

#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

AppNotifyWidget::AppNotifyWidget(AppItemModel *model, QWidget *parent)
    : QWidget(parent)
    , m_model(model)
    , m_btnAllowNotify(new DSwitchButton)
    , m_itemNotifySound(new NotificationItem)
    , m_itemLockShowNotify(new NotificationItem)
    , m_itemShowInNotifyCenter(new NotificationItem)
    , m_itemShowNotifyPreview(new NotificationItem)
{
    initUI();
    initConnect();
}

void AppNotifyWidget::setModel(AppItemModel *model)
{
    m_model = model;
}

void AppNotifyWidget::initUI()
{
    this->setAccessibleName("AppNotifyWidget");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainLayout->setMargin(0);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(mainLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *hLayoutAllowNotify = new QHBoxLayout;
    DLabel *lblAllowNotify = new DLabel(m_model->getAppName());
    DFontSizeManager::instance()->bind(lblAllowNotify, DFontSizeManager::T5, QFont::DemiBold);
    hLayoutAllowNotify->addWidget(lblAllowNotify, Qt::AlignLeft);
    hLayoutAllowNotify->addWidget(m_btnAllowNotify, Qt::AlignRight);
    hLayoutAllowNotify->setContentsMargins(10, 0, 0 ,0);
    mainLayout->addLayout(hLayoutAllowNotify);

    m_lblTip = new DLabel(tr("Show notifications from %1 on desktop and in the notification center.")
                                .arg(m_model->getAppName()));
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
    m_itemNotifySound->setObjectName("NotifySound");
    m_itemNotifySound->setTitle(tr("Play a sound"));
    m_settingsGrp->appendItem(m_itemNotifySound);
    m_itemLockShowNotify = new NotificationItem;
    m_itemLockShowNotify->setObjectName("LockShowNotify");
    m_itemLockShowNotify->setTitle(tr("Show messages on lockscreen"));
    m_settingsGrp->appendItem(m_itemLockShowNotify);
    m_itemShowInNotifyCenter = new NotificationItem;
    m_itemShowInNotifyCenter->setObjectName("ShowInNotifyCenter");
    m_itemShowInNotifyCenter->setTitle(tr("Show in notification center"));
    m_settingsGrp->appendItem(m_itemShowInNotifyCenter);
    m_itemShowNotifyPreview = new NotificationItem;
    m_itemShowNotifyPreview->setObjectName("ShowNotifyPreview");
    m_itemShowNotifyPreview->setTitle(tr("Show message preview"));
    m_settingsGrp->appendItem(m_itemShowNotifyPreview);
    mainLayout->addWidget(m_settingsGrp);
    mainLayout->addStretch();

    m_settingsGrp->setVisible(m_model->isAllowNotify());
    m_lblTip->setVisible(m_model->isAllowNotify());
}

void AppNotifyWidget::initConnect()
{
    //set connects: model to this
    connect(m_model, &AppItemModel::allowNotifyChanged, this, [this](bool state) {
        m_btnAllowNotify->setChecked(state);
    });
    m_btnAllowNotify->setChecked(m_model->isAllowNotify());
    connect(m_model, &AppItemModel::lockShowNotifyChanged, this, [this](bool state) {
        m_itemLockShowNotify->setState(state);
    });
    m_itemLockShowNotify->setState(m_model->isLockShowNotify());
    connect(m_model, &AppItemModel::notifySoundChanged, this, [this](bool state) {
        m_itemNotifySound->setState(state);
    });
    m_itemNotifySound->setState(m_model->isNotifySound());
    connect(m_model, &AppItemModel::showNotifyPreviewChanged, this, [this](bool state) {
        m_itemShowNotifyPreview->setState(state);
    });
    m_itemShowNotifyPreview->setState(m_model->isShowNotifyPreview());
    connect(m_model, &AppItemModel::showInNotifyCenterChanged, this, [this](bool state) {
        m_itemShowInNotifyCenter->setState(state);
    });
    m_itemShowInNotifyCenter->setState(m_model->isShowInNotifyCenter());

    //set connects: this to module
    connect(m_btnAllowNotify, &DSwitchButton::checkedChanged, this, [ = ](bool state) {
        m_model->setAllowNotify(state);
        m_lblTip->setVisible(state);
        m_settingsGrp->setVisible(state);
        Q_EMIT requestSetAppSetting(m_model->getActName(), AppItemModel::ENABELNOTIFICATION, state);
    });
    connect(m_itemLockShowNotify, &NotificationItem::stateChanged, this, [ = ](bool state) {
        m_model->setLockShowNotify(state);
        Q_EMIT requestSetAppSetting(m_model->getActName(), AppItemModel::LOCKSCREENSHOWNOTIFICATION, state);
    });
    connect(m_itemNotifySound, &NotificationItem::stateChanged, this, [ = ](bool state) {
        m_model->setNotifySound(state);
        Q_EMIT requestSetAppSetting(m_model->getActName(), AppItemModel::ENABELSOUND, state);
    });
    connect(m_itemShowNotifyPreview, &NotificationItem::stateChanged, this, [ = ](bool state) {
        m_model->setShowNotifyPreview(state);
        Q_EMIT requestSetAppSetting(m_model->getActName(), AppItemModel::ENABELPREVIEW, state);
    });
    connect(m_itemShowInNotifyCenter, &NotificationItem::stateChanged, this, [ = ](bool state) {
        m_model->setShowInNotifyCenter(state);
        Q_EMIT requestSetAppSetting(m_model->getActName(), AppItemModel::SHOWINNOTIFICATIONCENTER, state);
    });
}
