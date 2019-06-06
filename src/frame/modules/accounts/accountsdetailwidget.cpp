/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "accountsdetailwidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/translucentframe.h"
#include "fingermodel.h"

#include <QVBoxLayout>
#include <QDebug>

#include "removeuserdialog.h"

using namespace dcc::widgets;
using namespace dcc::accounts;

AccountsDetailWidget::AccountsDetailWidget(User *user, QWidget *parent)
    : ContentWidget(parent),
      m_user(user),

      m_accountSettings(new SettingsGroup),
      m_modifyAvatar(new NextPageWidget),
      m_modifyFullname(new NextPageWidget),
      m_modifyPassword(new NextPageWidget),
      m_autoLogin(new SwitchWidget),
      m_nopasswdLogin(new SwitchWidget),
      m_finger(new NextPageWidget),
      m_deleteAccount(new QPushButton)
{
    m_modifyAvatar->setTitle(tr("Modify Avatar"));
    m_accountSettings->appendItem(m_modifyAvatar);

    m_modifyFullname->setTitle(tr("Modify Fullname"));
    m_accountSettings->appendItem(m_modifyFullname);

    m_modifyPassword->setTitle(tr("Modify Password"));
    m_accountSettings->appendItem(m_modifyPassword);

    m_autoLogin->setTitle(tr("Auto Login"));
    m_autoLogin->setChecked(user->autoLogin());
    m_accountSettings->appendItem(m_autoLogin);

    m_nopasswdLogin->setTitle(tr("Login Without Password"));
    m_nopasswdLogin->setChecked(user->nopasswdLogin());
    m_accountSettings->appendItem(m_nopasswdLogin);

    m_deleteAccount->setText(tr("Delete Account"));
    m_deleteAccount->setObjectName("DeleteAccountButton");

    QLabel *tip = new QLabel(tr("Unable to delete, current user logged in"));
    tip->setContentsMargins(16, 0, 10, 0);

    m_finger->setTitle(tr("Fingerprint Password"));

    SettingsGroup *fingerGrp = new SettingsGroup;
    fingerGrp->appendItem(m_finger);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_accountSettings);
    mainLayout->addWidget(fingerGrp);
    mainLayout->addWidget(m_deleteAccount);
    mainLayout->addWidget(tip);
    mainLayout->addStretch();
    mainLayout->setMargin(0);
    mainLayout->setSpacing(10);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    connect(user, &User::nopasswdLoginChanged, m_nopasswdLogin, &SwitchWidget::setChecked);
    connect(user, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);
    connect(user, &User::onlineChanged, this, [=] (const bool online) {
        m_deleteAccount->setDisabled(online);
        tip->setVisible(online);
    });
    connect(user, &User::fullnameChanged, this, [=] {
        setTitle(user->displayName());
    });
    connect(user, &User::isCurrentUserChanged, m_modifyPassword, &NextPageWidget::setEnabled);
    connect(user, &User::isCurrentUserChanged, m_autoLogin, &SwitchWidget::setEnabled);
    connect(user, &User::isCurrentUserChanged, m_nopasswdLogin, &SwitchWidget::setEnabled);

    connect(m_deleteAccount, &QPushButton::clicked, this, &AccountsDetailWidget::deleteUserClicked);
    connect(m_autoLogin, &SwitchWidget::checkedChanged, [=] (const bool autoLogin) { Q_EMIT requestSetAutoLogin(user, autoLogin); });
    connect(m_modifyPassword, &NextPageWidget::clicked, [=] { Q_EMIT showPwdSettings(user); });
    connect(m_modifyAvatar, &NextPageWidget::clicked, [=] { Q_EMIT showAvatarSettings(user); });
    connect(m_modifyFullname, &NextPageWidget::clicked, [=] { Q_EMIT showFullnameSettings(user); });
    connect(m_nopasswdLogin, &SwitchWidget::checkedChanged, [=] (const bool nopasswdLogin) { Q_EMIT requestNopasswdLogin(user, nopasswdLogin);});
    connect(m_finger, &NextPageWidget::clicked, this, [=] { Q_EMIT showFingerSettings(user);});

    setContent(mainWidget);
    setTitle(user->displayName());

    const bool isOnline = user->online();

    m_deleteAccount->setDisabled(isOnline);
    tip->setVisible(isOnline);

    fingerGrp->setVisible(user->isCurrentUser());

    const bool isCurrent = user->isCurrentUser();
    m_modifyPassword->setEnabled(isCurrent);
    m_autoLogin->setEnabled(isCurrent);
    m_nopasswdLogin->setEnabled(isCurrent);
}

void AccountsDetailWidget::setFingerModel(FingerModel *model)
{
    m_finger->setVisible(model->isVaild());
}

void AccountsDetailWidget::deleteUserClicked()
{
    Q_EMIT requestChangeFrameAutoHide(false);

    RemoveUserDialog d(m_user);
    int ret = d.exec();

    QTimer::singleShot(300, [this] {
        Q_EMIT requestChangeFrameAutoHide(true);
    });

    if (ret == 1) {
        Q_EMIT requestDeleteAccount(m_user, d.deleteHome());
        Q_EMIT back();
    }
}
