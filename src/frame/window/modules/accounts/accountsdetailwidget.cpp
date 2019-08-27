/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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
#include "accounntfingeitem.h"
#include "modules/accounts/removeuserdialog.h"

#include <DIconButton>

#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QLineEdit>
#include <QCommandLinkButton>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

AccountsDetailWidget::AccountsDetailWidget(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_headLayout(new QVBoxLayout)
    , m_modifydelLayout(new QHBoxLayout)
    , m_setloginLayout(new QVBoxLayout)
    , m_setfingeLayout(new QVBoxLayout)
    , m_fingepasswdLayout(new QHBoxLayout)
    , m_mainContentLayout(new QVBoxLayout(this))
    , m_shortnameLayout(new QHBoxLayout)
    , m_fullnameLayout(new QHBoxLayout)
    , m_avatar(new AvatarWidget)
    , m_shortName(new QLabel)
    , m_fullName(new QLabel)
    , m_inputLineEdit(new QLineEdit)
    , m_modifyPassword(new QPushButton)
    , m_deleteAccount(new QPushButton)
    , m_autoLogin(new SwitchWidget)
    , m_nopasswdLogin(new SwitchWidget)
    , m_avatarListWidget(new AvatarListWidget)
    , m_shortnameBtn(new QLabel)
    , m_fullnameBtn(new DIconButton(this))
    , m_listGrp(new SettingsGroup)
    , m_fingetitleLabel(new QLabel)
    , m_addBtn(new QCommandLinkButton)
    , m_clearBtn(new QCommandLinkButton)
{
    initWidgets();
    initDatas();
}

void AccountsDetailWidget::initWidgets()
{
    setLayout(m_mainContentLayout);

    m_shortnameBtn->setPixmap(QIcon::fromTheme("dcc_avatar").pixmap(QSize(12, 12)));
    m_fullnameBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_fullnameBtn->setIconSize(QSize(12, 12));

    m_shortnameLayout->addWidget(m_shortnameBtn, 0, Qt::AlignRight);
    m_shortnameLayout->addWidget(m_shortName, 0, Qt::AlignLeft);

    m_fullnameLayout->addWidget(m_fullName, 2, Qt::AlignRight);
    m_fullnameLayout->addWidget(m_fullnameBtn, 1, Qt::AlignLeft);

    m_headLayout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    m_headLayout->addLayout(m_shortnameLayout);
    m_headLayout->addLayout(m_fullnameLayout);
    m_headLayout->addWidget(m_inputLineEdit, 0, Qt::AlignHCenter);

    m_modifydelLayout->addWidget(m_modifyPassword);
    m_modifydelLayout->addWidget(m_deleteAccount);

    m_setloginLayout->addWidget(m_autoLogin);
    m_setloginLayout->addWidget(m_nopasswdLogin);

    m_fingepasswdLayout->addWidget(m_fingetitleLabel, 0, Qt::AlignLeft);
    m_fingepasswdLayout->addWidget(m_clearBtn, 0, Qt::AlignRight);

    m_setfingeLayout->addLayout(m_fingepasswdLayout);
    m_setfingeLayout->addWidget(m_listGrp);
    m_setfingeLayout->addWidget(m_addBtn, 0, Qt::AlignLeft);

    m_mainContentLayout->addLayout(m_headLayout);
    m_mainContentLayout->addLayout(m_modifydelLayout);
    m_mainContentLayout->addLayout(m_setloginLayout);
    m_mainContentLayout->addLayout(m_setfingeLayout);
    m_mainContentLayout->addStretch();

    m_headLayout->setContentsMargins(0, 0, 0, 0);
    m_headLayout->setMargin(0);

    m_shortName->setFixedHeight(20);
    m_fullName->setFixedHeight(20);
    m_avatar->setAlignment(Qt::AlignHCenter);
    m_setloginLayout->setContentsMargins(0, 0, 0, 0);
    m_setloginLayout->setSpacing(0);
    m_setloginLayout->setMargin(0);

    m_modifydelLayout->setContentsMargins(1, 0, 1, 0);
    m_modifydelLayout->setSpacing(10);
    m_modifydelLayout->setMargin(3);

    m_avatarListWidget->setUserModel(m_curUser);

    const bool isOnline = m_curUser->online();
    m_deleteAccount->setEnabled(!isOnline);

    bool isCurUser = m_curUser->isCurrentUser();
    m_modifyPassword->setEnabled(isCurUser);
    m_autoLogin->setEnabled(isCurUser);
    m_nopasswdLogin->setEnabled(isCurUser);

    m_inputLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    m_inputLineEdit->setVisible(false);
}

void AccountsDetailWidget::initDatas()
{
    connect(m_avatarListWidget, &AvatarListWidget::requestAddNewAvatar, this, &AccountsDetailWidget::requestAddNewAvatar);
    connect(m_curUser, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);
    connect(m_curUser, &User::nopasswdLoginChanged, m_nopasswdLogin, &SwitchWidget::setChecked);
    connect(m_deleteAccount, &QPushButton::clicked, this, &AccountsDetailWidget::deleteUserClicked);
    connect(m_modifyPassword, &QPushButton::clicked, [ = ] {
        Q_EMIT requestShowPwdSettings(m_curUser);
    });
    connect(m_autoLogin, &SwitchWidget::checkedChanged, [ = ](const bool autoLogin) {
        Q_EMIT requestSetAutoLogin(m_curUser, autoLogin);
    });
    connect(m_nopasswdLogin, &SwitchWidget::checkedChanged, [ = ](const bool nopasswdLogin) {
        Q_EMIT requestNopasswdLogin(m_curUser, nopasswdLogin);
    });
    connect(m_avatar, &AvatarWidget::clicked, this, [ = ](const QString & iconPath) {
        Q_UNUSED(iconPath)
        setAvatarListWgtVisible(true);
        setFingerWgtsVisible(false);
    });
    connect(m_avatarListWidget, &AvatarListWidget::requestSetAvatar, this, [ = ](const QString & avatarPath) {
        Q_EMIT requestSetAvatar(m_curUser, avatarPath);
        setAvatarListWgtVisible(false);
        setFingerWgtsVisible(m_model->isVaild());
    });
    connect(m_curUser, &User::currentAvatarChanged, m_avatar, &AvatarWidget::setAvatarPath);
    connect(m_curUser, &User::nameChanged, m_shortName, &QLabel::setText);
    connect(m_curUser, &User::fullnameChanged, m_fullName, &QLabel::setText);
    connect(m_fullnameBtn, &DIconButton::clicked, this, [ = ]() {
        m_inputLineEdit->setFocus();
        updateLineEditDisplayStyle();
    });
    connect(m_addBtn, &QCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestAddThumbs(m_curUser->name(), m_notUseThumb);
    });
    connect(m_clearBtn, &QCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestCleanThumbs(m_curUser);
    });
    connect(m_curUser, &User::nopasswdLoginChanged, m_nopasswdLogin, &SwitchWidget::setChecked);
    connect(m_curUser, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);

    connect(m_inputLineEdit, &QLineEdit::editingFinished, this, [ = ]() {
        Q_EMIT requestShowFullnameSettings(m_curUser, m_inputLineEdit->text());
        updateLineEditDisplayStyle();
    });

    //use m_curUser fill widget data
    m_avatar->setAvatarPath(m_curUser->currentAvatar());
    m_shortName->setText(m_curUser->name());
    m_fullName->setText(m_curUser->fullname());

    m_modifyPassword->setText(tr("Change Password"));
    m_deleteAccount->setText(tr("Delete Account"));

    m_autoLogin->setTitle(tr("Auto Login"));
    m_autoLogin->setChecked(m_curUser->autoLogin());
    m_nopasswdLogin->setTitle(tr("Login Without Password"));
    m_nopasswdLogin->setChecked(m_curUser->nopasswdLogin());

    m_fingetitleLabel->setText(tr("Fingerprint Password"));
    m_addBtn->setText(tr("Add fingerprint"));
    m_clearBtn->setText(tr("Delete fingerprint"));
}

void AccountsDetailWidget::updateLineEditDisplayStyle()
{
    const bool visible = m_inputLineEdit->isVisible();

    m_fullName->setVisible(visible);
    m_fullnameBtn->setVisible(visible);
    m_inputLineEdit->setVisible(!visible);

    if (!visible) {
        m_inputLineEdit->setText(m_fullName->text());
        m_inputLineEdit->selectAll();
    }
}

void AccountsDetailWidget::setFingerModel(FingerModel *model)
{
    m_model = model;
    connect(model, &FingerModel::vaildChanged, this, &AccountsDetailWidget::setFingerWgtsVisible);
    setFingerWgtsVisible(model->isVaild());
    connect(model, &FingerModel::thumbsListChanged, this, &AccountsDetailWidget::onThumbsListChanged);
    onThumbsListChanged(model->thumbsList());
}

void AccountsDetailWidget::setFingerWgtsVisible(bool visible)
{
    m_fingetitleLabel->setVisible(visible);
    m_listGrp->setVisible(visible);
    m_addBtn->setVisible(visible);
    m_clearBtn->setVisible(visible);
}

void AccountsDetailWidget::setAvatarListWgtVisible(bool visible)
{
    m_avatarListWidget->setVisible(visible);
    if (visible) {
        m_modifydelLayout->addWidget(m_avatarListWidget);
    } else {
        m_modifydelLayout->removeWidget(m_avatarListWidget);
    }
    m_modifyPassword->setVisible(!visible);
    m_deleteAccount->setVisible(!visible);
    m_autoLogin->setVisible(!visible);
    m_nopasswdLogin->setVisible(!visible);
}

//删除账户
void AccountsDetailWidget::deleteUserClicked()
{
    RemoveUserDialog d(m_curUser);
    int ret = d.exec();

    if (ret == 1) {
        Q_EMIT requestDeleteAccount(m_curUser, d.deleteHome());
        Q_EMIT requestBack();
    }
}

void AccountsDetailWidget::onThumbsListChanged(const QList<FingerModel::UserThumbs> &thumbs)
{
    QStringList thumb = thumbsLists;

    m_listGrp->clear();

    for (int n = 0; n < 10 && n < thumbs.size(); ++n) {
        auto u = thumbs.at(n);
        if (u.username != m_curUser->name()) {
            continue;
        }

        int i = 1;//record fingerprint number
        Q_FOREACH (const QString &title, u.userThumbs) {
            AccounntFingeItem *item = new AccounntFingeItem;
            item->setTitle(tr("Fingerprint") + QString::number(i++));
            connect(item, &AccounntFingeItem::deleteItem, this, [&]() {
                Q_EMIT requestCleanThumbs(m_curUser);
            });
            m_listGrp->appendItem(item);
            thumb.removeOne(title);
        }

        if (!thumb.isEmpty()) {
            m_notUseThumb = thumb.first();
        }

        return;
    }

    m_notUseThumb = thumb.first();
}
