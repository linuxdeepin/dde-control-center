//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "accountslistview.h"
#include "accountsmodule.h"
#include "avatarwidget.h"
#include "createaccountpage.h"
#include "dwidgetutil.h"
#include "groupitem.h"
#include "modifypasswdpage.h"
#include "removeuserdialog.h"

#include "operation/accountsworker.h"
#include "operation/user.h"
#include "operation/usermodel.h"
#include "securityquestionspage.h"

#include "window/accountsmodel.h"

#include "widgets/listviewmodule.h"
#include "widgets/moduleobjectitem.h"
#include "widgets/horizontalmodule.h"
#include "widgets/itemmodule.h"
#include "widgets/dcclistview.h"

#include <QStringList>
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include <QListView>
#include <QListWidget>
#include <QScroller>
#include <QStackedWidget>
#include <QResizeEvent>

#include <DDialog>
#include <DLabel>
#include <DToolButton>
#include <DWarningButton>
#include <DSysInfo>
#include <DDesktopServices>
#include <DFloatingButton>
#include <DBlurEffectWidget>
#include <DIconTheme>

#include <polkit-qt5-1/PolkitQt1/Authority>

#include <DDBusSender>
#include <grp.h>

using namespace DCC_NAMESPACE;
DCORE_USE_NAMESPACE
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

QString AccountsPlugin::name() const
{
    return QStringLiteral("accounts");
}

ModuleObject *AccountsPlugin::module()
{
    return new AccountsModule(this);
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
    : PageModule("accounts", tr("Users"), tr("User management") , DIconTheme::findQIcon("dcc_nav_accounts"), parent)
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

    HorizontalModule *horModule = new HorizontalModule("accountsList", QString());
    horModule->appendChild(new ItemModule("accountsList", QString(), this, &AccountsModule::initAccountsList, false));
    horModule->appendChild(new ItemModule("createAccount", tr("Create User"), this, &AccountsModule::initCreateAccount, false), 0, Qt::AlignRight);
    appendChild(horModule);
    appendChild(new ItemModule("avatar", QString(), this, &AccountsModule::initAvatar, false), 0, Qt::AlignHCenter);
    horModule = new HorizontalModule("fullName", QString());
    horModule->setStretchType(HorizontalModule::AllStretch);
    m_fullNameModule = new ItemModule("fullName", QString(), this, &AccountsModule::initFullName, false);
    m_fullNameModule->setNoSearch(true);
    horModule->appendChild(m_fullNameModule);
    m_fullNameEditModule = new ItemModule("fullNameEdit", QString(), this, &AccountsModule::initFullNameEdit, false);
    horModule->appendChild(m_fullNameEditModule);
    m_fullNameEditModule->setHidden(true);
    m_fullNameIconModule = new ItemModule("fullNameIcon", QString(), this, &AccountsModule::initFullNameIcon, false);
    connect(m_fullNameIconModule, &ModuleObject::stateChanged, this, &AccountsModule::updateFullnameVisible);
    horModule->appendChild(m_fullNameIconModule);
    appendChild(horModule);

    appendChild(new ItemModule("name", tr("Username"), this, &AccountsModule::initName, false));
    horModule = new HorizontalModule("button", QString());
    m_changePasswordModule = new ItemModule("changePassword", tr("Change Password"), this, &AccountsModule::initChangePassword, false);
    horModule->appendChild(m_changePasswordModule);
    m_deleteAccountModule =new ItemModule("deleteUser", tr("Delete User"), this, &AccountsModule::initDeleteAccount, false);
    horModule->appendChild(m_deleteAccountModule);
    appendChild(horModule);

    ItemModule *accountTypeModule = new ItemModule("accountType", tr("User Type"), this, &AccountsModule::initAccountType);
    accountTypeModule->setBackground(true);
    m_accountTypeModule = accountTypeModule;
    appendChild(m_accountTypeModule);

    ListViewModule *listViewModule = new ListViewModule("autoLogin", tr("Auto Login"));
    connect(listViewModule, &ListViewModule::clicked, this, &AccountsModule::onLoginModule);
    appendChild(listViewModule);
    m_autoLoginModule = new ModuleObjectItem("autoLogin", tr("Auto Login"));
    listViewModule->appendChild(m_autoLoginModule);
    m_loginWithoutPasswordModule = new ModuleObjectItem("loginWithoutPassword", tr("Login Without Password"));
    listViewModule->appendChild(m_loginWithoutPasswordModule);

    ItemModule *validityDaysModule = new ItemModule("validityDays", tr("Validity Days"), this, &AccountsModule::initValidityDays);
    validityDaysModule->setBackground(true);
    m_validityDaysModule = validityDaysModule;
    appendChild(m_validityDaysModule);

    if (DSysInfo::UosServer == DSysInfo::uosType()) {
        appendChild(new ItemModule("group", tr("Group")));
        appendChild(new ItemModule(
                "groupListView", tr("Group"), [this](ModuleObject *module) {
                    Q_UNUSED(module)
                    DCCListView *groupListView = new DCCListView();
                    groupListView->setModel(m_groupItemModel);
                    groupListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
                    groupListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
                    groupListView->setSelectionMode(QAbstractItemView::NoSelection);
                    groupListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    groupListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    groupListView->setSpacing(1);
                    connect(groupListView, &DListView::clicked, this, &AccountsModule::userGroupClicked);
                    return groupListView;
                },
                false));
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

void AccountsModule::deactive()
{
    Q_EMIT deactivated();
}

bool AccountsModule::isSystemAdmin(User *user)
{
    // 本地管理员账户不一定是等保三级的管理员账户，要区分判断
    if (m_model->getIsSecurityHighLever())
        return user->securityLever() == SecurityLever::Sysadm;

    return user->userType() == User::UserType::Administrator;
}

static QWidget *findAncestorWidget(const QObject *obj)
{
    while (obj) {
        if (QWidget *w = qobject_cast<QWidget *>(obj->parent()))
            return w;
        obj = obj->parent();
    }

    return nullptr;
}

QWidget *AccountsModule::initAccountsList(ModuleObject *module)
{
    Q_UNUSED(module)

    QWidget *pw = findAncestorWidget(this);
    AccountsListView *userlistView = new AccountsListView(pw);
    userlistView->parentWidget();

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
        Q_UNUSED(previous)
        setCurrentUser(m_accountsmodel->getUser(current));
    });
    connect(this, &AccountsModule::currentUserChanged, userlistView, [this, userlistView](User *user, User *oldUser) {
        Q_UNUSED(oldUser)
        QModelIndex i = m_accountsmodel->index(user);
        if (userlistView->selectionModel()->currentIndex() != i) {
            userlistView->selectionModel()->setCurrentIndex(i, QItemSelectionModel::ClearAndSelect);
        }
    });
    userlistView->selectionModel()->select(m_accountsmodel->index(0, 0), QItemSelectionModel::SelectCurrent);
    return userlistView;
}

QWidget *AccountsModule::initCreateAccount(ModuleObject *module)
{
    Q_UNUSED(module)
    DFloatingButton *createBtn = new DFloatingButton(nullptr);
    createBtn->setIcon(DStyle::SP_IncreaseElement);
    createBtn->setFixedSize(50, 50);
    createBtn->setToolTip(tr("Create User"));
    createBtn->setAccessibleName(tr("Create User"));
    connect(createBtn, &QPushButton::clicked, this, &AccountsModule::onCreateAccount);
    return createBtn;
}

QWidget *AccountsModule::initAvatar(ModuleObject *module)
{
    Q_UNUSED(module)
    AvatarWidget *avatar = new AvatarWidget();
    avatar->setFixedSize(120, 120);
    avatar->setArrowed(false);
    auto updateUser = [avatar](User *user, User *oldUser) {
        if (!user) {
            return;
        }
        if (oldUser)
            disconnect(oldUser, 0, avatar, 0);

        avatar->setAvatarPath(user->currentAvatar());
        connect(user, &User::currentAvatarChanged, avatar, &AvatarWidget::setAvatarPath);
    };
    updateUser(m_curUser, nullptr);
    connect(this, &AccountsModule::currentUserChanged, avatar, updateUser);
    connect(avatar, &AvatarWidget::clicked, this, &AccountsModule::onModifyIcon);
    return avatar;
}

QWidget *AccountsModule::initFullName(ModuleObject *module)
{
    DLabel *fullName = new DLabel();
    fullName->setContentsMargins(0, 6, 0, 6);
    fullName->setElideMode(Qt::ElideRight);
    if (!m_curUser) {
        return fullName;
    }
    DFontSizeManager::instance()->bind(fullName, DFontSizeManager::T5);
    setFullname(m_curUser->fullname(), fullName);
    connect(module, &ModuleObject::displayNameChanged, fullName, [this, fullName](const QString &name) {
        setFullname(name, fullName);
    });

    return fullName;
}

QWidget *AccountsModule::initFullNameEdit(ModuleObject *module)
{
    Q_UNUSED(module)
    DLineEdit *inputLineEdit = new DLineEdit();
    inputLineEdit->setAccessibleName("fullName_edit");
    inputLineEdit->setMinimumWidth(220);
    inputLineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    inputLineEdit->lineEdit()->setFrame(false);
    inputLineEdit->lineEdit()->setAlignment(Qt::AlignCenter);
    inputLineEdit->lineEdit()->installEventFilter(this);
    DFontSizeManager::instance()->bind(inputLineEdit, DFontSizeManager::T5);

    if (!m_curUser) {
        return inputLineEdit;
    }

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

    connect(inputLineEdit, &DLineEdit::editingFinished, inputLineEdit, [inputLineEdit, this] {
        QString userFullName = inputLineEdit->lineEdit()->text();
        QDBusPendingReply<bool, QString, int> reply = m_worker->isUsernameValid(userFullName);
        //欧拉版会自己创建shutdown等root组账户且不会添加到userList中，导致无法重复性算法无效，先通过isUsernameValid校验这些账户再通过重复性算法校验
        // vaild == false && code ==6 是用户名已存在
        if (!reply.argumentAt(0).toBool() && PassErrorCode::ErrCodeSystemUsed == reply.argumentAt(2).toInt()) {
            onEditingFinished(true, inputLineEdit);
        } else {
            onEditingFinished(false, inputLineEdit);
        }
    });


    inputLineEdit->setAlert(false);
    inputLineEdit->setText(m_curUser->fullname());
    inputLineEdit->hideAlertMessage();
    QTimer::singleShot(10, inputLineEdit->lineEdit(), SLOT(setFocus()));
    return inputLineEdit;
}

QWidget *AccountsModule::initFullNameIcon(ModuleObject *module)
{
    DToolButton *fullNameBtn = new DToolButton();
    fullNameBtn->setAccessibleName("fullName_btn");
    fullNameBtn->setIcon(DIconTheme::findQIcon("dcc_edit"));
    fullNameBtn->setIconSize(QSize(12, 12));
    //点击用户全名编辑按钮
    connect(fullNameBtn, &DIconButton::clicked, module, [this]() {
        m_fullNameIconModule->setHidden(true);
    });
    return fullNameBtn;
}

QWidget *AccountsModule::initChangePassword(ModuleObject *module)
{
    QPushButton *modifyPassword = new QPushButton();
    modifyPassword->setText(module->displayName());
    connect(module, &ModuleObject::displayNameChanged, modifyPassword, &QPushButton::setText);
    connect(modifyPassword, &QPushButton::clicked, this, &AccountsModule::onModifyPassword);

    return modifyPassword;
}

QWidget *AccountsModule::initDeleteAccount(ModuleObject *module)
{
    Q_UNUSED(module)
    DWarningButton *deleteAccount = new DWarningButton();
    deleteAccount->setText(tr("Delete User"));
    connect(deleteAccount, &DWarningButton::clicked, this, &AccountsModule::onDeleteUser);
    return deleteAccount;
}

QWidget *AccountsModule::initAccountType(ModuleObject *module)
{
    Q_UNUSED(module)
    QComboBox *asAdministrator = new QComboBox();
    asAdministrator->addItems({ tr("Standard User"), tr("Administrator") });
    if (!m_curUser) {
        return asAdministrator;
    }
    auto updateType = [asAdministrator, this]() {
        asAdministrator->blockSignals(true);
        asAdministrator->setCurrentIndex(isSystemAdmin(m_curUser));
        asAdministrator->blockSignals(false);
    };
    updateType();
    connect(m_model, &UserModel::adminCntChange, asAdministrator, updateType);
    connect(this, &AccountsModule::currentUserChanged, asAdministrator, updateType);
    connect(asAdministrator, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](const int userType) {
        m_worker->setAdministrator(m_curUser, User::UserType::Administrator == userType);
    });
    return asAdministrator;
}

QWidget *AccountsModule::initValidityDays(ModuleObject *module)
{
    Q_UNUSED(module)
    AccountSpinBox *validityDaysBox = new AccountSpinBox();
    validityDaysBox->lineEdit()->setValidator(new QRegularExpressionValidator(QRegularExpression("[1-9]\\d{0,4}/^[1-9]\\d*$/"), validityDaysBox->lineEdit()));
    validityDaysBox->lineEdit()->setPlaceholderText("99999");
    validityDaysBox->setRange(1, 99999);

    connect(validityDaysBox, qOverload<int>(&DSpinBox::valueChanged), this, [=](const int value) {
        validityDaysBox->setValue(value);
        validityDaysBox->setAlert(false);
    });

    if (!m_curUser) {
        return validityDaysBox;
    }
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
    validityDaysBox->setValue(m_curUser->passwordAge());
    validityDaysBox->valueChanged(m_curUser->passwordAge());

    auto setvalidityDaysFun = [](AccountsModule *module, AccountSpinBox *validityDaysBox) {
        validityDaysBox->setValue(module->m_curUser->passwordAge());
        validityDaysBox->valueChanged(module->m_curUser->passwordAge());
        module->m_validityDaysModule->setEnabled(!(module->m_model->getIsSecurityHighLever() && module->m_curLoginUser->securityLever() != SecurityLever::Sysadm && !module->m_curUser->isCurrentUser()));
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
    return validityDaysBox;
}

QWidget *AccountsModule::initName(ModuleObject *module)
{
    Q_UNUSED(module)
    QWidget *w = new QWidget();
    QLabel *shortnameBtn = new QLabel();
    shortnameBtn->setPixmap(DIconTheme::findQIcon("dcc_avatar").pixmap(12, 12));
    QLabel *shortName = new QLabel();
    shortName->setEnabled(false);
    auto updateName = [shortName](User *user, User *oldUser) {
        if (!user) {
            return; 
        }
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
    return w;
}

void AccountsModule::onCreateAccount()
{
    if (m_checkAuthorizationing)
        return;
    m_checkAuthorizationing = true;
    QWidget *w = qobject_cast<QWidget *>(sender());
    PolkitQt1::Authority::instance()->checkAuthorization("org.deepin.dde.accounts.user-administration", PolkitQt1::UnixProcessSubject(getpid()),
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
    PolkitQt1::Authority::instance()->checkAuthorization("org.deepin.dde.accounts.user-administration", PolkitQt1::UnixProcessSubject(getpid()),
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
    RemoveUserDialog *d =  new RemoveUserDialog(m_curUser, w);
    d->deleteLater();
    if (d->exec() == QDialog::Accepted) {
        m_worker->deleteAccount(m_curUser, d->deleteHome());
    }
}

void AccountsModule::onModifyIcon()
{
    QWidget *w = qobject_cast<QWidget *>(sender());
    if (!w)
        return;

    AvatarListDialog avatarListDialog = AvatarListDialog(m_curUser, m_worker);

    if (avatarListDialog.exec() == QDialog::Rejected) {
        return;
    }

    auto path = avatarListDialog.get_path();
    if (path.has_value()) {
        m_worker->setAvatar(m_curUser, path.value());
    }
}

void AccountsModule::setCurrentUser(User *user)
{
    if (user && m_curUser != user) {
        User *oldUser = m_curUser;
        m_curUser = user;
        if (oldUser) {
            disconnect(oldUser, 0, this, 0);
        }
        connect(m_curUser, &User::gidChanged, this, &AccountsModule::onGidChanged);
        connect(m_curUser, &User::groupsChanged, this, &AccountsModule::changeUserGroup);
        onGidChanged(m_curUser->gid());
        changeUserGroup(m_curUser->groups());

        connect(m_curUser, &User::autoLoginChanged, this, &AccountsModule::updateLoginModule);
        connect(m_curUser, &User::nopasswdLoginChanged, this, &AccountsModule::updateLoginModule);
        updateLoginModule();
        m_fullNameIconModule->setHidden(false);
        m_fullNameModule->setDisplayName(m_curUser->fullname());
        connect(m_curUser, &User::fullnameChanged, this, [this](const QString &fullname) {
            m_fullNameModule->setDisplayName(fullname);
        });
        m_autoLoginModule->setEnabled(m_curUser->isCurrentUser());
        m_loginWithoutPasswordModule->setEnabled(m_curUser->isCurrentUser());
        m_changePasswordModule->setEnabled(!(m_model->getIsSecurityHighLever() && m_curLoginUser->securityLever() != SecurityLever::Sysadm && m_curUser != m_curLoginUser));
        m_changePasswordModule->setDisplayName(m_curUser->isCurrentUser() ? tr("Change Password") : tr("Reset Password"));
        bool deleteEnable = deleteUserBtnEnable();
        m_deleteAccountModule->setEnabled(deleteEnable);
        m_accountTypeModule->setEnabled(deleteEnable);
        connect(m_curUser, &User::onlineChanged, this, [this]() {
            bool deleteEnable = deleteUserBtnEnable();
            m_deleteAccountModule->setEnabled(deleteEnable);
            m_accountTypeModule->setEnabled(deleteEnable);
        });
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
        if (item) {
            item->setCheckState(groups.contains(item->text()) ? Qt::Checked
                                                                      : Qt::Unchecked);
            item->setEnabled(item->text() != m_groupName);
        }
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
        m_fullNameIconModule->setVisible(true);
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
    m_fullNameIconModule->setVisible(true);
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
    m_fullNameModule->setEnabled(true);
    if (fullname.simplified().isEmpty()) {
        fullname = tr("Full Name");
        m_fullNameModule->setEnabled(false);
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

void AccountsModule::updateFullnameVisible(uint32_t flag, bool state)
{
    Q_UNUSED(state)
    if (ModuleObject::IsHiddenFlag(flag)) {
        m_fullNameModule->setHidden(m_fullNameIconModule->isHidden());
        m_fullNameEditModule->setHidden(!m_fullNameIconModule->isHidden());
    }
}

void AccountsModule::onShowSafetyPage(const QString &errorTips)
{
    DDialog dlg("", errorTips, nullptr);
    dlg.setIcon(DIconTheme::findQIcon("preferences-system"));
    dlg.addButton(tr("Go to Settings"));
    dlg.addButton(tr("Cancel"), true, DDialog::ButtonWarning);
    connect(this, &AccountsModule::deactivated, &dlg, &DDialog::close);
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
                tipDialog->setIcon(DIconTheme::findQIcon("dialog-warning"));
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
