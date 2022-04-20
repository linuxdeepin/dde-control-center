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
#include "groupitem.h"
#include "widgets/settingsgroup.h"
#include "widgets/comboxwidget.h"
#include "src/plugin-accounts/operation/usermodel.h"
#include "src/plugin-accounts/window/removeuserdialog.h"

#include <DToolButton>
#include <DIconButton>
#include <DWarningButton>
#include <DCommandLinkButton>
#include <DFontSizeManager>
#include <DApplicationHelper>
#include <DLineEdit>
#include <DFontSizeManager>
#include <DTipLabel>
#include <DDesktopServices>
#include <DLabel>
#include <DSpinBox>
#include <DSysInfo>

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
#include <QByteArray>
#include <QPointer>

#include <grp.h>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

#define MAXVALUE 99999
#define GSETTINGS_EFFECTIVE_DAY_VISIBLE "effectiveDayVisible"

AccountSpinBox::AccountSpinBox(QWidget *parent)
    : DSpinBox(parent)
{
}

QString AccountSpinBox::textFromValue(int val) const
{
    if (val >= MAXVALUE && !lineEdit()->hasFocus()) {
        return tr("Always");
    }
    return QString::number(val);
}

void AccountSpinBox::focusInEvent(QFocusEvent *event)
{
    if (lineEdit()->text() == tr("Always")) {
        lineEdit()->setText(QString::number(MAXVALUE));
    }
    return DSpinBox::focusInEvent(event);
};

void AccountSpinBox::focusOutEvent(QFocusEvent *event)
{
    if (lineEdit()->text().isEmpty()) {
        editingFinished();
    }
    return DSpinBox::focusOutEvent(event);
}

AccountsDetailWidget::AccountsDetailWidget(User *user, UserModel *model, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_userModel(model)
    , m_groupListView(nullptr)
    , m_groupItemModel(nullptr)
    , m_autoLoginItem(new DStandardItem(tr("Auto Login")))
    , m_nopasswdLoginItem(new DStandardItem(tr("Login Without Password")))
    , m_avatarLayout(new QHBoxLayout)
    , m_deleteAccount(new DWarningButton)
    , m_modifyPassword(new QPushButton)
    , m_scrollArea(new QScrollArea)
    , m_curLoginUser(nullptr)
    , m_bindStatusLabel(new QLabel(tr("Go to Settings"), this))
    , m_view(new DListView)
    , m_viewModel(new QStandardItemModel(this))
{
    m_isServerSystem = (DSysInfo::UosServer == DSysInfo::uosType());
    //整体布局
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    setLayout(mainContentLayout);
    setFocusPolicy(Qt::FocusPolicy::ClickFocus);

    m_scrollArea->setAccessibleName("scrollArea");
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);

    QWidget *widget = new QWidget;
    widget->setAccessibleName("scrollAreaWidget");
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    widget->setContentsMargins(0, 0, 0, 0);
    widget->setMinimumWidth(330);

    mainContentLayout->addWidget(m_scrollArea);
    mainContentLayout->setContentsMargins(0, 0, 0, 0);
    auto contentLayout = new QVBoxLayout();
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(contentLayout);
    m_scrollArea->setWidget(widget);

    QScroller::grabGesture(m_scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_scrollArea->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootWhenScrollable);
    scroller->setScrollerProperties(sp);

    initUserInfo(contentLayout);
    initSetting(contentLayout);

    if (m_isServerSystem) {
        initGroups(contentLayout);
    }

    setAccountModel(model);
}

AccountsDetailWidget::~AccountsDetailWidget()
{
    delete m_autoLoginItem;
    m_autoLoginItem = nullptr;

    delete m_nopasswdLoginItem;
    m_nopasswdLoginItem = nullptr;
}

QString AccountsDetailWidget::getOtherUserAutoLogin()
{
    for (auto user : m_userModel->userList()) {
        if (user->name() != m_curUser->name() && user->autoLogin()) {
            return user->name();
        }
    }
    return "";
}

//删除账户
void AccountsDetailWidget::deleteUserClicked()
{
    RemoveUserDialog d(m_curUser, this);
    int ret = d.exec();

    if (ret == 1) {
        Q_EMIT requestDeleteAccount(m_curUser, d.deleteHome());
    } else {
        m_deleteAccount->setEnabled(true);
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

    m_fullName = new DLabel;
    m_fullName->setContentsMargins(0, 6, 0, 6);

    auto fullname = m_curUser->fullname();
    m_fullName->setEnabled(true);
    if (fullname.simplified().isEmpty()) {
        fullname = tr("Full Name");
        m_fullName->setEnabled(false);
    } else if (fullname.toLocal8Bit().size() > 32) {
        for (auto i = 1; i <= fullname.size(); ++i) {
            if (fullname.left(i).toLocal8Bit().size() > 29) {
                fullname = fullname.left(i - 1) + QString("...");
                break;
            }
        }
    }
    m_fullName->setText(fullname.toHtmlEscaped());
    m_fullName->setElideMode(Qt::ElideRight);

    m_fullNameBtn = new DToolButton(this);
    m_fullNameBtn->setAccessibleName("fullName_btn");
    m_fullNameBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    m_fullNameBtn->setIconSize(QSize(12, 12));

    m_inputLineEdit = new DLineEdit();
    m_inputLineEdit->setAccessibleName("fullName_edit");
    m_inputLineEdit->setMinimumWidth(220);
    m_inputLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_inputLineEdit->setVisible(false);
    m_inputLineEdit->lineEdit()->setFrame(false);
    m_inputLineEdit->lineEdit()->setAlignment(Qt::AlignCenter);
    m_inputLineEdit->lineEdit()->installEventFilter(this);

    DFontSizeManager::instance()->bind(m_fullName, DFontSizeManager::T5);
    DFontSizeManager::instance()->bind(m_inputLineEdit, DFontSizeManager::T5);

    QHBoxLayout *fullnameLayout = new QHBoxLayout;
    fullnameLayout->setSpacing(5);
    fullnameLayout->setAlignment(Qt::AlignHCenter);
    fullnameLayout->addWidget(m_fullName);
    fullnameLayout->addWidget(m_fullNameBtn);
    fullnameLayout->addWidget(m_inputLineEdit);
    fullnameLayout->setContentsMargins(10, 0, 10, 0);
    layout->addLayout(fullnameLayout);

    m_avatarListWidget = new AvatarListWidget(m_curUser, this);
    m_avatarListWidget->setAccessibleName("List_useravatarlist");
    m_avatarListWidget->setVisible(false);
    m_avatarListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_avatarListWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_avatarLayout->addWidget(m_avatarListWidget);
    layout->addLayout(m_avatarLayout);

    connect(m_inputLineEdit, &DLineEdit::textEdited, this, [ = ](const QString &userFullName) {
        /* 90401:在键盘输入下禁止冒号的输入，粘贴情况下自动识别冒号自动删除 */
        QString fullName = userFullName;
        fullName.remove(":");
        if (fullName != userFullName) {
            m_inputLineEdit->setText(fullName);
        }
        if (fullName.size() > 32) {
            m_inputLineEdit->lineEdit()->backspace();
            m_inputLineEdit->setAlert(true);
            m_inputLineEdit->showAlertMessage(tr("The full name is too long"), this);
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (m_inputLineEdit->isAlert()) {
            m_inputLineEdit->setAlert(false);
            m_inputLineEdit->hideAlertMessage();
        }
    });

    connect(m_inputLineEdit, &DLineEdit::editingFinished, this, [ = ] {
        QString userFullName = m_inputLineEdit->lineEdit()->text();
        Q_EMIT editingFinished(userFullName);
    });

    //点击用户图像
    connect(avatar, &AvatarWidget::clicked, this, [ = ](const QString & iconPath) {
        Q_UNUSED(iconPath)
        avatar->setArrowed(!avatar->arrowed());
        m_avatarListWidget->setVisible(avatar->arrowed());
    });

    connect(m_avatarListWidget, &AvatarListWidget::requesRetract, this, [ = ] {
        if (avatar->arrowed())
        {
            avatar->setArrowed(!avatar->arrowed());
            m_scrollArea->setVisible(false);
            m_avatarListWidget->setVisible(avatar->arrowed());
            m_scrollArea->setVisible(true);
        }
    });

    connect(m_curUser, &User::currentAvatarChanged, m_avatarListWidget, &AvatarListWidget::setCurrentAvatarChecked);
    connect(m_curUser, &User::currentAvatarChanged, avatar, &AvatarWidget::setAvatarPath);
    //用户名发生变化
    connect(m_curUser, &User::nameChanged, shortName, &QLabel::setText);
    connect(m_curUser, &User::fullnameChanged, this, [ = ](const QString & fullname) {
        auto tstr = fullname;
        m_fullName->setEnabled(true);
        if (fullname.simplified().isEmpty()) {
            tstr = tr("Full Name");
            m_fullName->setEnabled(false);
        } else if (fullname.toLocal8Bit().size() > 32) {
            for (auto i = 1; i <= fullname.size(); ++i) {
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
        m_fullName->setVisible(false);
        m_fullNameBtn->setVisible(false);
        m_inputLineEdit->setVisible(true);
        m_inputLineEdit->setAlert(false);
        m_inputLineEdit->setText(m_curUser->fullname());
        m_inputLineEdit->hideAlertMessage();
        m_inputLineEdit->lineEdit()->setFocus();
    });

    for (auto user : m_userModel->userList()) {
        if (user->name() == m_userModel->getCurrentUserName()) {
            m_curLoginUser = user;
            // 结束后续没必要的循环
            break;
        }
    }

    if (m_userModel->getIsSecurityHighLever() && m_curLoginUser->securityLever() != SecurityLever::Sysadm && !m_curUser->isCurrentUser()) {
        avatar->setEnabled(false) ;
    }

}

void AccountsDetailWidget::initSetting(QVBoxLayout *layout)
{
    QHBoxLayout *modifydelLayout = new QHBoxLayout;
    modifydelLayout->setContentsMargins(10, 0, 10, 0);
    modifydelLayout->addWidget(m_modifyPassword);
    modifydelLayout->addSpacing(10);
    modifydelLayout->addWidget(m_deleteAccount);
    layout->addSpacing(20);
    layout->addLayout(modifydelLayout);

    m_view->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_view->setSelectionMode(QListView::SelectionMode::NoSelection);
    m_view->setEditTriggers(DListView::NoEditTriggers);
    m_view->setFrameShape(DListView::NoFrame);
    m_view->setViewportMargins(10, 10, 10, 10);
    m_view->setItemSpacing(1);
    QMargins itemMargins(m_view->itemMargins());
    itemMargins.setLeft(1);
    m_view->setItemMargins(itemMargins);
    m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_view->setModel(m_viewModel);

    auto checkstatus = m_curUser->autoLogin() ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked;
    auto checkIcon = qobject_cast<DStyle *>(style())->standardIcon(checkstatus);
    DViewItemAction *autoLoginAction = new DViewItemAction(Qt::AlignVCenter, QSize(16, 16), QSize(16, 16), true);
    autoLoginAction->setIcon(checkIcon);
    autoLoginAction->setEnabled(m_curUser->isCurrentUser());
    m_autoLoginItem->setActionList(Qt::Edge::RightEdge, {autoLoginAction});
    m_viewModel->appendRow(m_autoLoginItem);

    DViewItemAction *nopasswdLoginAction = new DViewItemAction(Qt::AlignVCenter, QSize(16, 16), QSize(16, 16), true);
    checkstatus = m_curUser->nopasswdLogin() ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked;
    checkIcon = qobject_cast<DStyle *>(style())->standardIcon(checkstatus);
    nopasswdLoginAction->setIcon(checkIcon);
    nopasswdLoginAction->setEnabled(m_curUser->isCurrentUser());
    m_nopasswdLoginItem->setActionList(Qt::Edge::RightEdge, {nopasswdLoginAction});
    m_viewModel->appendRow(m_nopasswdLoginItem);

    SettingsGroup *loginGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);

    loginGrp->getLayout()->setContentsMargins(0, 0, 0, 0);
    loginGrp->setContentsMargins(10, 10, 10, 10);
    loginGrp->layout()->setMargin(0);
    loginGrp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    if (!m_isServerSystem) {
        layout->addSpacing(20);
    }
    // 设置密码有效期
    auto pwHLayout = new QHBoxLayout;
    auto pwWidget = new SettingsItem;
    loginGrp->appendItem(pwWidget);
    pwWidget->setLayout(pwHLayout);

    QLabel *vlidityLabel = new QLabel(tr("Validity Days"));
    pwHLayout->addWidget(vlidityLabel, 0, Qt::AlignLeft);
    auto validityDaysBox = new AccountSpinBox();
    validityDaysBox->lineEdit()->setValidator(new QRegularExpressionValidator(QRegularExpression("[1-9]\\d{0,4}/^[1-9]\\d*$/"), validityDaysBox->lineEdit()));
    validityDaysBox->lineEdit()->setPlaceholderText("99999");
    validityDaysBox->setRange(1, 99999);
    pwHLayout->addWidget(validityDaysBox, 0, Qt::AlignRight);

    // 设置安全问题
    auto sqHLayout = new QHBoxLayout;
    sqHLayout->setContentsMargins(10, 0, 10, 0);
    auto securityQuestionsWidget = new SettingsItem;
    loginGrp->appendItem(securityQuestionsWidget);
    securityQuestionsWidget->setLayout(sqHLayout);

    QLabel *securityQuestionsLabel= new QLabel(tr("Security Questions"));
    sqHLayout->addWidget(securityQuestionsLabel, 0, Qt::AlignLeft);
    sqHLayout->addSpacing(108);
    sqHLayout->addWidget(m_bindStatusLabel);
    auto securityQuestionsButton = new DToolButton(securityQuestionsWidget);
    QStyleOption opt;
    securityQuestionsButton->setIcon(DStyleHelper(this->style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
    sqHLayout->addWidget(securityQuestionsButton, 0, Qt::AlignRight);
    securityQuestionsWidget->setVisible(false);

    connect(securityQuestionsButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestShowSecurityQuestionsSettings(m_curUser);
    });

    connect(validityDaysBox, qOverload<int>(&DSpinBox::valueChanged), this, [=](const int value) {
        validityDaysBox->setValue(value);
        validityDaysBox->setAlert(false);
    });
    connect(validityDaysBox, &QSpinBox::editingFinished, this, [this, validityDaysBox]() {
        if (validityDaysBox->lineEdit()->text().isEmpty()) {
            validityDaysBox->setValue(m_curUser->passwordAge());
            return;
        }
        int age = validityDaysBox->value();
        if (age == m_curUser->passwordAge())
            return;

        Q_EMIT requsetSetPassWordAge(m_curUser, validityDaysBox->value());
    });
    connect(m_curUser, &User::passwordAgeChanged, validityDaysBox, &AccountSpinBox::setValue);

    validityDaysBox->setValue(m_curUser->passwordAge());
    validityDaysBox->valueChanged(m_curUser->passwordAge());

    pwWidget->setVisible(true);
    layout->addWidget(loginGrp);

    //账户设置
    SettingsGroup *accountSettingsGrp = new SettingsGroup(nullptr, SettingsGroup::GroupBackground);
    m_asAdministrator = new ComboxWidget;
    m_asAdministrator->setTitle(tr("Account Type"));
    m_asAdministrator->addBackground();
    QStringList options;
    options << tr("Standard User") << tr("Administrator");
    m_asAdministrator->setComboxOption(options);
    accountSettingsGrp->getLayout()->setContentsMargins(0, 0, 0, 0);
    accountSettingsGrp->setContentsMargins(10, 0, 10, 0);
    accountSettingsGrp->layout()->setMargin(0);
    accountSettingsGrp->appendItem(m_asAdministrator);
    layout->addWidget(accountSettingsGrp);
    layout->addWidget(m_view);

    //非当前用户不显示修改密码，自动登录，无密码登录,指纹页面
    bool isCurUser = m_curUser->isCurrentUser();
    m_autoLoginItem->setEnabled(isCurUser);
    m_nopasswdLoginItem->setEnabled(isCurUser);
    m_modifyPassword->setText(m_curUser->isCurrentUser() ? tr("Change Password") : tr("Reset Password"));
    m_deleteAccount->setText(tr("Delete Account"));
    m_asAdministrator->setCurrentIndex(isSystemAdmin(m_curUser)? User::UserType::Administrator: User::UserType::StandardUser);
    //开启等保3级后如果当前登陆的账户不是sysadm_u，则用户只能操作当前自己登陆用户的界面
    if (m_userModel->getIsSecurityHighLever() && m_curLoginUser->securityLever() != SecurityLever::Sysadm && !isCurUser) {
        validityDaysBox->setEnabled(false);
        m_fullNameBtn->setEnabled(false);
        m_deleteAccount->setEnabled(false);
        m_modifyPassword->setEnabled(false);
        m_autoLoginItem->setEnabled(false);
        m_nopasswdLoginItem->setEnabled(false);
        m_asAdministrator->setEnabled(false);
    }
    //修改密码状态判断
    connect(m_curUser, &User::onlineChanged, this, [ = ] {
        setModifyPwdBtnStatus("accountUserModifypasswd");
    });
    setModifyPwdBtnStatus("accountUserModifypasswd");

    //修改密码
    connect(m_modifyPassword, &QPushButton::clicked, this, [ = ] {
        Q_EMIT requestShowPwdSettings(m_curUser);
    });

    //删除用户
    connect(m_deleteAccount, &DWarningButton::clicked, this, [ = ] {
        m_deleteAccount->setEnabled(false);
        deleteUserClicked();
    });

    //自动登录，无密码登录操作
    connect(m_curUser, &User::autoLoginChanged, this, [=](const bool checked) {
        updateItemCheckStatus(m_autoLoginItem, checked);
    });
    connect(m_curUser, &User::nopasswdLoginChanged, this, [=](const bool checked) {
        updateItemCheckStatus(m_nopasswdLoginItem, checked);
    });
    connect(m_autoLoginItem->actionList(Qt::Edge::RightEdge).first(), &DViewItemAction::triggered,
    this, [ = ] {
        bool autoLogin = !m_curUser->autoLogin();
        updateItemCheckStatus(m_autoLoginItem, autoLogin);
        if (autoLogin) {
            const QString &existedAutoLoginUserName = getOtherUserAutoLogin();
            if (existedAutoLoginUserName.isEmpty()) {
                Q_EMIT requestSetAutoLogin(m_curUser, autoLogin);
            } else {
                DDialog *tipDialog = new DDialog(this);
                tipDialog->setIcon(QIcon::fromTheme("dialog-warning"));
                tipDialog->setModal(true);
                tipDialog->setAttribute(Qt::WA_DeleteOnClose);
                tipDialog->addButton(tr("OK"));
                tipDialog->setMessage(tr("\"Auto Login\" can be enabled for only one account, please disable it for the account \"%1\" first").arg(existedAutoLoginUserName));
                tipDialog->setFixedWidth(422);
                tipDialog->show();
                updateItemCheckStatus(m_autoLoginItem, false);
            }
        } else {
            Q_EMIT requestSetAutoLogin(m_curUser, autoLogin);
        }
    });
    connect(m_nopasswdLoginItem->actionList(Qt::Edge::RightEdge).first(), &DViewItemAction::triggered, this, [ = ] {
        updateItemCheckStatus(m_nopasswdLoginItem, !m_curUser->nopasswdLogin());
        Q_EMIT requestNopasswdLogin(m_curUser, !m_curUser->nopasswdLogin());
    });

    //图像列表操作
    connect(m_avatarListWidget, &AvatarListWidget::requestSetAvatar, this, [ = ](const QString & avatarPath) {
        Q_EMIT requestSetAvatar(m_curUser, avatarPath);
    });

    //切换账户类型
    connect(m_curUser, &User::userTypeChanged, this, [ = ](const int userType) {
        m_asAdministrator->setCurrentIndex(userType);
    });
    connect(m_asAdministrator, &ComboxWidget::onIndexChanged, this, [ = ](const int userType) {
        Q_EMIT requestSetAdministrator(m_curUser, User::UserType::Administrator == userType);
    });
    connect(m_curUser, &User::startSecurityQuestionsCheckReplied, this, &AccountsDetailWidget::onSecurityQuestionsCheckReplied);


}

void AccountsDetailWidget::updateItemCheckStatus(DStandardItem *item, const bool checked)
{
    if (item->actionList(Qt::Edge::RightEdge).size() > 0) {
        auto checkstatus = checked ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked;
        auto icon = qobject_cast<DStyle *>(style())->standardIcon(checkstatus);
        item->actionList(Qt::Edge::RightEdge).first()->setIcon(icon);
        m_view->update(item->index());
    }
}

void AccountsDetailWidget::setAccountModel(UserModel *model)
{
    if (!model) {
        return;
    }

    m_view->setRowHidden(m_viewModel->indexFromItem(m_autoLoginItem).row(), !m_userModel->isAutoLoginVisable());
    m_view->setRowHidden(m_viewModel->indexFromItem(m_nopasswdLoginItem).row(), !m_userModel->isNoPassWordLoginVisable());

    // 非服务器系统，关联配置改变信号，控制自动登陆开关/无密码登陆开关显隐
    if (!m_isServerSystem) {
        connect(m_userModel, &UserModel::autoLoginVisableChanged, this, [&](const bool checked) {
            m_view->setRowHidden(m_viewModel->indexFromItem(m_autoLoginItem).row(), !checked);
        });
        connect(m_userModel, &UserModel::noPassWordLoginVisableChanged, this, [&](const bool checked) {
            m_view->setRowHidden(m_viewModel->indexFromItem(m_nopasswdLoginItem).row(), !checked);
        });
    }

    auto adminCnt = [ = ]() {
        int cnt = 0;
        if (m_userModel->getIsSecurityHighLever()) {
            return 1;
        }
        for (auto user : m_userModel->userList()) {
            if (user->userType() == User::UserType::Administrator)
                cnt++;
        }
        return cnt;
    };

    auto isOnlyAdmin = [ = ] {                                // 是最后一个管理员
        return isSystemAdmin(m_curUser)                         // 是管理员
               && adminCnt() == 1;                              // 管理员只有一个
    };

    auto deleteUserBtnEnable = [ = ] {    // 可以删除用户
        if (m_userModel->getIsSecurityHighLever()){
            return m_curLoginUser->securityLever() == SecurityLever::Sysadm && !m_curUser->isCurrentUser();
        }
        return !m_curUser->isCurrentUser()  // 不是当前用户
               && !m_curUser->online()     // 未登录
               && !isOnlyAdmin();          // 不是最后一个管理员
    };

    auto asAdministratorEnable = [ = ] {  // 可以切换账户类型
        if (m_userModel->getIsSecurityHighLever()){
            return m_curLoginUser->securityLever() == SecurityLever::Sysadm && !m_curUser->isCurrentUser();
        }
        return !m_curUser->isCurrentUser()  // 不是当前用户
               && !m_curUser->online()      // 未登录
               && !isOnlyAdmin();           // 不是最后一个管理员
    };

    setDeleteBtnStatus("accountUserDeleteaccount", deleteUserBtnEnable());

    connect(m_curUser, &User::onlineChanged, m_deleteAccount, [ = ] {
        m_deleteAccount->setEnabled(deleteUserBtnEnable());
    });
    connect(m_userModel, &UserModel::adminCntChange, m_deleteAccount, [ = ] {
        m_deleteAccount->setEnabled(deleteUserBtnEnable());
    });

    m_asAdministrator->setEnabled(asAdministratorEnable());
    connect(m_curUser, &User::onlineChanged, m_asAdministrator, [ = ] {
        m_asAdministrator->setEnabled(asAdministratorEnable());
    });
    connect(m_userModel, &UserModel::adminCntChange, m_asAdministrator, [ = ] {
        m_asAdministrator->setEnabled(asAdministratorEnable());
    });

    if (!m_groupItemModel)
        return;
    m_groupItemModel->clear();
    for (QString item : m_userModel->getAllGroups()) {
        GroupItem *it = new GroupItem(item);
        it->setCheckable(false);
        m_groupItemModel->appendRow(it);
    }

    connect(m_userModel, &UserModel::allGroupsChange, this, &AccountsDetailWidget::setGroupInfo);
    changeUserGroup(m_curUser->groups());
    onGidChanged(m_curUser->gid());
    connect(m_curUser, &User::groupsChanged, this, &AccountsDetailWidget::changeUserGroup);
    connect(m_curUser, &User::gidChanged, this, &AccountsDetailWidget::onGidChanged);
    connect(m_curLoginUser, &User::userTypeChanged, this, [ = ] {
        setModifyPwdBtnStatus("accountUserModifypasswd");
    });
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
    layout->addSpacing(10);
    layout->addWidget(m_groupListView);
    layout->setContentsMargins(10, 10, 10, 10);
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

void AccountsDetailWidget::resetDelButtonState()
{
    m_deleteAccount->setEnabled(true);
}

void AccountsDetailWidget::onEditingFinished(bool isValid, const QString &userFullName)
{
    if (userFullName == m_curUser->fullname() || (!userFullName.isEmpty() && userFullName.simplified().isEmpty())) {
        m_inputLineEdit->lineEdit()->clearFocus();
        m_inputLineEdit->setVisible(false);
        m_fullName->setVisible(true);
        m_fullNameBtn->setVisible(true);
        if (m_inputLineEdit->isAlert()) {
            m_inputLineEdit->setAlert(false);
            m_inputLineEdit->hideAlertMessage();
        }
        return;
    }
    if (!userFullName.isEmpty()) {
        if (isValid) {
            m_inputLineEdit->setAlert(true);
            m_inputLineEdit->showAlertMessage(tr("The full name has been used by other user accounts"), m_inputLineEdit, 2000);
            m_inputLineEdit->lineEdit()->selectAll();
            return;
        }
        QList<User *> userList = m_userModel->userList();
        for (User *user : userList) {
            if (userFullName == user->fullname() || userFullName == user->name()) {
                m_inputLineEdit->setAlert(true);
                m_inputLineEdit->showAlertMessage(tr("The full name has been used by other user accounts"), m_inputLineEdit, 2000);
                m_inputLineEdit->lineEdit()->selectAll();
                return;
            }
        }
        QList<QString> groupList = m_userModel->getAllGroups();
        for (QString &group : groupList) {
            if (userFullName == group && userFullName != m_curUser->name()) {
                m_inputLineEdit->setAlert(true);
                m_inputLineEdit->showAlertMessage(tr("The full name has been used by other user accounts"), m_inputLineEdit, 2000);
                m_inputLineEdit->lineEdit()->selectAll();
                return;
            }
        }
    }
    m_inputLineEdit->lineEdit()->clearFocus();
    m_inputLineEdit->setVisible(false);
    m_fullName->setVisible(true);
    m_fullNameBtn->setVisible(true);
    if (m_inputLineEdit->isAlert()) {
        m_inputLineEdit->setAlert(false);
        m_inputLineEdit->hideAlertMessage();
    }

    Q_EMIT requestSetFullname(m_curUser, m_inputLineEdit->text());
}

void AccountsDetailWidget::onSecurityQuestionsCheckReplied(const QList<int> &questions)
{
    m_bindStatusLabel->setVisible(questions.isEmpty());
}

void AccountsDetailWidget::setDeleteBtnStatus(const QString &key, const bool &status)
{
    if ("accountUserDeleteaccount" != key)
        return;

    m_deleteAccount->setEnabled(status);
    m_deleteAccount->setVisible(true);
}

void AccountsDetailWidget::setModifyPwdBtnStatus(const QString &key)
{
    if ("accountUserModifypasswd" != key)
        return;

    // 若选择当前登录的账户，则允许修改，选择其他账户，当前登录账户必需是管理员且其他账户未登录时才允许修改密码
    m_modifyPassword->setEnabled(((!m_curUser->online() && isSystemAdmin(m_curLoginUser)) || m_curUser->isCurrentUser()));
    m_modifyPassword->setVisible(true);
}

bool AccountsDetailWidget::isSystemAdmin(User *user)
{
    // 本地管理员账户不一定是等保三级的管理员账户，要区分判断
    if (m_userModel->getIsSecurityHighLever())
        return user->securityLever() == SecurityLever::Sysadm;

    return user->userType() == User::UserType::Administrator;
}

void AccountsDetailWidget::userGroupClicked(const QModelIndex &index)
{
    QStandardItem *item = m_groupItemModel->item(index.row(), index.column());
    //不可移除主组
    if (!item || item->text() == m_groupName)
        return;

    QStringList curUserGroup;
    int row_count = m_groupItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *itemGroup = m_groupItemModel->item(i, 0);
        if (itemGroup && itemGroup->checkState()) {
            curUserGroup << itemGroup->text();
        }
    }

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
        item->setEnabled(item->text() != m_groupName);
    }
    m_groupItemModel->sort(0);
}

void AccountsDetailWidget::setGroupInfo(const QStringList &group)
{
    for (const QString &item : group) {
        GroupItem *it = new GroupItem(item);
        it->setCheckable(false);
        m_groupItemModel->appendRow(it);
    }
    changeUserGroup(m_curUser->groups());
}

void AccountsDetailWidget::onGidChanged(const QString &gid)
{
    bool ok;
    int iGid = gid.toInt(&ok, 10);
    if (!ok)
        return;

    const group *group = getgrgid(static_cast<__gid_t>(iGid));
    if (nullptr == group || nullptr == group->gr_name)
        return;

    m_groupName = QString(group->gr_name);
    for (int i = 0; i < m_groupItemModel->rowCount(); ++i) {
        QStandardItem *item = m_groupItemModel->item(i, 0);
        if (nullptr == item)
            continue;
        item->setEnabled(item->text() != m_groupName);
    }
}
