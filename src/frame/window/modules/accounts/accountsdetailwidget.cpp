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

#include <QStackedWidget>
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
    , m_fullName(new QLabel)
    , m_fullNameBtn(new DIconButton(this))
    , m_inputLineEdit(new QLineEdit)
    , m_mainStackedWidget(new QStackedWidget)
    , m_fingerStackedWidget(new QStackedWidget)
    , m_fingerWidget(new FingerWidget(user, this))
    , m_avatarListWidget(new AvatarListWidget(this, true))
{
    m_avatarListWidget->setUserModel(m_curUser);

    QVBoxLayout *headLayout = new QVBoxLayout;
    QVBoxLayout *bodyLayout = new QVBoxLayout;
    initHeadPart(headLayout);
    initBodyPart(bodyLayout);

    //整体布局
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->addLayout(headLayout);
    mainContentLayout->addLayout(bodyLayout);
    setLayout(mainContentLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);
}

void AccountsDetailWidget::initHeadPart(QVBoxLayout *headLayout)
{
    AvatarWidget *avatar = new AvatarWidget;
    QLabel *shortnameBtn = new QLabel;
    QLabel *shortName = new QLabel;

    avatar->setAvatarPath(m_curUser->currentAvatar());
    avatar->setFixedSize(80, 80);
    avatar->setAlignment(Qt::AlignHCenter);
    avatar->setArrowed(false);

    shortName->setText(m_curUser->name());
    shortnameBtn->setPixmap(QIcon::fromTheme("dcc_avatar").pixmap(QSize(12, 12)));

    //如果没有用户全名，提示"设置全名"。
    QString fullname = m_curUser->fullname();
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
    m_fullNameBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_fullNameBtn->setIconSize(QSize(12, 12));
    m_fullNameBtn->setFlat(true);//设置背景透明

    QHBoxLayout *shortnameLayout = new QHBoxLayout;
    shortnameLayout->setContentsMargins(0, 0, 0, 0);
    shortnameLayout->setSpacing(3);
    shortnameLayout->setMargin(0);
    shortnameLayout->addStretch();
    shortnameLayout->addWidget(shortnameBtn, 0, Qt::AlignRight|Qt::AlignVCenter);
    shortnameLayout->addWidget(shortName, 0, Qt::AlignLeft|Qt::AlignVCenter);
    shortnameLayout->addStretch();

    QHBoxLayout *fullnameLayout = new QHBoxLayout;
    fullnameLayout->setContentsMargins(0, 0, 0, 0);
    fullnameLayout->setSpacing(3);
    fullnameLayout->setMargin(0);
    fullnameLayout->addStretch();
    fullnameLayout->addWidget(m_fullName, 0, Qt::AlignCenter);
    fullnameLayout->addStretch();

    headLayout->setContentsMargins(0, 0, 0, 0);
    headLayout->setSpacing(3);
    headLayout->setMargin(0);
    headLayout->addWidget(avatar, 0, Qt::AlignHCenter);
    headLayout->addLayout(shortnameLayout);
    headLayout->addLayout(fullnameLayout);
    headLayout->addWidget(m_inputLineEdit, 0, Qt::AlignHCenter);

    m_inputLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_inputLineEdit->setVisible(false);
    m_inputLineEdit->installEventFilter(this);

    //点击用户图像
    connect(avatar, &AvatarWidget::clicked, this, [ = ](const QString &iconPath) {
        Q_UNUSED(iconPath)
        if (m_mainStackedWidget->currentIndex() == 0) {
            avatar->setArrowed(true);
            m_mainStackedWidget->setCurrentIndex(1);
        } else {
            avatar->setArrowed(false);
            m_mainStackedWidget->setCurrentIndex(0);
            m_fingerWidget->setVisible(m_model->isVaild());
        }
    });
    //用户图像发生变化
    connect(m_curUser, &User::currentAvatarChanged, this, [ = ](const QString &iconPath) {
        avatar->setAvatarPath(iconPath);
        m_avatarListWidget->setCurrentAvatarChecked(iconPath);
    });
    //用户名发生变化
    connect(m_curUser, &User::nameChanged, shortName, &QLabel::setText);
    connect(m_curUser, &User::fullnameChanged, this, [ = ](const QString &fullname) {
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
    //点击用户全名编辑按钮
    connect(m_fullNameBtn, &DIconButton::clicked, this, [ = ]() {
        updateLineEditDisplayStyle();
        m_inputLineEdit->setFocus();
    });
    connect(m_inputLineEdit, &QLineEdit::editingFinished, this, [ = ]() {
        QString inputString = m_inputLineEdit->text();
        if (!checkStrIsAllEmpty(inputString)) {
            Q_EMIT requestShowFullnameSettings(m_curUser, inputString);
        }
        updateLineEditDisplayStyle();
    });

    m_fullName->setDisabled(true);//将设置全名变浅灰色
    m_fullName->installEventFilter(this);
}

void AccountsDetailWidget::initBodyPart(QVBoxLayout *bodyLayout)
{
    QPushButton *modifyPassword = new QPushButton;
    DWarningButton *deleteAccount = new DWarningButton;
    SettingsGroup *loginGrp = new SettingsGroup;
    SwitchWidget *autoLogin = new SwitchWidget;
    SwitchWidget *nopasswdLogin = new SwitchWidget;

    QHBoxLayout *modifydelLayout = new QHBoxLayout;
    modifydelLayout->setContentsMargins(1, 0, 1, 0);
    modifydelLayout->setSpacing(10);
    modifydelLayout->setMargin(3);
    modifydelLayout->addWidget(modifyPassword);
    modifydelLayout->addWidget(deleteAccount);

    loginGrp->appendItem(autoLogin);
    loginGrp->appendItem(nopasswdLogin);
    loginGrp->setSpacing(2);
    loginGrp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    autoLogin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    nopasswdLogin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout *normalMainLayout = new QVBoxLayout;
    normalMainLayout->setContentsMargins(0, 0, 0, 0);
    normalMainLayout->setSpacing(0);
    normalMainLayout->setMargin(0);
    normalMainLayout->addLayout(modifydelLayout, 1);
    normalMainLayout->addWidget(loginGrp, 2);

    m_fingerStackedWidget->insertWidget(0, new QWidget);//普通QWidget页面
    m_fingerStackedWidget->insertWidget(1, m_fingerWidget);//指纹模块界面
    normalMainLayout->addSpacing(30);
    normalMainLayout->addWidget(m_fingerStackedWidget, 6);

    QVBoxLayout *pictureMainLayout = new QVBoxLayout;
    pictureMainLayout->setContentsMargins(0, 0, 0, 0);
    pictureMainLayout->setSpacing(0);
    pictureMainLayout->setMargin(0);
    pictureMainLayout->addWidget(m_avatarListWidget);

    QWidget *normalWidget = new QWidget;
    QWidget *pictureWidget = new QWidget;
    normalWidget->setLayout(normalMainLayout);
    pictureWidget->setLayout(pictureMainLayout);

    m_mainStackedWidget->insertWidget(0, normalWidget);
    m_mainStackedWidget->insertWidget(1, pictureWidget);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(3);
    bodyLayout->setMargin(0);
    bodyLayout->addWidget(m_mainStackedWidget);

    m_mainStackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_fingerStackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    //修改密码，删除账户操作
    connect(modifyPassword, &QPushButton::clicked, [ = ] {
        Q_EMIT requestShowPwdSettings(m_curUser);
    });
    connect(deleteAccount, &DWarningButton::clicked, this, &AccountsDetailWidget::deleteUserClicked);

    //自动登录，无密码登录操作
    connect(m_curUser, &User::autoLoginChanged, autoLogin, &SwitchWidget::setChecked);
    connect(m_curUser, &User::nopasswdLoginChanged, nopasswdLogin, &SwitchWidget::setChecked);
    connect(autoLogin, &SwitchWidget::checkedChanged, [ = ](const bool autoLogin) {
        Q_EMIT requestSetAutoLogin(m_curUser, autoLogin);
    });
    connect(nopasswdLogin, &SwitchWidget::checkedChanged, [ = ](const bool nopasswdLogin) {
        Q_EMIT requestNopasswdLogin(m_curUser, nopasswdLogin);
    });

    //指纹界面操作
    connect(m_fingerWidget, &FingerWidget::requestAddThumbs, this, &AccountsDetailWidget::requestAddThumbs);
    connect(m_fingerWidget, &FingerWidget::requestCleanThumbs, this, &AccountsDetailWidget::requestCleanThumbs);

    //图像列表操作
    connect(m_avatarListWidget, &AvatarListWidget::requestAddNewAvatar, this, &AccountsDetailWidget::requestAddNewAvatar);
    connect(m_avatarListWidget, &AvatarListWidget::requestSetAvatar, this, [ = ](const QString &avatarPath) {
        Q_EMIT requestSetAvatar(m_curUser, avatarPath);
    });
    connect(m_avatarListWidget, &AvatarListWidget::requestDeleteAvatar, this, [ = ](const QString &iconPath) {
        Q_EMIT requestDeleteAvatar(m_curUser, iconPath);
    });
    //添加图像最终结果处理
    connect(this, &AccountsDetailWidget::requestAddNewAvatarSuccess , m_avatarListWidget, &AvatarListWidget::onAddNewAvatarSuccess);

    //图像列表选中当前用户图像
    m_avatarListWidget->setCurrentAvatarChecked(m_curUser->currentAvatar());

    //当前用户禁止使用删除按钮
    const bool isOnline = m_curUser->online();
    deleteAccount->setEnabled(!isOnline);

    //非当前用户不显示修改密码，自动登录，无密码登录
    bool isCurUser = m_curUser->isCurrentUser();
    modifyPassword->setEnabled(isCurUser);
    autoLogin->setEnabled(isCurUser);
    nopasswdLogin->setEnabled(isCurUser);

    //只有当前用户才显示指纹这块
    if (isCurUser) {
        m_fingerStackedWidget->setCurrentIndex(1);
    } else {
        m_fingerStackedWidget->setCurrentIndex(0);
    }

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

    m_mainStackedWidget->setCurrentIndex(0);
    m_fingerWidget->setVisible(false);
}

void AccountsDetailWidget::updateLineEditDisplayStyle()
{
    const bool visible = m_inputLineEdit->isVisible();
    const int distance = 15;
    m_fullName->setVisible(visible);
    m_fullNameBtn->setVisible(visible);
    m_inputLineEdit->setVisible(!visible);

    if (!visible) {
        if (m_curUser->fullname().isEmpty()) {
            m_inputLineEdit->setTextMargins(distance, 0, distance, 0);
            m_inputLineEdit->setText("");
        } else {
            m_inputLineEdit->setTextMargins(distance, 0, distance, 0);
            m_inputLineEdit->setText(m_curUser->fullname());
            m_inputLineEdit->selectAll();
        }
    }
}

void AccountsDetailWidget::setFingerModel(FingerModel *model)
{
    m_model = model;
    m_fingerWidget->setFingerModel(model);
    connect(model, &FingerModel::vaildChanged, this, [&](const bool isVaild) {
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

bool AccountsDetailWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_inputLineEdit) {
        if(event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                Q_EMIT requestShowFullnameSettings(m_curUser, m_inputLineEdit->text());
                m_inputLineEdit->clearFocus();
                return true;
            }
         }
    }

    if (obj == m_fullName) {
        if (event->type() == QEvent::Resize) {
            int fullNameWidth = m_fullName->fontMetrics().width(m_fullName->text());
            m_fullNameBtn->move(width() / 2 + fullNameWidth / 2 + 5, m_fullName->y() + 5);
            return true;
        }
    }

    return QWidget::eventFilter(obj, event);
}

void AccountsDetailWidget::resizeEvent(QResizeEvent *event)
{
    int width = event->size().width();
    int fullNameWidth = m_fullName->fontMetrics().width(m_fullName->text());
    m_fullNameBtn->move(width / 2 + fullNameWidth / 2 + 5, m_fullName->y() + 5);

    QWidget::resizeEvent(event);
}

bool AccountsDetailWidget::checkStrIsAllEmpty(const QString &str)
{
    for (const QChar &p : str) {
        if (p != QChar(' ')) {
            return false;
        }
    }

    return true;
}
