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

#include "createaccountpage.h"
#include "widgets/titlelabel.h"
#include "window/utils.h"
#include "groupitem.h"

#include <DFontSizeManager>

#include <QtGlobal>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QDebug>
#include <QSettings>
#include <QApplication>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

CreateAccountPage::CreateAccountPage(QWidget *parent)
    : QWidget(parent)
    , m_newUser{nullptr}
    , m_avatarListWidget(nullptr)
    , m_nameEdit(new DLineEdit)
    , m_fullnameEdit(new DLineEdit)
    , m_passwdEdit(new DPasswordEdit)
    , m_repeatpasswdEdit(new DPasswordEdit)
    , m_accountChooser(new DComboBox)
    , m_groupListView(nullptr)
    , m_groupItemModel(nullptr)
    , m_groupTip(new QLabel(tr("Group")))
    , m_passwordMinLength(-1)
    , m_passwordMaxLength(-1)
    , m_validate_Required(-1)
{
    m_groupListView = new DListView(this);
    m_isServerSystem = IsServerSystem;
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    mainContentLayout->setMargin(0);
    setLayout(mainContentLayout);

    m_scrollArea = new QScrollArea;
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);
    mainContentLayout->addWidget(m_scrollArea);

    auto contentLayout = new QVBoxLayout();
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    m_tw = new QWidget();
    m_tw->setLayout(contentLayout);
    contentLayout->setSpacing(0);
    contentLayout->setMargin(0);
    m_scrollArea->setWidget(m_tw);

    initWidgets(contentLayout);
    if (m_isServerSystem) {
        contentLayout->addSpacing(List_Interval);
        initUsrGroup(contentLayout);
    }

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);

    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *addBtn = new DSuggestButton(tr("Create"));
    btnLayout->addWidget(cancleBtn);
    btnLayout->addWidget(addBtn);
    mainContentLayout->addSpacing(0);
    mainContentLayout->addLayout(btnLayout);

    connect(cancleBtn, &QPushButton::clicked, this, [&] {
        Q_EMIT requestBack();
    });
    connect(addBtn, &DSuggestButton::clicked, this, &CreateAccountPage::createUser);
}

CreateAccountPage::~CreateAccountPage()
{
    m_repeatpasswdEdit->hideAlertMessage();
}

void CreateAccountPage::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
    if (m_tw) {
        m_tw->resize(m_scrollArea->size());
    }
}

void CreateAccountPage::initUsrGroup(QVBoxLayout *layout)
{
    m_groupItemModel = new QStandardItemModel(this);
    m_groupListView->setModel(m_groupItemModel);
    m_groupListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_groupListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_groupListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_groupListView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_groupListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_groupListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_groupListView->setSpacing(1);
    connect(m_groupListView, &DListView::clicked, this, [=](const QModelIndex &index) {
        QStandardItem *item = m_groupItemModel->item(index.row() ,index.column());
        Qt::CheckState state = item->checkState();
        if (state == Qt::Checked) {
            item->setCheckState(Qt::Unchecked);
        } else {
            item->setCheckState(Qt::Checked);
        }
        m_groupItemModel->sort(0);
    });
    layout->addWidget(m_groupTip);
    layout->addSpacing(List_Interval);
    layout->addWidget(m_groupListView);

    if (m_accountChooser->currentIndex() != 2) {
        m_groupTip->setVisible(false);
        m_groupListView->setVisible(false);
    }
}

void CreateAccountPage::initWidgets(QVBoxLayout *layout)
{
    //~ contents_path /accounts/New Account
    TitleLabel *titleLabel = new TitleLabel(tr("New Account"));
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 10, 0);
    layout->addWidget(titleLabel);

    m_avatarListWidget = new AvatarListWidget(m_newUser, this);
    m_avatarListWidget->setAccessibleName("List_useravatarlist");
    m_avatarListWidget->setAvatarSize(QSize(40, 40));
    m_avatarListWidget->setViewportMargins(0, 0, 0, 0);
    m_avatarListWidget->setSpacing(14);
    m_avatarListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addSpacing(7);
    layout->addWidget(m_avatarListWidget, 0, Qt::AlignTop);

    if (m_isServerSystem) {
        QLabel *accountTypeLabel = new QLabel(tr("Account Type") + ':');
        layout->addWidget(accountTypeLabel);
        layout->addWidget(m_accountChooser);
        layout->addSpacing(7);
    }

    QLabel *nameLabel = new QLabel(tr("Username") + ':');
    m_nameEdit->setAccessibleName("username_edit");
    layout->addWidget(nameLabel);
    layout->addWidget(m_nameEdit);
    layout->addSpacing(7);

    QLabel *fullnameLabel = new QLabel(tr("Full Name") + ':');
    m_fullnameEdit->setAccessibleName("fullname_edit");
    layout->addWidget(fullnameLabel);
    layout->addWidget(m_fullnameEdit);
    layout->addSpacing(7);

    QLabel *passwdLabel = new QLabel(tr("Password") + ':');
    m_passwdEdit->setAccessibleName("passwd_edit");
    layout->addWidget(passwdLabel);
    layout->addWidget(m_passwdEdit);
    layout->addSpacing(7);

    QLabel *repeatpasswdLabel = new QLabel(tr("Repeat Password") + ':');
    m_repeatpasswdEdit->setAccessibleName("repeatpasswd_edit");
    layout->addWidget(repeatpasswdLabel);
    layout->addWidget(m_repeatpasswdEdit);
    layout->addSpacing(7);

    connect(m_avatarListWidget, &AvatarListWidget::requestSetAvatar,
            m_avatarListWidget, &AvatarListWidget::setCurrentAvatarChecked);
    connect(m_avatarListWidget, &AvatarListWidget::requestAddNewAvatar,
    this, [this](dcc::accounts::User *user, const QString &file) {
        Q_UNUSED(user)
        m_avatarListWidget->setCurrentAvatarChecked(file);
    });

    connect(m_nameEdit, &DLineEdit::textEdited, this, [ = ](const QString &strText) {
        Q_UNUSED(strText);
        if (m_nameEdit->isAlert()) {
            m_nameEdit->hideAlertMessage();
            m_nameEdit->setAlert(false);
        }

        if (strText.isEmpty())
            return;

        QString strTemp;
        int idx;
        int illegalNum = 0;
        for (idx = 0; idx < strText.size(); ++idx) {
            if ((strText[idx] >= '0' && strText[idx] <= '9') ||
                (strText[idx] >= 'a' && strText[idx] <= 'z') ||
                (strText[idx] >= 'A' && strText[idx] <= 'Z') ||
                (strText[idx] == '-' || strText[idx] == '_')) {
                strTemp.append(strText[idx]);
            } else {
                illegalNum++;
                continue;
            }
        }
        idx = m_nameEdit->lineEdit()->cursorPosition() - illegalNum;

        m_nameEdit->lineEdit()->blockSignals(true);
        m_nameEdit->lineEdit()->setText(strTemp);
        m_nameEdit->lineEdit()->setCursorPosition(idx);
        m_nameEdit->lineEdit()->blockSignals(false);
    });

    connect(m_fullnameEdit, &DLineEdit::textEdited, this, [ = ] {
        if (m_fullnameEdit->isAlert()){
            m_fullnameEdit->hideAlertMessage();
            m_fullnameEdit->setAlert(false);
        }
    });

    connect(m_passwdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_passwdEdit->isAlert()) {
            m_passwdEdit->hideAlertMessage();
            m_passwdEdit->setAlert(false);
        }
        if (m_repeatpasswdEdit->isAlert()) {
            m_repeatpasswdEdit->hideAlertMessage();
            m_repeatpasswdEdit->setAlert(false);
        }
    });

    connect(m_repeatpasswdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_repeatpasswdEdit->isAlert()) {
            m_repeatpasswdEdit->hideAlertMessage();
            m_repeatpasswdEdit->setAlert(false);
        }
    });

    connect(m_accountChooser, &DComboBox::currentTextChanged, this, &CreateAccountPage::showGroupList);

    m_accountChooser->addItem(tr("Standard"));
    m_accountChooser->addItem(tr("Administrator"));
    m_accountChooser->addItem(tr("Customized"));

    m_nameEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填
    m_fullnameEdit->lineEdit()->setPlaceholderText(tr("optional"));//选填
    m_passwdEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填
    m_repeatpasswdEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填
}

void CreateAccountPage::showGroupList(const QString &index)
{
    Q_UNUSED(index)

    if (m_accountChooser->currentIndex() == 2) {
        m_groupTip->setVisible(true);
        m_groupListView->setVisible(true);
    } else {
        m_groupTip->setVisible(false);
        m_groupListView->setVisible(false);
    }
}

void CreateAccountPage::setModel(UserModel *userModel, User *user)
{
    m_newUser = user;
    Q_ASSERT(m_newUser);
    m_userModel = userModel;
    Q_ASSERT(m_userModel);

    if (!m_groupItemModel) {
        return;
    }
    m_groupItemModel->clear();
    for(QString item : m_userModel->getAllGroups()) {
        GroupItem *it = new GroupItem(item);
        it->setCheckable(false);
        m_groupItemModel->appendRow(it);
    }

    QStringList presetGroup = m_userModel->getPresetGroups();
    int row_count = m_groupItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_groupItemModel->item(i, 0);
        if (item && presetGroup.contains(item->text())) {
            item->setCheckState(Qt::Checked);
        } else {
            item->setCheckState(Qt::Unchecked);
        }
    }
    m_groupItemModel->sort(0);
}

void CreateAccountPage::createUser()
{
    //校验输入的用户名和密码
    if (!onNameEditFinished(m_nameEdit)) {
        return;
    }
    if (!onPasswordEditFinished(m_passwdEdit, m_repeatpasswdEdit)) {
        return;
    }
    if (!onFullNameEidtFinished(m_fullnameEdit)) {
        return;
    }

    //如果用户没有选图像
    auto avatarPaht = m_avatarListWidget->getAvatarPath();
    m_newUser->setCurrentAvatar(avatarPaht);
    m_newUser->setName(m_nameEdit->lineEdit()->text());
    m_newUser->setFullname(m_fullnameEdit->lineEdit()->text());
    m_newUser->setPassword(m_passwdEdit->lineEdit()->text());
    m_newUser->setRepeatPassword(m_repeatpasswdEdit->lineEdit()->text());

    if (m_isServerSystem) {
        if (m_accountChooser->currentIndex() == 1) {
            QDBusInterface inter("com.deepin.daemon.Accounts", "/com/deepin/daemon/Accounts",
                                 "com.deepin.daemon.Accounts", QDBusConnection::systemBus());
            QDBusPendingReply<QStringList> reply = inter.call("GetPresetGroups", 1);
            m_newUser->setGroups(reply.value());
        } else {
            QStringList usrGroups;
            int row_count = m_groupItemModel->rowCount();
            for (int i = 0; i < row_count; ++i) {
                QStandardItem *item = m_groupItemModel->item(i, 0);
                if (item->checkState() == Qt::Checked) {
                    usrGroups << item->text();
                }
            }
            m_newUser->setGroups(usrGroups);
        }
    }

    DaemonService *daemonservice = new DaemonService("com.deepin.defender.daemonservice",
                                                     "/com/deepin/defender/daemonservice",
                                                     QDBusConnection::sessionBus(), this);
    QString strPwd = m_passwdEdit->lineEdit()->text();
    if (strPwd.length() >= daemonservice->GetPwdLen() && m_newUser->charactertypes(strPwd) >= daemonservice->GetPwdTypeLen()) {
        Q_EMIT requestCreateUser(m_newUser);
    } else {
        DDialog dlg("", daemonservice->GetPwdError());
        dlg.setIcon(QIcon::fromTheme("preferences-system"));
        dlg.addButton(tr("Go to Settings"));
        dlg.addButton(tr("Cancel"), true, DDialog::ButtonWarning);
        connect(&dlg, &DDialog::buttonClicked, this, [this](int idx){
            if (idx == 0) {
                Defender *defender = new Defender("com.deepin.defender.hmiscreen",
                                                  "/com/deepin/defender/hmiscreen",
                                                  QDBusConnection::sessionBus(), this);
                defender->ShowModule("systemsafety");
            }
        });
        dlg.exec();
    }
}

int  CreateAccountPage::passwordCompositionType(const QStringList &validate, const QString &password)
{
    return static_cast<int>(std::count_if(validate.cbegin(), validate.cend(),
                                          [=](const QString &policy) {
                                              for (const QChar &c : policy) {
                                                  if (password.contains(c)) {
                                                      return true;
                                                  }
                                              }
                                              return false;
                                          }));
}

int CreateAccountPage::verifyPassword(const QString &password)
{
    // NOTE(justforlxz): 配置文件由安装器生成，后续改成PAM模块
    QFileInfo fileInfo("/etc/deepin/dde.conf");
    if (fileInfo.isFile()) {
        QSettings setting("/etc/deepin/dde.conf", QSettings::IniFormat);
        setting.beginGroup("Password");
        const bool strong_password_check = setting.value("STRONG_PASSWORD", false).toBool();
        m_passwordMinLength   = setting.value("PASSWORD_MIN_LENGTH").toInt();
        m_passwordMaxLength   = setting.value("PASSWORD_MAX_LENGTH").toInt();
        m_validate_Required   = setting.value("VALIDATE_REQUIRED").toInt();
        QStringList validate_policy = setting.value("VALIDATE_POLICY").toString().split(";");
        setting.endGroup();

        //由于安装器之前配置文件中,反斜杠被当成转义字符了,这里确保校验规则中包含以下字符: "\", "|", "空格符"
        for (auto &policy : validate_policy) {
            if (policy.contains("~")) {
                if (!policy.contains("\\")) {
                    policy.append("\\");
                }
                if (!policy.contains("|")) {
                    policy.append("|");
                }
                if (!policy.contains(" ")) {
                    policy.append(" ");
                }
            }
        }

        // 如果不是强密码校验,则直接返回成功
        if (!strong_password_check) {
            return ENUM_PASSWORD_SUCCESS;
        }

        if (password.size() == 0) {
            return ENUM_PASSWORD_NOTEMPTY;
        } else if (password.size() > 0 && password.size() < m_passwordMinLength) {
            return ENUM_PASSWORD_TOOSHORT;
        }

        // 密码不能包含校验规则字符之外的其他字符
        QString allPolicyStr;
        for (auto policy : validate_policy) {
            allPolicyStr.append(policy);
        }
        if (!containsChar(password, allPolicyStr)) {
            return ENUM_PASSWORD_SEVERAL;
        }

        // 密码至少同时包括小写字母,大写字母,数字,符号中的m_validate_Required种
        if (passwordCompositionType(validate_policy, password) < m_validate_Required) {
            return ENUM_PASSWORD_SEVERAL;
        }

        // 密码不可为用户名重复或倒置
        QString username = m_nameEdit->lineEdit()->text();
        QString reversusername;
        QStringList reversenamelist;
        for (int i = username.count() - 1; i > -1; i--) {
            reversenamelist << username.at(i);
        }
        reversusername = reversenamelist.join("");
        if (password == username || password == reversusername) {
            return ENUM_PASSWORD_REPEATED;
        }

        // 密码长度不能超过m_passwordMinLength个字符
        if (password.size() > m_passwordMaxLength) {
            return ENUM_PASSWORD_TOOLONG;
        }

        return ENUM_PASSWORD_SUCCESS;
    } else {
        QString validate_policy = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") +
                                      QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ") + QString("~!@#$%^&*()[]{}\\|/?,.<> ");
        bool ret = containsChar(password, validate_policy);
        if (!ret) {
            return ENUM_PASSWORD_CHARACTER;
        }

        return ENUM_PASSWORD_SUCCESS;
    }
}

bool CreateAccountPage::containsChar(const QString &password, const QString &validate)
{
    for (const QChar &p : password) {
        if (!validate.contains(p)) {
            return false;
        }
    }

    return true;
}

void CreateAccountPage::setCreationResult(CreationResult *result)
{
    switch (result->type()) {
    case CreationResult::NoError:
        Q_EMIT requestBack(AccountsWidget::CreateUserSuccess);
        break;
    case CreationResult::UserNameError:
        m_nameEdit->setAlert(true);
        m_nameEdit->showAlertMessage(result->message(), -1);
        break;
    case CreationResult::PasswordError:
        m_passwdEdit->setAlert(true);
        m_passwdEdit->showAlertMessage(result->message(), -1);
        break;
    case CreationResult::PasswordMatchError:
        m_repeatpasswdEdit->setAlert(true);
        m_repeatpasswdEdit->showAlertMessage(result->message(), -1);
        break; // reserved for future server edition feature.
    case CreationResult::UnknownError:
        qDebug() << "error encountered creating user: " << result->message();
        break;
    }

    result->deleteLater();
}

bool CreateAccountPage::onPasswordEditFinished(DPasswordEdit *passwdEdit, DPasswordEdit *repeatpasswdEdit)
{
    const QString &userpassword = passwdEdit->lineEdit()->text();
    const QString &repeatpassword = repeatpasswdEdit->lineEdit()->text();
    if (userpassword.isEmpty()) {
        passwdEdit->setAlert(true);
        return false;
    }

    if (repeatpassword.isEmpty()) {
        repeatpasswdEdit->setAlert(true);
        return false;
    }

    if (userpassword != repeatpassword) {
        m_repeatpasswdEdit->setAlert(true);
        m_repeatpasswdEdit->showAlertMessage(tr("Passwords do not match"), -1);
        return false;
    }

    int passResult = verifyPassword(userpassword);
    switch (passResult)
    {
    case ENUM_PASSWORD_NOTEMPTY:
        passwdEdit->setAlert(true);
        passwdEdit->showAlertMessage(tr("Password cannot be empty"), -1);
        return false;
    case ENUM_PASSWORD_TOOSHORT:
    case ENUM_PASSWORD_SEVERAL:
        passwdEdit->setAlert(true);
        passwdEdit->showAlertMessage(tr("The password must have at least %1 characters, and contain at least %2 of the four available character types: lowercase letters, uppercase letters, numbers, and symbols").arg(m_passwordMinLength).arg(m_validate_Required), -1);
        return false;
    case ENUM_PASSWORD_TOOLONG:
        passwdEdit->setAlert(true);
        passwdEdit->showAlertMessage(tr("Password must be no more than %1 characters").arg(m_passwordMaxLength), -1);
        return false;
    case ENUM_PASSWORD_TYPE:
        passwdEdit->setAlert(true);
        passwdEdit->showAlertMessage(tr("The password should contain at least %1 of the four available character types: lowercase letters, uppercase letters, numbers, and symbols").arg(m_validate_Required), -1);
        return false;
    case ENUM_PASSWORD_CHARACTER:
        passwdEdit->setAlert(true);
        passwdEdit->showAlertMessage(tr("Password can only contain English letters (case-sensitive), numbers or special symbols (~!@#$%^&*()[]{}\\|/?,.<>)"), -1);
        return false;
    case ENUM_PASSWORD_REPEATED:
        passwdEdit->setAlert(true);
        passwdEdit->showAlertMessage(tr("Password should not be the repeated or reversed username"), -1);
        return false;
    }

    return true;
}

bool CreateAccountPage::validateUsername(const QString &username)
{
    const QString name_validate = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") +
                                  QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ") + QString("-_");
    return containsChar(username, name_validate);
}

bool CreateAccountPage::onNameEditFinished(DLineEdit *edit)
{
    const QString &username = edit->lineEdit()->text();
    if (username.isEmpty()) {
        edit->setAlert(true);
        return false;
    }

    if (username.size() < 3 || username.size() > 32) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Username must be between 3 and 32 characters"), -1);
        return false;
    }

    const QString compStr = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") + QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (!compStr.contains(username.at(0))) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("The first character must be a letter or number"), -1);
        return false;
    }

    if(username.contains(QRegExp("^\\d+$"))) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Your username should not only have numbers"), -1);
        return false;
    }

    if (!validateUsername(username)) {
        edit->setAlert(true);
        return false;
    }
    return true;
}

bool CreateAccountPage::onFullNameEidtFinished(DLineEdit *edit)
{
    auto userFullName = edit->lineEdit()->text();
    auto userList = m_userModel->userList();
    for (auto u : userList) {
        if (u->fullname() == userFullName && u->fullname() != nullptr) {
            edit->setAlert(true);
            edit->showAlertMessage(tr("The full name already exists"), -1);
            return false;
        }
    }

    if (userFullName.size() > 32) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("The full name is too long"), -1);
        return false;
    }
    return true;
}
