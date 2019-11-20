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
#include <DFontSizeManager>
#include <DApplicationHelper>
#include <DFontSizeManager>

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QLineEdit>
#include <QCommandLinkButton>
#include <QScrollArea>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

AccountsDetailWidget::AccountsDetailWidget(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
{
    //整体布局
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainContentLayout->setMargin(0);

    setLayout(mainContentLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameStyle(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setContentsMargins(0, 0, 0, 0);
    mainContentLayout->addWidget(scrollArea);

    auto contentLayout = new QVBoxLayout();
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    auto tw = new QWidget();
    tw->setLayout(contentLayout);
    contentLayout->setSpacing(0);
    contentLayout->setMargin(0);
    scrollArea->setWidget(tw);

    initUserInfo(contentLayout);
    initSetting(contentLayout);
}

void AccountsDetailWidget::setFingerModel(FingerModel *model)
{
    m_model = model;
    m_fingerWidget->setFingerModel(model);
    connect(model, &FingerModel::vaildChanged, this, [this](const bool isVaild) {
        if (m_curUser->isCurrentUser()) {
            m_fingerWidget->setVisible(isVaild);
        }
    });

    if (m_curUser->isCurrentUser()) {
        m_fingerWidget->setVisible(model->isVaild());
    }
}

//删除账户
void AccountsDetailWidget::deleteUserClicked()
{
    RemoveUserDialog d(m_curUser);
    int ret = d.exec();

    if (ret == 1) {
        Q_EMIT requestDeleteAccount(m_curUser, d.deleteHome());
    }
}

void AccountsDetailWidget::initUserInfo(QVBoxLayout *layout)
{
    layout->addSpacing(35);
    AvatarWidget *avatar = new AvatarWidget;
    layout->addWidget(avatar, 0, Qt::AlignTop | Qt::AlignHCenter);

    avatar->setAvatarPath(m_curUser->currentAvatar());
    avatar->setFixedSize(80, 80);
    avatar->setArrowed(false);

    QLabel *shortName = new QLabel;
    shortName->setEnabled(false);
    shortName->setText(m_curUser->name());
    QLabel *shortnameBtn = new QLabel(this);
    shortnameBtn->setPixmap(QIcon::fromTheme("dcc_avatar").pixmap(12, 12));

    QHBoxLayout *shortnameLayout = new QHBoxLayout;
    shortnameLayout->setMargin(0);
    shortnameLayout->setAlignment(Qt::AlignHCenter);
    shortnameLayout->addWidget(shortnameBtn);
    shortnameLayout->addSpacing(3);
    shortnameLayout->addWidget(shortName);
    layout->addSpacing(5);
    layout->addLayout(shortnameLayout);

    m_fullName = new QLabel;
    m_fullName->setContentsMargins(0, 6, 0, 6);

    auto fullname = m_curUser->fullname();
    m_fullName->setEnabled(true);
    if (fullname.isEmpty()) {
        fullname = tr("Full Name");
        m_fullName->setEnabled(false);
    } else if (fullname.size() > 32) {
        fullname = fullname.left(32) + QString("...");
    }
    m_fullName->setText(fullname);

    m_fullNameBtn = new DIconButton(this);
    m_fullNameBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_fullNameBtn->setIconSize(QSize(12, 12));
    m_fullNameBtn->setFlat(true);//设置背景透明

    m_inputLineEdit = new QLineEdit;
    m_inputLineEdit->setMinimumWidth(220);
    m_inputLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_inputLineEdit->setVisible(false);
    m_inputLineEdit->setFrame(false);
    m_inputLineEdit->setAlignment(Qt::AlignCenter);

    DFontSizeManager::instance()->bind(m_fullName, DFontSizeManager::T5);
    DFontSizeManager::instance()->bind(m_inputLineEdit, DFontSizeManager::T5);

    QHBoxLayout *fullnameLayout = new QHBoxLayout;
    fullnameLayout->setMargin(0);
    fullnameLayout->setSpacing(5);
    fullnameLayout->setAlignment(Qt::AlignHCenter);
    fullnameLayout->addWidget(m_fullName);
    fullnameLayout->addWidget(m_fullNameBtn);
    fullnameLayout->addWidget(m_inputLineEdit);
    layout->addLayout(fullnameLayout);

    m_avatarListWidget = new AvatarListWidget(this, true);
    m_avatarListWidget->setUserModel(m_curUser);
    m_avatarListWidget->setVisible(false);
    m_avatarListWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_avatarListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(m_avatarListWidget, 0);

    //点击用户图像
    connect(avatar, &AvatarWidget::clicked, this, [ = ](const QString &iconPath) {
        Q_UNUSED(iconPath)
        avatar->setArrowed(!avatar->arrowed());
        m_avatarListWidget->setVisible(avatar->arrowed());
    });

    //用户图像发生变化
    connect(m_curUser, &User::currentAvatarChanged, this, [ = ](const QString &iconPath) {
        avatar->setAvatarPath(iconPath);
        m_avatarListWidget->setCurrentAvatarChecked(iconPath);
    });
    //用户名发生变化
    connect(m_curUser, &User::nameChanged, shortName, &QLabel::setText);
    connect(m_curUser, &User::fullnameChanged, this, [ = ](const QString &fullname) {
        auto tstr = fullname;
        m_fullName->setEnabled(true);
        if (fullname.isEmpty()) {
            tstr = tr("Full Name");
            m_fullName->setEnabled(false);
        } else if (fullname.size() > 32) {
            tstr = fullname.left(32) + QString("...");
        }
        m_fullName->setText(tstr);
    });

    //点击用户全名编辑按钮
    connect(m_fullNameBtn, &DIconButton::clicked, this, [ = ]() {
        updateLineEditDisplayStyle(true);
        m_inputLineEdit->setFocus();
    });
    connect(m_inputLineEdit, &QLineEdit::editingFinished, this, [ = ]() {
        Q_EMIT requestShowFullnameSettings(m_curUser, m_inputLineEdit->text());
        updateLineEditDisplayStyle();
    });
}

void AccountsDetailWidget::initSetting(QVBoxLayout *layout)
{
    QPushButton *modifyPassword = new QPushButton;
    DWarningButton *deleteAccount = new DWarningButton;

    QHBoxLayout *modifydelLayout = new QHBoxLayout;
    modifydelLayout->setContentsMargins(10, 0, 10, 0);
    modifydelLayout->addWidget(modifyPassword);
    modifydelLayout->addSpacing(10);
    modifydelLayout->addWidget(deleteAccount);
    layout->addSpacing(40);
    layout->addLayout(modifydelLayout);

    SwitchWidget *autoLogin = new SwitchWidget;
    SwitchWidget *nopasswdLogin = new SwitchWidget;
    SettingsGroup *loginGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);

    loginGrp->setContentsMargins(0, 0, 0, 0);
    loginGrp->layout()->setMargin(0);
    loginGrp->appendItem(autoLogin);
    loginGrp->appendItem(nopasswdLogin);
    layout->addSpacing(20);
    layout->addWidget(loginGrp);

    m_fingerWidget = new FingerWidget(m_curUser, this);
    m_fingerWidget->setContentsMargins(0, 0, 0, 0);
    m_fingerWidget->layout()->setMargin(0);
    m_fingerWidget->setVisible(false);
    layout->addSpacing(30);
    layout->addWidget(m_fingerWidget);

    //非当前用户不显示修改密码，自动登录，无密码登录
    bool isCurUser = m_curUser->isCurrentUser();
    modifyPassword->setEnabled(isCurUser);
    autoLogin->setEnabled(isCurUser);
    nopasswdLogin->setEnabled(isCurUser);

    //~ contents_path /accounts/Accounts Detail
    modifyPassword->setText(tr("Change Password"));
    //~ contents_path /accounts/Accounts Detail
    deleteAccount->setText(tr("Delete Account"));

    //~ contents_path /accounts/Accounts Detail
    autoLogin->setTitle(tr("Auto Login"));
    autoLogin->setChecked(m_curUser->autoLogin());

    //~ contents_path /accounts/Accounts Detail
    nopasswdLogin->setTitle(tr("Login Without Password"));
    nopasswdLogin->setChecked(m_curUser->nopasswdLogin());

    //当前用户禁止使用删除按钮
    const bool isOnline = m_curUser->online();
    deleteAccount->setEnabled(!isOnline);

    //修改密码，删除账户操作
    connect(modifyPassword, &QPushButton::clicked, [ = ] {
        Q_EMIT requestShowPwdSettings(m_curUser);
    });
    connect(deleteAccount, &DWarningButton::clicked, this, &AccountsDetailWidget::deleteUserClicked);

    //自动登录，无密码登录操作
    connect(m_curUser, &User::autoLoginChanged, autoLogin, &SwitchWidget::setChecked);
    connect(m_curUser, &User::nopasswdLoginChanged,
            nopasswdLogin, &SwitchWidget::setChecked);
    connect(autoLogin, &SwitchWidget::checkedChanged,
    this, [ = ](const bool autoLogin) {
        Q_EMIT requestSetAutoLogin(m_curUser, autoLogin);
    });
    connect(nopasswdLogin, &SwitchWidget::checkedChanged,
    this, [ = ](const bool nopasswdLogin) {
        Q_EMIT requestNopasswdLogin(m_curUser, nopasswdLogin);
    });

    //指纹界面操作
    connect(m_fingerWidget, &FingerWidget::requestAddThumbs, this,
            &AccountsDetailWidget::requestAddThumbs);
    connect(m_fingerWidget, &FingerWidget::requestCleanThumbs, this,
            &AccountsDetailWidget::requestCleanThumbs);

    //图像列表操作
    connect(m_avatarListWidget, &AvatarListWidget::requestAddNewAvatar,
            this, &AccountsDetailWidget::requestAddNewAvatar);
    connect(m_avatarListWidget, &AvatarListWidget::requestSetAvatar,
    this, [ = ](const QString &avatarPath) {
        Q_EMIT requestSetAvatar(m_curUser, avatarPath);
    });
    connect(m_avatarListWidget, &AvatarListWidget::requestDeleteAvatar,
    this, [ = ](const QString &iconPath) {
        Q_EMIT requestDeleteAvatar(m_curUser, iconPath);
    });
    //添加图像最终结果处理
    connect(this, &AccountsDetailWidget::requestAddNewAvatarSuccess,
            m_avatarListWidget, &AvatarListWidget::onAddNewAvatarSuccess);
    connect(this, &AccountsDetailWidget::requestSetAvatarSuccess,
            m_avatarListWidget, &AvatarListWidget::onSetAvatarSuccess);
}

void AccountsDetailWidget::updateLineEditDisplayStyle(bool edit)
{
    qDebug() << "change edit status : " << sender();

    m_fullName->setVisible(!edit);
    m_fullNameBtn->setVisible(!edit);
    m_inputLineEdit->setVisible(edit);

    if (edit) {
        m_inputLineEdit->setText(m_curUser->fullname());
        m_inputLineEdit->selectAll();
    }
}
