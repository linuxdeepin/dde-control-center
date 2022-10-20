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

#include "accountslistview.h"
#include "accountsmodule.h"
#include "avatarwidget.h"
#include "createaccountpage.h"
#include "groupitem.h"
#include "modifypasswdpage.h"
#include "removeuserdialog.h"

#include "operation/accountsworker.h"
#include "operation/user.h"
#include "operation/usermodel.h"
#include "securityquestionspage.h"

#include "window/accountsmodel.h"

#include "widgets/widgetmodule.h"
#include "widgets/listviewmodule.h"
#include "widgets/moduleobjectitem.h"

#include <DDialog>

#include <QStringList>
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include <QListView>
#include <QListWidget>
#include <QScroller>
#include <QStackedWidget>
#include <QResizeEvent>
#include <DLabel>
#include <DToolButton>
#include <DWarningButton>
#include <DSysInfo>
#include <DDesktopServices>
#include <DFloatingButton>

#include <widgets/comboxwidget.h>
#include <widgets/dcclistview.h>
#include <widgets/settingsgroup.h>
#include <widgets/settingsitem.h>

#include <polkit-qt5-1/PolkitQt1/Authority>

#include <DDBusSender>
#include <grp.h>

using namespace DCC_NAMESPACE;
DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

QString AccountsPlugin::name() const
{
    return QStringLiteral("accounts");
}

ModuleObject *AccountsPlugin::module()
{
    return new AccountsModule();
}

QString AccountsPlugin::location() const
{
    return "1";
}
///////////////////////////////////////
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
///////////////////////////////////////
AccountsModule::AccountsModule(QObject *parent)
    : PageModule("accounts", tr("Accounts"), tr("Accounts"), QIcon::fromTheme("dcc_nav_accounts"), parent)
    , m_model(nullptr)
    , m_worker(nullptr)
    , m_curLoginUser(nullptr)
    , m_curUser(nullptr)
    , m_accountsmodel(nullptr)
    , m_groupItemModel(new QStandardItemModel(this))
    , m_checkAuthorizationing(false)
{
    m_model = new UserModel(this);
    m_worker = new AccountsWorker(m_model, this);

    setGroupInfo(m_model->getAllGroups());
    connect(m_model, &UserModel::allGroupsChange, this, &AccountsModule::setGroupInfo);
    connect(m_worker, &AccountsWorker::showSafeyPage, this, &AccountsModule::onShowSafetyPage);

    appendChild(new WidgetModule<QWidget>("accountsList", "", this, &AccountsModule::initAccountsList));
    appendChild(new WidgetModule<QWidget>("avatar", "", this, &AccountsModule::initAvatar));
    appendChild(new WidgetModule<QWidget>("fullName", tr("Full Name"), this, &AccountsModule::initFullName));
    appendChild(new WidgetModule<QWidget>("name", tr("Username"), this, &AccountsModule::initName));
    appendChild(new WidgetModule<QWidget>("deleteAccount", tr("Delete Account"), this, &AccountsModule::initModifyButton));
    //
    appendChild(new WidgetModule<SettingsGroup>("accountType", tr("Account Type"), this, &AccountsModule::initAccountType));

    ListViewModule *listViewModule = new ListViewModule("autoLogin", tr("Auto Login"));
    connect(listViewModule, &ListViewModule::clicked, this, &AccountsModule::onLoginModule);
    appendChild(listViewModule);
    m_autoLoginModule = new ModuleObjectItem("autoLogin", tr("Auto Login"));
    listViewModule->appendChild(m_autoLoginModule);
    m_loginWithoutPasswordModule = new ModuleObjectItem("loginWithoutPassword", tr("Login Without Password"));
    listViewModule->appendChild(m_loginWithoutPasswordModule);

    appendChild(new WidgetModule<SettingsGroup>("validityDays", tr("Validity Days"), this, &AccountsModule::initValidityDays));

    if (DSysInfo::UosServer == DSysInfo::uosType()) {
        appendChild(new WidgetModule<QLabel>("group", tr("Group"), [](QLabel *groupTip) {
            groupTip->setText(tr("Group"));
        }));
        appendChild(new WidgetModule<DCCListView>("groupListView", tr("Group"), [this](DCCListView *m_groupListView) {
            m_groupListView->setModel(m_groupItemModel);
            m_groupListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
            m_groupListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
            m_groupListView->setSelectionMode(QAbstractItemView::NoSelection);
            m_groupListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_groupListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_groupListView->setSpacing(1);
            connect(m_groupListView, &DListView::clicked, this, &AccountsModule::userGroupClicked);
        }));
    }
}

AccountsModule::~AccountsModule()
{
    m_model->deleteLater();
    m_worker->deleteLater();
}

void AccountsModule::active()
{
    m_worker->active();
    for (auto user : m_model->userList()) {
        if (user->name() == m_model->getCurrentUserName()) {
            m_curLoginUser = user;
            // 结束后续没必要的循环
            break;
        }
    }
    m_checkAuthorizationing = false;
    if (!m_accountsmodel) {
        m_accountsmodel = new AccountsModel(this);
        m_accountsmodel->setUserModel(m_model);
    }
    setCurrentUser(m_accountsmodel->getUser(m_accountsmodel->index(0, 0)));
}

bool AccountsModule::isSystemAdmin(User *user)
{
    // 本地管理员账户不一定是等保三级的管理员账户，要区分判断
    if (m_model->getIsSecurityHighLever())
        return user->securityLever() == SecurityLever::Sysadm;

    return user->userType() == User::UserType::Administrator;
}

void AccountsModule::initAccountsList(QWidget *w)
{
    AccountsListView *userlistView = new AccountsListView(w);
    userlistView->setMaximumHeight(90);
    userlistView->setFrameShape(QFrame::NoFrame);
    QPalette pa = userlistView->palette();
    pa.setColor(QPalette::Base, pa.color(QPalette::Window));
    userlistView->setAutoFillBackground(true);
    userlistView->setPalette(pa);

    userlistView->setIconSize(QSize(40, 40));
    userlistView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    userlistView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QScroller::grabGesture(userlistView, QScroller::LeftMouseButtonGesture);
    UserDelegate *delegate = new UserDelegate(userlistView);
    userlistView->setItemDelegate(delegate);
    userlistView->setModel(m_accountsmodel);

    QScroller::grabGesture(userlistView->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(userlistView);
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);
    //自动刷新当前链接状态
    connect(userlistView->selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex &current, const QModelIndex &previous) {
        setCurrentUser(m_accountsmodel->getUser(current));
    });
    connect(this, &AccountsModule::currentUserChanged, userlistView, [this, userlistView](User *user, User *oldUser) {
        QModelIndex i = m_accountsmodel->index(user);
        if (userlistView->selectionModel()->currentIndex() != i) {
            userlistView->selectionModel()->setCurrentIndex(i, QItemSelectionModel::ClearAndSelect);
        }
    });

    DFloatingButton *createBtn = new DFloatingButton(nullptr);
    createBtn->setIcon(DStyle::SP_IncreaseElement);
    createBtn->setFixedSize(50, 50);
    createBtn->setToolTip(tr("Create Account"));
    createBtn->setAccessibleName(tr("Create Account"));
    createBtn->setVisible(true);
    connect(createBtn, &QPushButton::clicked, this, &AccountsModule::onCreateAccount);
    userlistView->selectionModel()->select(m_accountsmodel->index(0, 0), QItemSelectionModel::SelectCurrent);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(userlistView);
    layout->addWidget(createBtn, 0, Qt::AlignRight);
    w->setLayout(layout);
}

void AccountsModule::initAvatar(QWidget *w)
{
    QHBoxLayout *layout = new QHBoxLayout;
    AvatarWidget *avatar = new AvatarWidget();
    layout->addWidget(avatar, 0, Qt::AlignCenter);
    w->setLayout(layout);
    avatar->setFixedSize(120, 120);
    avatar->setArrowed(false);
    auto updateUser = [avatar](User *user, User *oldUser) {
        if (oldUser)
            disconnect(oldUser, 0, avatar, 0);

        avatar->setAvatarPath(user->currentAvatar());
        connect(user, &User::currentAvatarChanged, avatar, &AvatarWidget::setAvatarPath);
    };
    updateUser(m_curUser, nullptr);
    connect(this, &AccountsModule::currentUserChanged, avatar, updateUser);
    connect(avatar, &AvatarWidget::clicked, this, &AccountsModule::onModifyIcon);
}

void AccountsModule::initFullName(QWidget *w)
{
    DLabel *fullName = new DLabel();
    fullName->setContentsMargins(0, 6, 0, 6);
    fullName->setElideMode(Qt::ElideRight);
    DFontSizeManager::instance()->bind(fullName, DFontSizeManager::T5);

    DLineEdit *inputLineEdit = new DLineEdit();
    inputLineEdit->setAccessibleName("fullName_edit");
    inputLineEdit->setMinimumWidth(220);
    inputLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    inputLineEdit->setVisible(false);
    inputLineEdit->lineEdit()->setFrame(false);
    inputLineEdit->lineEdit()->setAlignment(Qt::AlignCenter);
    inputLineEdit->lineEdit()->installEventFilter(this);
    DFontSizeManager::instance()->bind(inputLineEdit, DFontSizeManager::T5);

    connect(inputLineEdit, &DLineEdit::textEdited, inputLineEdit, [inputLineEdit](const QString &userFullName) {
        /* 90401:在键盘输入下禁止冒号的输入，粘贴情况下自动识别冒号自动删除 */
        QString fullName = userFullName;
        fullName.remove(":");
        if (fullName != userFullName) {
            inputLineEdit->setText(fullName);
        }
        if (fullName.size() > 32) {
            inputLineEdit->lineEdit()->backspace();
            inputLineEdit->setAlert(true);
            inputLineEdit->showAlertMessage(tr("The full name is too long"), inputLineEdit);
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (inputLineEdit->isAlert()) {
            inputLineEdit->setAlert(false);
            inputLineEdit->hideAlertMessage();
        }
    });

    DToolButton *fullNameBtn = new DToolButton();
    fullNameBtn->setAccessibleName("fullName_btn");
    fullNameBtn->setIcon(QIcon::fromTheme("dcc_edit"));
    fullNameBtn->setIconSize(QSize(12, 12));
    //点击用户全名编辑按钮
    connect(fullNameBtn, &DIconButton::clicked, this, [fullName, fullNameBtn, inputLineEdit, this]() {
        fullName->setVisible(false);
        fullNameBtn->setVisible(false);
        inputLineEdit->setVisible(true);
        inputLineEdit->setAlert(false);
        inputLineEdit->setText(m_curUser->fullname());
        inputLineEdit->hideAlertMessage();
        inputLineEdit->lineEdit()->setFocus();
    });
    auto updateFullname = [fullName, fullNameBtn, inputLineEdit, this](User *user, User *oldUser) {
        if (oldUser)
            disconnect(oldUser, 0, fullName, 0);
        fullName->setVisible(true);
        fullNameBtn->setVisible(true);
        inputLineEdit->setVisible(false);
        if (m_model->getIsSecurityHighLever() && m_curLoginUser->securityLever() != SecurityLever::Sysadm && !m_curUser->isCurrentUser()) {
            fullNameBtn->setEnabled(false);
        }
        setFullname(user->fullname(), fullName);
        connect(user, &User::fullnameChanged, fullName, [this, fullName](const QString &name) {
            setFullname(name, fullName);
        });
    };
    updateFullname(m_curUser, nullptr);
    connect(this, &AccountsModule::currentUserChanged, fullName, updateFullname);

    connect(inputLineEdit, &DLineEdit::editingFinished, inputLineEdit, [fullName, fullNameBtn, inputLineEdit, this] {
        QString userFullName = inputLineEdit->lineEdit()->text();
        QDBusPendingReply<bool, QString, int> reply = m_worker->isUsernameValid(userFullName);
        //欧拉版会自己创建shutdown等root组账户且不会添加到userList中，导致无法重复性算法无效，先通过isUsernameValid校验这些账户再通过重复性算法校验
        // vaild == false && code ==6 是用户名已存在
        if (!reply.argumentAt(0).toBool() && PassErrorCode::ErrCodeSystemUsed == reply.argumentAt(2).toInt()) {
            bool ok = onEditingFinished(true, inputLineEdit);
            fullName->setVisible(ok);
            fullNameBtn->setVisible(ok);
        } else {
            bool ok = onEditingFinished(false, inputLineEdit);
            fullName->setVisible(ok);
            fullNameBtn->setVisible(ok);
        }
    });

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(fullName);
    layout->addWidget(inputLineEdit);
    layout->addWidget(fullNameBtn);
    layout->addStretch();
    w->setLayout(layout);
}

void AccountsModule::initName(QWidget *w)
{
    QLabel *shortnameBtn = new QLabel();
    shortnameBtn->setPixmap(QIcon::fromTheme("dcc_avatar").pixmap(12, 12));
    QLabel *shortName = new QLabel();
    shortName->setEnabled(false);
    auto updateName = [shortName](User *user, User *oldUser) {
        if (oldUser)
            disconnect(oldUser, 0, shortName, 0);

        shortName->setText(user->name());
        connect(user, &User::nameChanged, shortName, &QLabel::setText);
    };
    updateName(m_curUser, nullptr);
    connect(this, &AccountsModule::currentUserChanged, shortName, updateName);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(shortnameBtn);
    layout->addWidget(shortName);
    layout->addStretch();
    w->setLayout(layout);
}

void AccountsModule::initModifyButton(QWidget *w)
{
    QPushButton *modifyPassword = new QPushButton();
    modifyPassword->setEnabled(!(m_model->getIsSecurityHighLever() && m_curLoginUser->securityLever() != SecurityLever::Sysadm && m_curUser != m_curLoginUser));
    DWarningButton *deleteAccount = new DWarningButton();
    deleteAccount->setText(tr("Delete Account"));
    deleteAccount->setEnabled(!(m_model->getIsSecurityHighLever() && m_curLoginUser->securityLever() != SecurityLever::Sysadm && m_curUser != m_curLoginUser));
    auto setButtonFun = [](AccountsModule *module, DWarningButton *deleteAccount, QPushButton *modifyPassword) {
        bool enabled = !(module->m_model->getIsSecurityHighLever() && module->m_curLoginUser->securityLever() != SecurityLever::Sysadm && !module->m_curUser->isCurrentUser());
        modifyPassword->setText(module->m_curUser->isCurrentUser() ? tr("Change Password") : tr("Reset Password"));
        // 若选择当前登录的账户，则允许修改，选择其他账户，当前登录账户必需是管理员且其他账户未登录时才允许修改密码
        modifyPassword->setEnabled((!module->m_curUser->online() && module->isSystemAdmin(module->m_curLoginUser) && enabled) || module->m_curUser->isCurrentUser());
        deleteAccount->setEnabled(module->deleteUserBtnEnable());
    };
    std::function<void()> setButton = std::bind(setButtonFun, this, deleteAccount, modifyPassword);
    auto updateButton = [this, deleteAccount, setButton](User *user, User *oldUser) {
        if (oldUser)
            disconnect(oldUser, 0, deleteAccount, 0);
        setButton();
        connect(user, &User::onlineChanged, deleteAccount, setButton);
        connect(m_model, &UserModel::adminCntChange, deleteAccount, setButton);
    };
    updateButton(m_curUser, nullptr);
    connect(this, &AccountsModule::currentUserChanged, modifyPassword, updateButton);
    connect(modifyPassword, &QPushButton::clicked, this, &AccountsModule::onModifyPassword);
    connect(deleteAccount, &DWarningButton::clicked, this, &AccountsModule::onDeleteUser);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(8);
    layout->addWidget(modifyPassword);
    layout->addWidget(deleteAccount);
    layout->setContentsMargins(0, 0, 0, 0);
    w->setLayout(layout);
}

void AccountsModule::initAccountType(SettingsGroup *accountSettingsGrp)
{
    accountSettingsGrp->setBackgroundStyle(SettingsGroup::GroupBackground);
    accountSettingsGrp->getLayout()->setContentsMargins(0, 0, 0, 0);
    accountSettingsGrp->setContentsMargins(0, 0, 0, 0);
    accountSettingsGrp->layout()->setMargin(0);
    ComboxWidget *asAdministrator = new ComboxWidget;
    asAdministrator->setTitle(tr("Account Type"));
    asAdministrator->addBackground();
    QStringList options;
    options << tr("Standard User") << tr("Administrator");
    asAdministrator->setComboxOption(options);
    accountSettingsGrp->appendItem(asAdministrator);
    auto setTypeFun = [](AccountsModule *module, ComboxWidget *asAdministrator) {
        asAdministrator->setEnabled(module->deleteUserBtnEnable());
        asAdministrator->setCurrentIndex(module->isSystemAdmin(module->m_curUser) ? User::Administrator : User::StandardUser);
    };
    std::function<void()> setType = std::bind(setTypeFun, this, asAdministrator);

    auto updateType = [asAdministrator, setType](User *user, User *oldUser) {
        if (oldUser)
            disconnect(oldUser, 0, asAdministrator, 0);
        setType();

        connect(user, &User::userTypeChanged, asAdministrator, setType);
        connect(user, &User::onlineChanged, asAdministrator, setType);
    };
    updateType(m_curUser, nullptr);
    connect(m_model, &UserModel::adminCntChange, asAdministrator, [this, updateType]() { updateType(m_curUser, m_curUser); });
    connect(this, &AccountsModule::currentUserChanged, asAdministrator, updateType);
    connect(asAdministrator, &ComboxWidget::onIndexChanged, this, [this](const int userType) {
        m_worker->setAdministrator(m_curUser, User::UserType::Administrator == userType);
    });
}

void AccountsModule::initValidityDays(SettingsGroup *pwGroup)
{
    // 设置密码有效期
    pwGroup->setBackgroundStyle(SettingsGroup::GroupBackground);
    pwGroup->getLayout()->setContentsMargins(0, 0, 0, 0);
    pwGroup->setContentsMargins(0, 0, 0, 0);
    pwGroup->layout()->setMargin(0);

    auto pwHLayout = new QHBoxLayout;
    auto pwWidget = new SettingsItem;
    pwGroup->appendItem(pwWidget);
    pwWidget->setLayout(pwHLayout);
    pwHLayout->setContentsMargins(10, 0, 10, 0);

    QLabel *vlidityLabel = new QLabel(tr("Validity Days"));
    pwHLayout->addWidget(vlidityLabel, 0, Qt::AlignLeft);
    auto validityDaysBox = new AccountSpinBox();
    validityDaysBox->lineEdit()->setValidator(new QRegularExpressionValidator(QRegularExpression("[1-9]\\d{0,4}/^[1-9]\\d*$/"), validityDaysBox->lineEdit()));
    validityDaysBox->lineEdit()->setPlaceholderText("99999");
    validityDaysBox->setRange(1, 99999);
    pwHLayout->addWidget(validityDaysBox, 0, Qt::AlignRight);

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

        m_worker->setMaxPasswordAge(m_curUser, validityDaysBox->value());
    });

    connect(m_curUser, &User::passwordAgeChanged, validityDaysBox, &AccountSpinBox::setValue);

    validityDaysBox->setValue(m_curUser->passwordAge());
    validityDaysBox->valueChanged(m_curUser->passwordAge());

    auto setvalidityDaysFun = [](AccountsModule *module, AccountSpinBox *validityDaysBox) {
        validityDaysBox->setValue(module->m_curUser->passwordAge());
        validityDaysBox->valueChanged(module->m_curUser->passwordAge());
        validityDaysBox->setEnabled(!(module->m_model->getIsSecurityHighLever() && module->m_curLoginUser->securityLever() != SecurityLever::Sysadm && !module->m_curUser->isCurrentUser()));
    };
    std::function<void()> setvalidityDays = std::bind(setvalidityDaysFun, this, validityDaysBox);

    auto updateValidityDays = [validityDaysBox, setvalidityDays](User *user, User *oldUser) {
        if (oldUser)
            disconnect(oldUser, 0, validityDaysBox, 0);
        setvalidityDays();

        connect(user, &User::passwordAgeChanged, validityDaysBox, setvalidityDays);
    };
    updateValidityDays(m_curUser, nullptr);

    connect(this, &AccountsModule::currentUserChanged, validityDaysBox, updateValidityDays);
}

void AccountsModule::onCreateAccount()
{
    if (m_checkAuthorizationing)
        return;
    m_checkAuthorizationing = true;
    QWidget *w = qobject_cast<QWidget *>(sender());
    PolkitQt1::Authority::instance()->checkAuthorization("com.deepin.daemon.accounts.user-administration", PolkitQt1::UnixProcessSubject(getpid()),
                                                         PolkitQt1::Authority::AllowUserInteraction);

    connect(PolkitQt1::Authority::instance(), &PolkitQt1::Authority::checkAuthorizationFinished, w, [this, w](PolkitQt1::Authority::Result authenticationResult) {
        disconnect(PolkitQt1::Authority::instance(), nullptr, w, nullptr);
        m_checkAuthorizationing = false;
        if (PolkitQt1::Authority::Result::Yes != authenticationResult) {
            return;
        }
        CreateAccountPage *createAccountPage = new CreateAccountPage(m_worker, w);
        User *newUser = new User(this);
        createAccountPage->setAttribute(Qt::WA_DeleteOnClose);
        newUser->setUserType(m_curLoginUser->userType());
        createAccountPage->setModel(m_model, newUser);
        connect(createAccountPage, &CreateAccountPage::requestCreateUser, m_worker, &AccountsWorker::createAccount);
        connect(m_worker, &AccountsWorker::accountCreationFinished, createAccountPage, &CreateAccountPage::setCreationResult);
        connect(createAccountPage, &CreateAccountPage::requestCheckPwdLimitLevel, m_worker, &AccountsWorker::checkPwdLimitLevel);
        if (createAccountPage->exec() == QDialog::Accepted) {
            for (auto &&user : m_model->userList()) {
                if (user->name() == newUser->name()) {
                    setCurrentUser(user);
                    break;
                }
            }
        }
    });
}

void AccountsModule::onModifyPassword()
{
    if (m_checkAuthorizationing)
        return;
    m_checkAuthorizationing = true;
    QWidget *w = qobject_cast<QWidget *>(sender());
    PolkitQt1::Authority::instance()->checkAuthorization("com.deepin.daemon.accounts.user-administration", PolkitQt1::UnixProcessSubject(getpid()),
                                                         PolkitQt1::Authority::AllowUserInteraction);

    connect(PolkitQt1::Authority::instance(), &PolkitQt1::Authority::checkAuthorizationFinished, w, [this, w](PolkitQt1::Authority::Result authenticationResult) {
        disconnect(PolkitQt1::Authority::instance(), nullptr, w, nullptr);
        m_checkAuthorizationing = false;
        if (PolkitQt1::Authority::Result::Yes != authenticationResult) {
            return;
        }
        ModifyPasswdPage *modifyPasswdPage = new ModifyPasswdPage(m_curUser, m_curUser->isCurrentUser(), w);
        modifyPasswdPage->setAttribute(Qt::WA_DeleteOnClose);
        connect(modifyPasswdPage, &ModifyPasswdPage::requestChangePassword, m_worker, &AccountsWorker::setPassword);
        connect(modifyPasswdPage, &ModifyPasswdPage::requestResetPassword, m_worker, &AccountsWorker::resetPassword);

        connect(modifyPasswdPage, &ModifyPasswdPage::requestSetPasswordHint, m_worker, &AccountsWorker::setPasswordHint);
        connect(modifyPasswdPage, &ModifyPasswdPage::requestUOSID, m_worker, &AccountsWorker::getUOSID);
        connect(modifyPasswdPage, &ModifyPasswdPage::requestUUID, m_worker, &AccountsWorker::getUUID);
        connect(modifyPasswdPage, &ModifyPasswdPage::requestLocalBindCheck, m_worker, &AccountsWorker::localBindCheck);
        connect(modifyPasswdPage, &ModifyPasswdPage::requestStartResetPasswordExec, m_worker, &AccountsWorker::startResetPasswordExec);
        connect(modifyPasswdPage, &ModifyPasswdPage::requestSecurityQuestionsCheck, m_worker, &AccountsWorker::asyncSecurityQuestionsCheck);
        connect(modifyPasswdPage, &ModifyPasswdPage::requestCheckPwdLimitLevel, m_worker, &AccountsWorker::checkPwdLimitLevel);
        connect(m_worker, &AccountsWorker::localBindUbid, modifyPasswdPage, &ModifyPasswdPage::onLocalBindCheckUbid);
        connect(m_worker, &AccountsWorker::localBindError, modifyPasswdPage, &ModifyPasswdPage::onLocalBindCheckError);
        modifyPasswdPage->exec();
    });
}

void AccountsModule::onDeleteUser()
{
    QWidget *w = qobject_cast<QWidget *>(sender());
    if (!w)
        return;
    w->setEnabled(false);
    RemoveUserDialog d(m_curUser, w);
    if (d.exec() == QDialog::Accepted) {
        m_worker->deleteAccount(m_curUser, d.deleteHome());
    }
    w->setEnabled(true);
}

void AccountsModule::onModifyIcon()
{
    QWidget *w = qobject_cast<QWidget *>(sender());
    if (!w)
        return;
    AvatarListDialog *avatarListDialog = new AvatarListDialog(m_curUser, w);
    if (avatarListDialog->exec() == QDialog::Accepted) {
        QString avatarpath = avatarListDialog->getAvatarPath();
        if (!avatarpath.isEmpty() && avatarpath != m_curUser->currentAvatar())
            m_worker->setAvatar(m_curUser, avatarpath);
    }
    avatarListDialog->deleteLater();
}

void AccountsModule::setCurrentUser(User *user)
{
    if (user && m_curUser != user) {
        User *oldUser = m_curUser;
        m_curUser = user;
        if (oldUser) {
            disconnect(oldUser, 0, this, 0);
        }
        connect(m_curUser, &User::groupsChanged, this, &AccountsModule::changeUserGroup);
        changeUserGroup(m_curUser->groups());

        connect(m_curUser, &User::autoLoginChanged, this, &AccountsModule::updateLoginModule);
        connect(m_curUser, &User::nopasswdLoginChanged, this, &AccountsModule::updateLoginModule);
        updateLoginModule();
        emit currentUserChanged(m_curUser, oldUser);
    }
}

void AccountsModule::setGroupInfo(const QStringList &group)
{
    m_groupItemModel->clear();
    for (const QString &item : group) {
        GroupItem *it = new GroupItem(item);
        it->setCheckable(false);
        m_groupItemModel->appendRow(it);
    }
    if (m_curUser)
        changeUserGroup(m_curUser->groups());
}

void AccountsModule::userGroupClicked(const QModelIndex &index)
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

    m_worker->setGroups(m_curUser, curUserGroup);
}

void AccountsModule::changeUserGroup(const QStringList &groups)
{
    int row_count = m_groupItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_groupItemModel->item(i, 0);
        item->setCheckState(item && groups.contains(item->text()) ? Qt::Checked : Qt::Unchecked);
        item->setEnabled(item->text() != m_groupName);
    }
    m_groupItemModel->sort(0);
}

void AccountsModule::onGidChanged(const QString &gid)
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

bool AccountsModule::onEditingFinished(bool isValid, DLineEdit *fullNameEdit)
{
    const QString &userFullName = fullNameEdit->text();
    if (userFullName == m_curUser->fullname() || (!userFullName.isEmpty() && userFullName.simplified().isEmpty())) {
        fullNameEdit->lineEdit()->clearFocus();
        fullNameEdit->setVisible(false);
        if (fullNameEdit->isAlert()) {
            fullNameEdit->setAlert(false);
            fullNameEdit->hideAlertMessage();
        }
        return true;
    }
    if (!userFullName.isEmpty()) {
        if (isValid) {
            fullNameEdit->setAlert(true);
            fullNameEdit->showAlertMessage(tr("The full name has been used by other user accounts"), fullNameEdit, 2000);
            fullNameEdit->lineEdit()->selectAll();
            return false;
        }
        QList<User *> userList = m_model->userList();
        for (User *user : userList) {
            if (userFullName == user->fullname() || userFullName == user->name()) {
                fullNameEdit->setAlert(true);
                fullNameEdit->showAlertMessage(tr("The full name has been used by other user accounts"), fullNameEdit, 2000);
                fullNameEdit->lineEdit()->selectAll();
                return false;
            }
        }
        QList<QString> groupList = m_model->getAllGroups();
        for (QString &group : groupList) {
            if (userFullName == group && userFullName != m_curUser->name()) {
                fullNameEdit->setAlert(true);
                fullNameEdit->showAlertMessage(tr("The full name has been used by other user accounts"), fullNameEdit, 2000);
                fullNameEdit->lineEdit()->selectAll();
                return false;
            }
        }
    }
    fullNameEdit->lineEdit()->clearFocus();
    fullNameEdit->setVisible(false);
    if (fullNameEdit->isAlert()) {
        fullNameEdit->setAlert(false);
        fullNameEdit->hideAlertMessage();
    }

    m_worker->setFullname(m_curUser, fullNameEdit->text());
    return true;
}

void AccountsModule::setFullname(const QString &fullName, DLabel *fullNameLabel)
{
    QString fullname = fullName;
    fullNameLabel->setEnabled(true);
    if (fullname.simplified().isEmpty()) {
        fullname = tr("Full Name");
        fullNameLabel->setEnabled(false);
    } else if (fullname.toLocal8Bit().size() > 32) {
        for (auto i = 1; i <= fullname.size(); ++i) {
            if (fullname.left(i).toLocal8Bit().size() > 29) {
                fullname = fullname.left(i - 1) + QString("...");
                break;
            }
        }
    }
    fullNameLabel->setText(fullname.toHtmlEscaped());
}

void AccountsModule::onShowSafetyPage(const QString &errorTips)
{
    DDialog dlg("", errorTips, nullptr);
    dlg.setIcon(QIcon::fromTheme("preferences-system"));
    dlg.addButton(tr("Go to Settings"));
    dlg.addButton(tr("Cancel"), true, DDialog::ButtonWarning);
    connect(&dlg, &DDialog::buttonClicked, this, [=](int idx) {
        if (idx == 0) {
            DDBusSender()
                    .service("com.deepin.defender.hmiscreen")
                    .interface("com.deepin.defender.hmiscreen")
                    .path("/com/deepin/defender/hmiscreen")
                    .method(QString("ShowPage"))
                    .arg(QString("securitytools"))
                    .arg(QString("login-safety"))
                    .call();
        }
    });
    dlg.exec();
}

void AccountsModule::onLoginModule(ModuleObject *module)
{
    if (module == m_autoLoginModule) {
        bool autoLogin = !m_curUser->autoLogin();
        if (autoLogin) {
            const QString &existedAutoLoginUserName = getOtherUserAutoLogin();
            if (existedAutoLoginUserName.isEmpty()) {
                m_worker->setAutoLogin(m_curUser, autoLogin);
            } else {
                DDialog *tipDialog = new DDialog(qobject_cast<QWidget *>(sender()));
                tipDialog->setIcon(QIcon::fromTheme("dialog-warning"));
                tipDialog->setModal(true);
                tipDialog->setAttribute(Qt::WA_DeleteOnClose);
                tipDialog->addButton(tr("OK"));
                tipDialog->setMessage(tr("\"Auto Login\" can be enabled for only one account, please disable it for the account \"%1\" first").arg(existedAutoLoginUserName));
                tipDialog->setFixedWidth(422);
                tipDialog->show();
            }
        } else {
            m_worker->setAutoLogin(m_curUser, autoLogin);
        }
    } else if (module == m_loginWithoutPasswordModule) {
        m_worker->setNopasswdLogin(m_curUser, !m_curUser->nopasswdLogin());
    }
}

void AccountsModule::updateLoginModule()
{
    m_autoLoginModule->setRightIcon(m_curUser->autoLogin() ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked);
    m_loginWithoutPasswordModule->setRightIcon(m_curUser->nopasswdLogin() ? DStyle::SP_IndicatorChecked : DStyle::SP_IndicatorUnchecked);
}

QString AccountsModule::getOtherUserAutoLogin()
{
    for (auto user : m_model->userList()) {
        if (user->name() != m_curUser->name() && user->autoLogin()) {
            return user->name();
        }
    }
    return "";
}

bool AccountsModule::deleteUserBtnEnable()
{
    auto adminCnt = [this]() {
        int cnt = 0;
        if (m_model->getIsSecurityHighLever()) {
            return 1;
        }
        for (auto user : m_model->userList()) {
            if (user->userType() == User::UserType::Administrator)
                cnt++;
        }
        return cnt;
    };

    auto isOnlyAdmin = [this, adminCnt] { // 是最后一个管理员
        return isSystemAdmin(m_curUser)   // 是管理员
                && adminCnt() == 1;       // 管理员只有一个
    };

    if (m_model->getIsSecurityHighLever()) {
        return m_curLoginUser->securityLever() == SecurityLever::Sysadm && !m_curUser->isCurrentUser();
    }
    return !m_curUser->isCurrentUser() // 不是当前用户
            && !m_curUser->online()    // 未登录
            && !isOnlyAdmin();         // 不是最后一个管理员
}
