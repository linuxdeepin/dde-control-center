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
#include "accountfingeitem.h"
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
#include <DDesktopServices>

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QLineEdit>
#include <QCommandLinkButton>
#include <QScrollArea>
#include <QScroller>
#include <QValidator>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

AccountsDetailWidget::AccountsDetailWidget(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_groupListView(nullptr)
    , m_groupItemModel(nullptr)
    , m_avatarLayout(new QHBoxLayout)
    , m_tipDialog(nullptr)
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
    contentLayout->setContentsMargins(0, 0, 0, 0);
    scrollArea->setWidget(tw);

    /* 设置账户详情列表支持触屏滑动，不使用TouchGesture的原因，TouchGesture qt内部存在bug。1、滚动区滑动过程中，主窗口也跟随move；
     * 2、滑动回弹过程中，点击头像，窗口出现错位*/
    QScroller::grabGesture(scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(scrollArea->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

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
            m_fingerWidget->setVisible(!IsServerSystem && isVaild);
        }
    });
    if (m_curUser->isCurrentUser()) {
        m_fingerWidget->setVisible(!IsServerSystem && model->isVaild());
    }
}

bool AccountsDetailWidget::getOtherUserAutoLogin()
{
    for(auto user : m_userModel->userList()) {
        if (user->name() != m_curUser->name() && user->autoLogin()) {
            return false;
        }
    }
    return true;
}

void AccountsDetailWidget::initSetting(QVBoxLayout *layout)
{
    QPushButton *modifyPassword = new QPushButton;
    if (DGuiApplicationHelper::isTabletEnvironment()) {
        modifyPassword->setLayoutDirection(Qt::RightToLeft);
        modifyPassword->setIcon(QIcon(":/frame/themes/dark/icons/expand_normal.svg"));
    }

    QHBoxLayout *modifydelLayout = new QHBoxLayout;
    modifydelLayout->setContentsMargins(10, 0, 10, 0);
    modifydelLayout->addWidget(modifyPassword);
    modifydelLayout->addSpacing(10);
    layout->addSpacing(40);
    layout->addLayout(modifydelLayout);

    m_autoLogin = new SwitchWidget;
    m_nopasswdLogin = new SwitchWidget;
    SettingsGroup *loginGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);

    loginGrp->getLayout()->setContentsMargins(0, 0, 0, 0);
    loginGrp->setContentsMargins(10, 10, 10, 10);
    loginGrp->layout()->setMargin(0);
    loginGrp->appendItem(m_autoLogin);
    loginGrp->appendItem(m_nopasswdLogin);
    if (!IsServerSystem) {
        layout->addSpacing(20);
    }

    if (m_isServerSystem) {
        auto pwHLayout = new QHBoxLayout;
        auto pwWidget = new SettingsItem;
        layout->addSpacing(15);
        loginGrp->appendItem(pwWidget);
        pwWidget->setLayout(pwHLayout);

        pwHLayout->addWidget(new QLabel(tr("Validity Days")), 0, Qt::AlignLeft);
        auto ageEdit = new DLineEdit();
        ageEdit->lineEdit()->setPlaceholderText(tr("Always"));
        ageEdit->setText(m_curUser->passwordAge() >= 99999 ? tr("Always") : QString::number(m_curUser->passwordAge()));
        ageEdit->setClearButtonEnabled(false);
        ageEdit->lineEdit()->setValidator(new QIntValidator(1, 99999));
        pwHLayout->addWidget(ageEdit, 0, Qt::AlignRight);

        connect(ageEdit, &DLineEdit::textChanged, this, [ageEdit]() {
            ageEdit->setAlert(false);

        });
        connect(ageEdit, &DLineEdit::editingFinished, this, [this, pwWidget, ageEdit]() {
            if (ageEdit->text().isEmpty()) {
                ageEdit->lineEdit()->setText(m_curUser->passwordAge() >= 99999 ? tr("Always") : QString::number(m_curUser->passwordAge()));
                return;
            }

            int age = ageEdit->text().toInt();

            if (age == m_curUser->passwordAge())
                return;

            if (age <= 0) {
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

    layout->addWidget(loginGrp);

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
    m_fingerWidget->setVisible(!IsServerSystem && isCurUser);
    //~ contents_path /accounts/Accounts Detail
    modifyPassword->setText(tr("Change Password"));
    //~ contents_path /accounts/Accounts Detail
    m_autoLogin->setTitle(tr("Auto Login"));
    // 平板项目，需要默认不设置自动登录
    m_autoLogin->setChecked(false);
    //~ contents_path /accounts/Accounts Detail
    m_nopasswdLogin->setTitle(DGuiApplicationHelper::isTabletEnvironment() ? tr("Unlock without Password") : tr("Login Without Password"));
    m_nopasswdLogin->setChecked(m_curUser->nopasswdLogin());

    //修改密码
    connect(modifyPassword, &QPushButton::clicked, [ = ] {
        Q_EMIT requestShowPwdSettings(m_curUser);
    });

    //自动登录，无密码登录操作
    connect(m_curUser, &User::autoLoginChanged, m_autoLogin, &SwitchWidget::setChecked);
    connect(m_curUser, &User::nopasswdLoginChanged,
            m_nopasswdLogin, &SwitchWidget::setChecked);
    connect(m_autoLogin, &SwitchWidget::checkedChanged,
            this, [ = ](const bool autoLogin) {
        if (autoLogin) {
            if (getOtherUserAutoLogin()) {
                Q_EMIT requestSetAutoLogin(m_curUser, autoLogin);
            } else {
                m_tipDialog = new DDialog(this);
                m_tipDialog->setModal(true);
                m_tipDialog->setAttribute(Qt::WA_DeleteOnClose);
                m_tipDialog->setMessage(tr("Only one account can have \"Auto Login\" enabled. If proceeding,"\
                                           " that option of other accounts will be disabled."));
                m_tipDialog->addButton(tr("Cancel"), false, DDialog::ButtonRecommend);
                m_tipDialog->addButton(tr("Enable"), true, DDialog::ButtonRecommend);
                m_tipDialog->show();
                connect(m_tipDialog, &DDialog::buttonClicked, this, [ = ](int index, const QString &text) {
                    Q_UNUSED(text);
                    if (!index) {
                        m_tipDialog->close();
                        m_autoLogin->setChecked(false);
                    } else {
                        Q_EMIT requestSetAutoLogin(m_curUser, autoLogin);
                    }
                });
                connect(m_tipDialog, &DDialog::closed, this, [ = ] {
                    m_autoLogin->setChecked(false);
                });
            }
        } else {
            Q_EMIT requestSetAutoLogin(m_curUser, autoLogin);
        }
    });
    connect(m_nopasswdLogin, &SwitchWidget::checkedChanged, this, [ = ](const bool nopasswdLogin) {
        Q_EMIT requestNopasswdLogin(m_curUser, nopasswdLogin);
    });

    //指纹界面操作
    connect(m_fingerWidget, &FingerWidget::requestAddThumbs, this, &AccountsDetailWidget::requestAddThumbs);
    connect(m_fingerWidget, &FingerWidget::requestCleanThumbs, this, &AccountsDetailWidget::requestCleanThumbs);
    connect(m_fingerWidget, &FingerWidget::requestDeleteFingerItem, this, &AccountsDetailWidget::requestDeleteFingerItem);
    connect(m_fingerWidget, &FingerWidget::requestRenameFingerItem, this, &AccountsDetailWidget::requestRenameFingerItem);
    connect(m_fingerWidget, &FingerWidget::noticeEnrollCompleted, this, &AccountsDetailWidget::noticeEnrollCompleted);
}

void AccountsDetailWidget::setAccountModel(dcc::accounts::UserModel *model)
{
    if (!model) {
        return;
    }
    m_userModel = model;
    // 平板项目，需要隐藏自动登录
    m_autoLogin->setVisible(false);
    m_nopasswdLogin->setVisible(m_userModel->isNoPassWordLoginVisable());

    // 非服务器系统，关联配置改变信号，控制自动登陆开关/无密码登陆开关显隐
    if (!IsServerSystem) {
        connect(m_userModel, &UserModel::autoLoginVisableChanged, m_autoLogin, &SwitchWidget::setVisible);
        connect(m_userModel, &UserModel::noPassWordLoginVisableChanged, m_nopasswdLogin, &SwitchWidget::setVisible);
    }

    if (!m_groupItemModel)
        return;
    m_groupItemModel->clear();
    for (QString item : m_userModel->getAllGroups()) {
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
    layout->setContentsMargins(ScrollAreaMargins);
}

bool AccountsDetailWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_inputLineEdit->lineEdit() && event->type() == QEvent::MouseButtonPress) {
        m_inputLineEdit->setAlert(false);
        m_inputLineEdit->hideAlertMessage();
        m_inputLineEdit->lineEdit()->setFocus();
    }
    return false;
}

void AccountsDetailWidget::resizeEvent(QResizeEvent *event)
{
    // 头像个数为15,头像大小为74，间距为20
    int w = event->size().width() - 20 - 94 * 15 - 1;
    if (w < 0) {
        w = (event->size().width() - 20) % 94;
    }
    m_avatarLayout->setContentsMargins(w / 2 - 1, 0, 0, 0);
}

void AccountsDetailWidget::setAllGroups(const QStringList &groups)
{
    setAccountModel(m_userModel);
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

    QStandardItem *item = m_groupItemModel->item(index.row(), index.column());
    Qt::CheckState state = item->checkState();
    state == Qt::Checked ? (void)curUserGroup.removeOne(item->text()) : curUserGroup.append(item->text());

    Q_EMIT requestSetGroups(m_curUser, curUserGroup);
}

void AccountsDetailWidget::changeUserGroup(const QStringList &groups)
{
    int row_count = m_groupItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_groupItemModel->item(i, 0);
        item->setCheckState(item && groups.contains(item->text()) ? Qt::Checked : Qt::Unchecked);
    }
    m_groupItemModel->sort(0);
}
