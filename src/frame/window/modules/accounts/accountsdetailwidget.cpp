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
#include "groupitem.h"
#include "window/utils.h"
#include "modules/accounts/usermodel.h"
#include "modules/accounts/removeuserdialog.h"

#include <DIconButton>
#include <DWarningButton>
#include <DCommandLinkButton>
#include <DFontSizeManager>
#include <DApplicationHelper>
#include <DLineEdit>
#include <DFontSizeManager>
#include <DTipLabel>

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
    , m_groupListView(nullptr)
    , m_groupItemModel(nullptr)
{
    m_isServerSystem = IsServerSystem;
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

    if (m_isServerSystem) {
        initGroups(contentLayout);
    }
}

void AccountsDetailWidget::setFingerModel(FingerModel *model)
{
    m_model = model;
    m_fingerWidget->setFingerModel(model);
    connect(model, &FingerModel::vaildChanged, this, [this](const bool isVaild) {
        if (m_curUser->isCurrentUser()) {
            m_fingerWidgetVisible = !IsServerSystem && isVaild;
            m_fingerWidget->setVisible(m_fingerWidgetVisible);
        }
    });
    if (m_curUser->isCurrentUser()) {
        m_fingerWidgetVisible = !IsServerSystem && model->isVaild();
        m_fingerWidget->setVisible(m_fingerWidgetVisible);
    }

    connect(m_model, &FingerModel::DevicesStatus, this, &AccountsDetailWidget::dealDevicesStatus);

}

void AccountsDetailWidget::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e);
    updateLineEditDisplayStyle(false);
}

//删除账户
void AccountsDetailWidget::deleteUserClicked()
{
    RemoveUserDialog d(m_curUser, this);
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
    } else if (fullname.toLocal8Bit().size() > 32) {
        for(auto i = 1; i <= fullname.size(); ++i) {
            if (fullname.left(i).toLocal8Bit().size() > 29) {
                fullname = fullname.left(i - 1) + QString("...");
                break;
            }
        }
    }
    m_fullName->setText(fullname.toHtmlEscaped());

    m_fullNameBtn = new DIconButton(this);
    m_fullNameBtn->setAccessibleName("fullName_btn");
    m_fullNameBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_fullNameBtn->setIconSize(QSize(12, 12));
    m_fullNameBtn->setFlat(true);//设置背景透明

    m_inputLineEdit = new DLineEdit();
    m_inputLineEdit->setAccessibleName("fullName_edit");
    m_inputLineEdit->setMinimumWidth(220);
    m_inputLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_inputLineEdit->setVisible(false);
    m_inputLineEdit->lineEdit()->setFrame(false);
    m_inputLineEdit->lineEdit()->setAlignment(Qt::AlignCenter);

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

    m_avatarListWidget = new AvatarListWidget(m_curUser, this);
    m_avatarListWidget->setAccessibleName("List_useravatarlist");
    m_avatarListWidget->setVisible(false);
    m_avatarListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_avatarListWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    layout->addWidget(m_avatarListWidget);

    connect(m_curUser, &User::currentAvatarChanged, m_avatarListWidget, &AvatarListWidget::setCurrentAvatarChecked);
    connect(m_inputLineEdit, &DLineEdit::textEdited, this, [ = ] {
        if (m_inputLineEdit->isAlert()){
            m_inputLineEdit->hideAlertMessage();
            m_inputLineEdit->setAlert(false);
        }
    });

    //点击用户图像
    connect(avatar, &AvatarWidget::clicked, this, [ = ](const QString &iconPath) {
        Q_UNUSED(iconPath)
        avatar->setArrowed(!avatar->arrowed());
        m_avatarListWidget->setVisible(avatar->arrowed());
    });

    connect(m_avatarListWidget, &AvatarListWidget::requesRetract, this, [ = ] {
        if (avatar->arrowed()) {
            avatar->setArrowed(!avatar->arrowed());
            m_avatarListWidget->setVisible(avatar->arrowed());
        }
    });

    connect(m_curUser, &User::currentAvatarChanged, avatar, &AvatarWidget::setAvatarPath);
    //用户名发生变化
    connect(m_curUser, &User::nameChanged, shortName, &QLabel::setText);
    connect(m_curUser, &User::fullnameChanged, this, [ = ](const QString &fullname) {
        auto tstr = fullname;
        m_fullName->setEnabled(true);
        if (fullname.isEmpty()) {
            tstr = tr("Full Name");
            m_fullName->setEnabled(false);
        } else if (fullname.toLocal8Bit().size() > 32) {
            for(auto i = 1; i <= fullname.size(); ++i) {
                if (fullname.left(i).toLocal8Bit().size() > 29) {
                    tstr = fullname.left(i - 1) + QString("...");
                    break;
                }
            }
        }
        m_fullName->setText(tstr.toHtmlEscaped());
    });

    //点击用户全名编辑按钮
    connect(m_fullNameBtn, &DIconButton::clicked, this, [ = ]() {
        updateLineEditDisplayStyle(true);
        m_inputLineEdit->lineEdit()->setFocus();
    });
    connect(m_inputLineEdit->lineEdit(), &QLineEdit::textChanged, this, [ = ]() {
        m_inputLineEdit->setAlert(false);
        m_inputLineEdit->hideAlertMessage();
    });
    connect(m_inputLineEdit->lineEdit(), &QLineEdit::editingFinished, this, [ = ]() {
        //判断账户全名是否被其他用户所用
        auto userList = m_userModel->userList();

        if(m_inputLineEdit->text() != m_curUser->fullname()) {
            for(auto u : userList){
                if(u->fullname() == m_inputLineEdit->text() && u->fullname() != nullptr) {
                    m_inputLineEdit->setAlert(true);
                    m_inputLineEdit->showAlertMessage(tr("The full name already exists"), -1);
                    return;
                }
            }

            // 判断全名长度不能超过32个字符
            if (m_inputLineEdit->text().size() > 32) {
                m_inputLineEdit->setAlert(true);
                m_inputLineEdit->showAlertMessage(tr("The full name is too long"), -1);
                return;
            }
            Q_EMIT requestShowFullnameSettings(m_curUser, m_inputLineEdit->text());
        }

        m_inputLineEdit->clearFocus();
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
    modifydelLayout->setStretchFactor(modifyPassword,1);
    modifydelLayout->setStretchFactor(deleteAccount,1);
    layout->addSpacing(40);
    layout->addLayout(modifydelLayout);

    if (m_isServerSystem) {
        auto pwHLayout = new QHBoxLayout;
        auto pwWidget = new SettingsItem;
        pwWidget->addBackground();
        layout->addSpacing(15);
        layout->addWidget(pwWidget);
        pwWidget->setLayout(pwHLayout);

        pwHLayout->addWidget(new QLabel(tr("Validity Days")), 0, Qt::AlignLeft);
        auto ageEdit = new DLineEdit();
        ageEdit->setMinimumWidth(165);
        ageEdit->lineEdit()->setPlaceholderText(tr("Always"));
        ageEdit->setText(m_curUser->passwordAge() >= 99999 ? tr("Always") : QString::number(m_curUser->passwordAge()));
        ageEdit->setClearButtonEnabled(false);
        pwHLayout->addWidget(ageEdit, 0, Qt::AlignRight);

        connect(ageEdit, &DLineEdit::textChanged, this, [ageEdit](){
            ageEdit->setAlert(false);
        });
        connect(ageEdit, &DLineEdit::textEdited, this, [ageEdit](){
            if (ageEdit->text().isEmpty())
                return;

            bool isInt(false);
            auto age = ageEdit->text().toInt(&isInt);
            if (!isInt) {
                QString strText(ageEdit->text());
                QString strTemp;
                int idx;
                for (idx = 0; idx < strText.size(); ++idx) {
                    if (strText[idx] >= '0' && strText[idx] <= '9') {
                        strTemp.append(strText[idx]);
                    } else {
                        break;
                    }
                }

                ageEdit->lineEdit()->blockSignals(true);
                ageEdit->lineEdit()->setText(strTemp);
                ageEdit->lineEdit()->setCursorPosition(idx);
                ageEdit->lineEdit()->blockSignals(false);
            } else if (age > 99999) {
                auto idx = ageEdit->lineEdit()->cursorPosition();
                ageEdit->lineEdit()->blockSignals(true);
                ageEdit->lineEdit()->setText(ageEdit->text().left(ageEdit->text().length() - 1));
                ageEdit->lineEdit()->setCursorPosition(idx - 1);
                ageEdit->lineEdit()->blockSignals(false);
            }
        });
        connect(ageEdit, &DLineEdit::editingFinished, this, [this, pwWidget, ageEdit](){
            if (ageEdit->text().isEmpty()) {
                ageEdit->lineEdit()->setText(m_curUser->passwordAge() >= 99999 ? tr("Always") : QString::number(m_curUser->passwordAge()));
                return;
            }

            bool isInt = false;
            auto age = ageEdit->text().toInt(&isInt);
            if (!isInt) {
                ageEdit->setAlert(true);
                if (ageEdit->text() == tr("Always")) ageEdit->setAlert(false);
                return;
            }

            if (age == m_curUser->passwordAge())
                return;

            if(age <= 0) {
                ageEdit->setAlert(true);
                ageEdit->setAlertMessageAlignment(Qt::AlignRight);
                ageEdit->showAlertMessage(tr("Please input a number between 1-99999"), pwWidget, 2000);
                return;
            }

            Q_EMIT requsetSetPassWordAge(m_curUser, ageEdit->text().toInt());
        });
        connect(m_curUser, &User::passwordAgeChanged, this, [ageEdit](const int age) {
            ageEdit->setText(age >= 99999 ? tr("Always") : QString::number(age));
        });
    }

    m_autoLogin = new SwitchWidget;
    m_nopasswdLogin = new SwitchWidget;
    SettingsGroup *loginGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);

    loginGrp->setContentsMargins(0, 0, 0, 0);
    loginGrp->layout()->setMargin(0);
    loginGrp->appendItem(m_autoLogin);
    loginGrp->appendItem(m_nopasswdLogin);
    //if (!IsServerSystem) {
    //    layout->addSpacing(20);
    //}
    layout->addWidget(loginGrp);
    //服务器版本不显示自动登录，无密码登录
    //loginGrp->setVisible(!IsServerSystem);
    loginGrp->setVisible(false);

    m_fingerWidget = new FingerWidget(m_curUser, this);
    m_fingerWidget->setContentsMargins(0, 0, 0, 0);
    m_fingerWidget->layout()->setMargin(0);
    layout->addSpacing(30);
    layout->addWidget(m_fingerWidget);

    //非当前用户不显示修改密码，自动登录，无密码登录,指纹页面
    bool isCurUser = m_curUser->isCurrentUser();
    modifyPassword->setEnabled(isCurUser);
    m_autoLogin->setEnabled(isCurUser);
    m_nopasswdLogin->setEnabled(isCurUser);
    m_fingerWidgetVisible = !IsServerSystem && isCurUser;
    m_fingerWidget->setVisible(m_fingerWidgetVisible);

    //~ contents_path /accounts/Accounts Detail
    modifyPassword->setText(tr("Change Password"));
    //~ contents_path /accounts/Accounts Detail
    deleteAccount->setText(tr("Delete Account"));

    //~ contents_path /accounts/Accounts Detail
    m_autoLogin->setTitle(tr("Auto Login"));
    m_autoLogin->setChecked(m_curUser->autoLogin());

    //~ contents_path /accounts/Accounts Detail
    m_nopasswdLogin->setTitle(tr("Login Without Password"));
    m_nopasswdLogin->setChecked(m_curUser->nopasswdLogin());

    //当前用户禁止使用删除按钮
    const bool isOnline = m_curUser->online();
    if (m_curUser->isCurrentUser()) {
        deleteAccount->setEnabled(false);
    } else {
        deleteAccount->setEnabled(!isOnline);
    }
    connect(m_curUser, &User::onlineChanged, deleteAccount, [ = ](const bool online) {
        deleteAccount->setEnabled(!online);
    });

    connect(m_curUser, &User::onlineChanged, this, [=] (const bool online) {
        deleteAccount->setDisabled(online);
        this->update();
    });

    //修改密码，删除账户操作
    connect(modifyPassword, &QPushButton::clicked, [ = ] {
        Q_EMIT requestShowPwdSettings(m_curUser);
    });
    connect(deleteAccount, &DWarningButton::clicked, this, &AccountsDetailWidget::deleteUserClicked);

    //自动登录，无密码登录操作
    connect(m_curUser, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);
    connect(m_curUser, &User::nopasswdLoginChanged,
            m_nopasswdLogin, &SwitchWidget::setChecked);
    connect(m_autoLogin, &SwitchWidget::checkedChanged,
    this, [ = ](const bool autoLogin) {
        Q_EMIT requestSetAutoLogin(m_curUser, autoLogin);
    });
    connect(m_nopasswdLogin, &SwitchWidget::checkedChanged,
    this, [ = ](const bool nopasswdLogin) {
        Q_EMIT requestNopasswdLogin(m_curUser, nopasswdLogin);
    });

    //指纹界面操作
    connect(m_fingerWidget, &FingerWidget::requestAddThumbs, this,
            &AccountsDetailWidget::requestAddThumbs);
    connect(m_fingerWidget, &FingerWidget::requestCleanThumbs, this,
            &AccountsDetailWidget::requestCleanThumbs);
    connect(m_fingerWidget, &FingerWidget::requestDeleteFingerItem, this, &AccountsDetailWidget::requestDeleteFingerItem);
    connect(m_fingerWidget, &FingerWidget::requestRenameFingerItem, this, &AccountsDetailWidget::requestRenameFingerItem);
    connect(m_fingerWidget, &FingerWidget::noticeEnrollCompleted, this, &AccountsDetailWidget::noticeEnrollCompleted);
    //图像列表操作
    connect(m_avatarListWidget, &AvatarListWidget::requestSetAvatar,
    this, [ = ](const QString &avatarPath) {
        Q_EMIT requestSetAvatar(m_curUser, avatarPath);
    });
}

void AccountsDetailWidget::setAccountModel(dcc::accounts::UserModel *model)
{
    if (!model) {
        return;
    }
    m_userModel = model;

    m_autoLogin->setVisible(m_userModel->isAutoLoginValid() && !IsServerSystem);
    m_nopasswdLogin->setVisible(m_userModel->isNoPassWordLoginValid() && !IsServerSystem);

    if (!m_groupItemModel)
        return;
    m_groupItemModel->clear();
    for(QString item : m_userModel->getAllGroups()) {
        GroupItem *it = new GroupItem(item);
        it->setCheckable(false);
        m_groupItemModel->appendRow(it);
    }

    changeUserGroup(m_curUser->groups());
    connect(m_curUser, &User::groupsChanged, this, &AccountsDetailWidget::changeUserGroup);
}

void AccountsDetailWidget::initGroups(QVBoxLayout *layout)
{
    QStringList userGroup = m_curUser->groups();
    m_groupListView = new DListView(this);
    m_groupItemModel = new QStandardItemModel(this);
    m_groupListView->setModel(m_groupItemModel);
    m_groupListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_groupListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_groupListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_groupListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_groupListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_groupListView->setSpacing(1);
    connect(m_groupListView, &DListView::clicked, this, &AccountsDetailWidget::userGroupClicked);
    QLabel *groupTip = new QLabel(tr("Group"));
    layout->addWidget(groupTip);
    layout->addSpacing(List_Interval);
    layout->addWidget(m_groupListView);
}

void AccountsDetailWidget::userGroupClicked(const QModelIndex &index)
{
    QStringList curUserGroup;
    int row_count = m_groupItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *itemGroup = m_groupItemModel->item(i, 0);
        if (itemGroup && itemGroup->checkState()) {
            curUserGroup << itemGroup->text();
        }
    }

    QStandardItem *item = m_groupItemModel->item(index.row() ,index.column());
    Qt::CheckState state = item->checkState();
    if (state == Qt::Checked) {
        curUserGroup.removeOne(item->text());
    } else {
        curUserGroup << item->text();
    }

    Q_EMIT requestSetGroups(m_curUser, curUserGroup);
}

void AccountsDetailWidget::dealDevicesStatus(bool status)
{
    if(m_fingerWidgetVisible == status) return;

    m_fingerWidgetVisible = status;

    m_fingerWidget->setVisible(status);

    qDebug() << "m_fingerWidgetVisible:" << m_fingerWidgetVisible;
    m_fingerWidget->displayAccountFingerItemInfo();//DebugInfo
}

void AccountsDetailWidget::changeUserGroup(const QStringList &groups)
{
    int row_count = m_groupItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_groupItemModel->item(i, 0);
        if (item && groups.contains(item->text())) {
            item->setCheckState(Qt::Checked);
        } else {
            item->setCheckState(Qt::Unchecked);
        }
    }
    m_groupItemModel->sort(0);
}

void AccountsDetailWidget::updateLineEditDisplayStyle(bool edit)
{
    auto inputFullName = m_inputLineEdit->lineEdit()->text();
    m_inputLineEdit->lineEdit()->selectAll();
    if (inputFullName.size() > 100) {
        m_inputLineEdit->setVisible(!edit);
        m_inputLineEdit->setAlert(!edit);
        m_inputLineEdit->showAlertMessage(tr("The full name is too long"), -1);
    } else {
        m_fullName->setVisible(!edit);
        m_fullNameBtn->setVisible(!edit);
        m_inputLineEdit->setVisible(edit);
    }
}
