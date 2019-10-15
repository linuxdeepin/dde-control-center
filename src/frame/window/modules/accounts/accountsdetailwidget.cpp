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
#include <DWarningButton>
#include <DCommandLinkButton>

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QLineEdit>
#include <QCommandLinkButton>
#include <QAction>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

AccountsDetailWidget::AccountsDetailWidget(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_avatar(new AvatarWidget)
    , m_shortnameBtn(new QLabel)
    , m_shortName(new QLabel)
    , m_fullName(new QLabel)
    , m_fullnameBtn(new DIconButton(this))
    , m_inputLineEdit(new QLineEdit)
    , m_mainStackedWidget(new QStackedWidget)
    , m_modifyPassword(new QPushButton)
    , m_deleteAccount(new DWarningButton)
    , m_autoLogin(new SwitchWidget)
    , m_nopasswdLogin(new SwitchWidget)
    , m_listGrp(new SettingsGroup)
    , m_fingetitleLabel(new QLabel)
    , m_addBtn(new DCommandLinkButton(""))
    , m_clearBtn(new DCommandLinkButton(""))
    , m_avatarListWidget(new AvatarListWidget)
{
    initWidgets();
    initDatas();
}

void AccountsDetailWidget::initWidgets()
{
    m_shortnameBtn->setPixmap(QIcon::fromTheme("dcc_avatar").pixmap(QSize(12, 12)));
    m_fullnameBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_fullnameBtn->setIconSize(QSize(12, 12));

    QHBoxLayout *shortnameLayout = new QHBoxLayout;
    shortnameLayout->addWidget(m_shortnameBtn, 0, Qt::AlignRight);
    shortnameLayout->addWidget(m_shortName, 0, Qt::AlignLeft);

    QHBoxLayout *fullnameLayout = new QHBoxLayout;
    fullnameLayout->addWidget(m_fullName, 2, Qt::AlignRight);
    fullnameLayout->addWidget(m_fullnameBtn, 1, Qt::AlignLeft);

    QHBoxLayout *inputlineLayout = new QHBoxLayout;
    inputlineLayout->addWidget(m_inputLineEdit);

    QVBoxLayout *headLayout = new QVBoxLayout;
    headLayout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    headLayout->addLayout(shortnameLayout);
    headLayout->addLayout(fullnameLayout);
    headLayout->addLayout(inputlineLayout);

    QHBoxLayout *modifydelLayout = new QHBoxLayout;
    modifydelLayout->addWidget(m_modifyPassword);
    modifydelLayout->addWidget(m_deleteAccount);

    QVBoxLayout *setloginLayout = new QVBoxLayout;
    setloginLayout->addWidget(m_autoLogin);
    setloginLayout->addWidget(m_nopasswdLogin);

    QHBoxLayout *passwdcancelLayout = new QHBoxLayout;
    passwdcancelLayout->addWidget(m_fingetitleLabel, 0, Qt::AlignLeft);
    passwdcancelLayout->addWidget(m_clearBtn, 0, Qt::AlignRight);

    QVBoxLayout *setfingeLayout = new QVBoxLayout;
    setfingeLayout->addLayout(passwdcancelLayout);
    setfingeLayout->addWidget(m_listGrp);
    setfingeLayout->addWidget(m_addBtn, 0, Qt::AlignLeft);

    QVBoxLayout *normalMainLayout = new QVBoxLayout;
    normalMainLayout->addLayout(modifydelLayout);
    normalMainLayout->addLayout(setloginLayout);
    normalMainLayout->addLayout(setfingeLayout);
    normalMainLayout->addStretch();

    QVBoxLayout *pictureMainLayout = new QVBoxLayout;
    pictureMainLayout->addWidget(m_avatarListWidget);

    QWidget *normalWidget = new QWidget;
    QWidget *pictureWidget = new QWidget;
    normalWidget->setLayout(normalMainLayout);
    pictureWidget->setLayout(pictureMainLayout);

    m_mainStackedWidget->insertWidget(0, normalWidget);
    m_mainStackedWidget->insertWidget(1, pictureWidget);

    QVBoxLayout *bodyLayout = new QVBoxLayout;
    bodyLayout->addWidget(m_mainStackedWidget);

    //整体布局
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->addLayout(headLayout);
    mainContentLayout->addLayout(bodyLayout);
    setLayout(mainContentLayout);

    headLayout->setContentsMargins(0, 0, 0, 0);
    headLayout->setMargin(0);

    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setMargin(0);

    normalMainLayout->setContentsMargins(0, 0, 0, 0);
    normalMainLayout->setMargin(0);

    pictureMainLayout->setContentsMargins(0, 0, 0, 0);
    pictureMainLayout->setMargin(0);

    m_mainStackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_shortName->setFixedHeight(20);
    m_fullName->setFixedHeight(20);
    m_avatar->setAlignment(Qt::AlignHCenter);
    m_avatar->setArrowed(false);

    setloginLayout->setContentsMargins(0, 0, 0, 0);
    setloginLayout->setSpacing(0);
    setloginLayout->setMargin(0);

    modifydelLayout->setContentsMargins(1, 0, 1, 0);
    modifydelLayout->setSpacing(10);
    modifydelLayout->setMargin(3);

    m_avatarListWidget->setUserModel(m_curUser);

    const bool isOnline = m_curUser->online();
    m_deleteAccount->setEnabled(!isOnline);

    bool isCurUser = m_curUser->isCurrentUser();
    m_modifyPassword->setEnabled(isCurUser);
    m_autoLogin->setEnabled(isCurUser);
    m_nopasswdLogin->setEnabled(isCurUser);

    //只有当前用户才显示指纹这块
    setFingerWgtsVisible(isCurUser);

    m_inputLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    m_inputLineEdit->setVisible(false);
    m_inputLineEdit->installEventFilter(this);

    m_mainStackedWidget->setCurrentIndex(0);
}

void AccountsDetailWidget::initDatas()
{
    connect(m_avatarListWidget, &AvatarListWidget::requestDeleteAvatar, this, [ = ](const QString & iconPath) {
        Q_EMIT requestDeleteAvatar(m_curUser, iconPath);
    });
    connect(m_avatarListWidget, &AvatarListWidget::requestAddNewAvatar, this, &AccountsDetailWidget::requestAddNewAvatar);
    connect(m_curUser, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);
    connect(m_curUser, &User::nopasswdLoginChanged, m_nopasswdLogin, &SwitchWidget::setChecked);
    connect(m_deleteAccount, &DWarningButton::clicked, this, &AccountsDetailWidget::deleteUserClicked);
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
        if (m_mainStackedWidget->currentIndex() == 0) {
            m_avatar->setArrowed(true);
            m_mainStackedWidget->setCurrentIndex(1);
        } else {
            m_avatar->setArrowed(false);
            m_mainStackedWidget->setCurrentIndex(0);
            setFingerWgtsVisible(m_model->isVaild());
        }
    });
    connect(m_avatarListWidget, &AvatarListWidget::requestSetAvatar, this, [ = ](const QString & avatarPath) {
        Q_EMIT requestSetAvatar(m_curUser, avatarPath);
    });
    connect(m_curUser, &User::currentAvatarChanged, this, [ = ](const QString & avatar) {
        m_avatar->setAvatarPath(avatar);
        m_avatarListWidget->setCurrentAvatarChecked(avatar);
    });
    connect(m_curUser, &User::nameChanged, m_shortName, &QLabel::setText);
    connect(m_curUser, &User::fullnameChanged, this, [ = ](const QString & fullname) {
        //如果没有用户全名，提示"设置全名"。
        if (fullname.isEmpty()) {
            m_fullName->setText(tr("Full Name"));
        } else {
            //对用户全名做限制，如果长度超过32，就在后面显示...
            if (fullname.length() > 32) {
                QString newfullname = fullname.left(32) + QString("...");
                m_fullName->setText(newfullname);
            } else {
                m_fullName->setText(fullname);
            }
        }
    });
    connect(m_fullnameBtn, &DIconButton::clicked, this, [ = ]() {
        updateLineEditDisplayStyle();
        m_inputLineEdit->setFocus();
    });
    connect(m_addBtn, &DCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestAddThumbs(m_curUser->name(), m_notUseThumb);
    });
    connect(m_clearBtn, &DCommandLinkButton::clicked, this, [ = ] {
        Q_EMIT requestCleanThumbs(m_curUser);
        m_addBtn->setVisible(true);
    });
    connect(m_curUser, &User::nopasswdLoginChanged, m_nopasswdLogin, &SwitchWidget::setChecked);
    connect(m_curUser, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);

    connect(m_inputLineEdit, &QLineEdit::editingFinished, this, [ = ]() {
        Q_EMIT requestShowFullnameSettings(m_curUser, m_inputLineEdit->text());
        updateLineEditDisplayStyle();
    });

    m_avatar->setAvatarPath(m_curUser->currentAvatar());
    m_avatarListWidget->setCurrentAvatarChecked(m_curUser->currentAvatar());
    m_shortName->setText(m_curUser->name());

    QString fullname = m_curUser->fullname();
    //如果没有用户全名，提示"设置全名"。
    if (fullname.isEmpty()) {
        m_fullName->setText(tr("Full Name"));
    } else {
        //对用户全名做限制，如果长度超过32，就在后面显示...
        if (fullname.length() > 32) {
            QString newfullname = fullname.left(32) + QString("...");
            m_fullName->setText(newfullname);
        } else {
            m_fullName->setText(fullname);
        }
    }

    //~ contents_path /accounts/Change Password
    m_modifyPassword->setText(tr("Change Password"));
    //~ contents_path /accounts/Delete Account
    m_deleteAccount->setText(tr("Delete Account"));

    //~ contents_path /accounts/Auto Login
    m_autoLogin->setTitle(tr("Auto Login"));
    m_autoLogin->setChecked(m_curUser->autoLogin());

    //~ contents_path /accounts/Login Without Password
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
        if (m_fullName->text() == tr("Full Name")) {
            m_inputLineEdit->setText("");
        } else {
            m_inputLineEdit->setTextMargins(34, 0, 0, 0);
            m_inputLineEdit->setText(m_curUser->fullname());
            m_inputLineEdit->selectAll();
        }
    }
}

void AccountsDetailWidget::setFingerModel(FingerModel *model)
{
    m_model = model;
    connect(model, &FingerModel::vaildChanged, this, [&](const bool isVaild) {
        if (m_curUser->isCurrentUser()) {
            setFingerWgtsVisible(isVaild);
        }
    });

    if (m_curUser->isCurrentUser()) {
        setFingerWgtsVisible(model->isVaild());
    }
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
            AccounntFingeItem *item = new AccounntFingeItem(this);
            item->setTitle(tr("Fingerprint") + QString::number(i++));

            m_listGrp->appendItem(item);
            thumb.removeOne(title);
        }

        if (!thumb.isEmpty()) {
            m_notUseThumb = thumb.first();
        }

        if (i == 11) {
            m_addBtn->setVisible(false);
        }
        return;
    }

    m_notUseThumb = thumb.first();
}

bool AccountsDetailWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_inputLineEdit) {
        if(event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Return) {
                Q_EMIT requestShowFullnameSettings(m_curUser, m_inputLineEdit->text());
                m_inputLineEdit->clearFocus();
                return true;
            }
         }
    }
    return QWidget::eventFilter(obj, event);
}
