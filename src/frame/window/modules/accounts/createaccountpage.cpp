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
#include "pwqualitymanager.h"

#include <DFontSizeManager>
#include <DDesktopServices>
#include <DDBusSender>

#include <QtGlobal>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QDebug>
#include <QSettings>
#include <QApplication>
#include <QScroller>

DWIDGET_USE_NAMESPACE
using namespace dcc::accounts;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::accounts;

CreateAccountPage::CreateAccountPage(dcc::accounts::AccountsWorker *accountsWorker, QWidget *parent)
    : QWidget(parent)
    , m_newUser{nullptr}
    , m_accountWorker(accountsWorker)
    , m_avatarListWidget(nullptr)
    , m_nameEdit(new DLineEdit)
    , m_fullnameEdit(new DLineEdit)
    , m_passwdEdit(new DPasswordEdit)
    , m_repeatpasswdEdit(new DPasswordEdit)
    , m_accountChooser(new DComboBox)
    , m_groupListView(nullptr)
    , m_groupItemModel(nullptr)
    , m_groupTip(new QLabel(tr("Group")))
{
    m_groupListView = new DListView(this);
    m_isServerSystem = IsServerSystem;
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    setLayout(mainContentLayout);

    m_scrollArea = new QScrollArea;
    QScroller::grabGesture(m_scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameStyle(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setContentsMargins(0, 0, 0, 0);
    mainContentLayout->addWidget(m_scrollArea);

    m_tw = new QWidget(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(m_tw);
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(m_tw);

    initWidgets(contentLayout);
    initUsrGroup(contentLayout);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);

    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *addBtn = new DSuggestButton(tr("Create"));
    cancleBtn->setDefault(true);
    addBtn->setDefault(true);
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
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(titleLabel);

    m_avatarListWidget = new AvatarListWidget(m_newUser, this);
    m_avatarListWidget->setAccessibleName("List_useravatarlist");
    m_avatarListWidget->setAvatarSize(QSize(40, 40));
    m_avatarListWidget->setViewportMargins(0, 0, 0, 0);
    m_avatarListWidget->setSpacing(14);
    m_avatarListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    layout->addSpacing(7);
    layout->addWidget(m_avatarListWidget, 0, Qt::AlignTop);

    /* 用户类型 */
    QLabel *accountTypeLabel = new QLabel(tr("Account Type") + ':');
    layout->addWidget(accountTypeLabel);
    layout->addWidget(m_accountChooser);
    layout->addSpacing(7);

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
        for (idx = 0; idx < strText.size(); ++idx) {
            if ((strText[idx] >= '0' && strText[idx] <= '9') ||
                (strText[idx] >= 'a' && strText[idx] <= 'z') ||
                (strText[idx] >= 'A' && strText[idx] <= 'Z') ||
                (strText[idx] == '-' || strText[idx] == '_')) {
                strTemp.append(strText[idx]);
            } else {
                DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
                break;
            }
        }

        m_nameEdit->lineEdit()->blockSignals(true);
        int cursorIndex = m_nameEdit->lineEdit()->cursorPosition();
        m_nameEdit->lineEdit()->setText(strTemp);
        m_nameEdit->lineEdit()->setCursorPosition(cursorIndex);
        m_nameEdit->lineEdit()->blockSignals(false);
    });

    connect(m_nameEdit, &DLineEdit::editingFinished, this, &CreateAccountPage::checkName);

    connect(m_fullnameEdit, &DLineEdit::textEdited, this, [ = ](const QString &userFullName) {
        /* 在输入的过程中仅检查全名的长度，输入完成后检查其它规则 */
        if (userFullName.size() > 32) {
            m_fullnameEdit->lineEdit()->backspace();
            m_fullnameEdit->setAlert(true);
            m_fullnameEdit->showAlertMessage(tr("The full name is too long"), m_fullnameEdit, 2000);
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (m_fullnameEdit->isAlert()) {
            m_fullnameEdit->setAlert(false);
            m_fullnameEdit->hideAlertMessage();
        }
    });

    connect(m_fullnameEdit, &DLineEdit::editingFinished, this, &CreateAccountPage::checkFullname);

    connect(m_passwdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_passwdEdit->isAlert()) {
            m_passwdEdit->hideAlertMessage();
            m_passwdEdit->setAlert(false);
        }
    });

    connect(m_repeatpasswdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_repeatpasswdEdit->isAlert()) {
            m_repeatpasswdEdit->hideAlertMessage();
            m_repeatpasswdEdit->setAlert(false);
        }
    });

    connect(m_accountChooser, &DComboBox::currentTextChanged, this, &CreateAccountPage::showGroupList);

    m_accountChooser->addItem(tr("Standard User"));
    m_accountChooser->addItem(tr("Administrator"));
    /* 仅在服务器模式下创建用户才能自定义用户组 */
    if (m_isServerSystem) {
        m_accountChooser->addItem(tr("Customized"));
    }
    if (!m_isServerSystem) {
        accountTypeLabel->setVisible(false);
        m_accountChooser->setVisible(false);
        m_accountChooser->setCurrentIndex(1);
    }

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
        item->setCheckState(item && presetGroup.contains(item->text()) ? Qt::Checked : Qt::Unchecked);
    }
    m_groupItemModel->sort(0);
}

//在修改密码页面当前密码处设置焦点
void CreateAccountPage::showEvent(QShowEvent *event)
{
    if (m_nameEdit && !m_nameEdit->hasFocus()) {
        m_nameEdit->lineEdit()->setFocus();
    }
    QWidget::showEvent(event);
}

void CreateAccountPage::createUser()
{
    //校验输入的用户名和密码
    if (!checkName() || !checkFullname() || !checkPassword(m_repeatpasswdEdit) || !checkPassword(m_passwdEdit)) {
        return;
    }

    //如果用户没有选图像
    auto avatarPaht = m_avatarListWidget->getAvatarPath();
    m_newUser->setCurrentAvatar(avatarPaht);
    m_newUser->setName(m_nameEdit->lineEdit()->text().simplified());
    m_newUser->setFullname(m_fullnameEdit->lineEdit()->text());
    m_newUser->setPassword(m_passwdEdit->lineEdit()->text());
    m_newUser->setRepeatPassword(m_repeatpasswdEdit->lineEdit()->text());

    /* 设置用户组 */
    if (m_accountChooser->currentIndex() == 1) {
        m_newUser->setUserType(User::UserType::Administrator);
    } else if (m_accountChooser->currentIndex() == 0) {
        m_newUser->setUserType(User::UserType::StandardUser);
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
        m_newUser->setUserType(User::UserType::StandardUser);
    }
    Q_EMIT requestCreateUser(m_newUser); // 请求创建用户
}

void CreateAccountPage::setCreationResult(CreationResult *result)
{
    switch (result->type()) {
    case CreationResult::NoError:
        Q_EMIT requestBack(AccountsWidget::CreateUserSuccess);
        break;
    case CreationResult::UserNameError:
        m_nameEdit->setAlert(true);
        m_nameEdit->showAlertMessage(result->message(), m_nameEdit, 2000);
        break;
    case CreationResult::PasswordError:
        m_passwdEdit->setAlert(true);
        m_passwdEdit->showAlertMessage(result->message(), m_passwdEdit, 2000);
        break;
    case CreationResult::PasswordMatchError:
        m_repeatpasswdEdit->setAlert(true);
        m_repeatpasswdEdit->showAlertMessage(result->message(), m_repeatpasswdEdit, 2000);
        break; // reserved for future server edition feature.
    case CreationResult::UnknownError:
        //当用户名与用户组信息重名时,会返回UnknownError,并且提示信息是从系统中获取过来的,控制中心无法区分他的中英文
        qDebug() << "error encountered creating user: " << result->message();
        m_nameEdit->setAlert(true);
        if (result->message() == "Policykit authentication failed") {
            m_nameEdit->showAlertMessage(tr("Policykit authentication failed"), m_nameEdit, 2000);
        } else {
            m_nameEdit->showAlertMessage(result->message(), m_nameEdit, 2000);
        }
        break;
    case CreationResult::Canceled:
        // canceled
        break;
    }

    result->deleteLater();
}

bool CreateAccountPage::checkName()
{
    const QString &userName = m_nameEdit->lineEdit()->text();

    if (userName.size() < 3 || userName.size() > 32) {
        m_nameEdit->setAlert(true);
        m_nameEdit->showAlertMessage(tr("Username must be between 3 and 32 characters"), m_nameEdit, 2000);
        return false;
    }

    const QString compStr = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") + QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (!compStr.contains(userName.at(0))) {
        m_nameEdit->setAlert(true);
        m_nameEdit->showAlertMessage(tr("The first character must be a letter or number"), m_nameEdit, 2000);
        return false;
    }

    if (userName.contains(QRegExp("^\\d+$"))) {
        m_nameEdit->setAlert(true);
        m_nameEdit->showAlertMessage(tr("Your username should not only have numbers"), m_nameEdit, 2000);
        return false;
    }

    if (!m_accountWorker->isUsernameValid(userName).argumentAt(0).toBool() && NAME_ALREADY == m_accountWorker->isUsernameValid(userName).argumentAt(2).toInt()) {
        m_nameEdit->setAlert(true);
        m_nameEdit->showAlertMessage(tr("The name already exists"), m_nameEdit, 2000);
        return false;
    }


    /* 暂时先屏蔽用户名与用户全名的重复性检查 */
    // QList<User *> userList = m_userModel->userList();
    // for (User *user : userList) {
    //     if (userName == user->fullname()) {
    //         m_nameEdit->setAlert(true);
    //         m_nameEdit->showAlertMessage(tr("The name already exists"), m_nameEdit, 2000);
    //         return false;
    //     }
    // }

    if (m_nameEdit->isAlert()) {
        m_nameEdit->setAlert(false);
        m_nameEdit->hideAlertMessage();
    }

    return true;
}

bool CreateAccountPage::checkFullname()
{
    QString userFullName = m_fullnameEdit->lineEdit()->text();

    if (userFullName.size() > 32) {
        m_fullnameEdit->setAlert(true);
        m_fullnameEdit->showAlertMessage(tr("The full name is too long"), m_fullnameEdit, 2000);
        return false;
    }

    if (!userFullName.simplified().isEmpty()) {
        QList<QString> groupList = m_userModel->getAllGroups();
        /* 与系统中已有的用户名和用户组进行重复性校验 */
        for (QString &group : groupList) {
            if (userFullName == group) {
                m_fullnameEdit->setAlert(true);
                m_fullnameEdit->showAlertMessage(tr("The name already exists"), m_fullnameEdit, 2000);
                m_fullnameEdit->lineEdit()->selectAll();
                return false;
            }
        }
        QList<User *> userList = m_userModel->userList();
        /* 与已有的用户全名进行重复性校验 */
        for (User *user : userList) {
            if (userFullName == user->fullname()) {
                m_fullnameEdit->setAlert(true);
                m_fullnameEdit->showAlertMessage(tr("The name already exists"), m_fullnameEdit, 2000);
                m_fullnameEdit->lineEdit()->selectAll();
                return false;
            }
        }
    } else {
        m_fullnameEdit->lineEdit()->clear(); // 输入全空格不保存
    }

    if (m_fullnameEdit->isAlert()) {
        m_fullnameEdit->setAlert(false);
        m_fullnameEdit->hideAlertMessage();
    }

    return true;
}

bool CreateAccountPage::checkPassword(DPasswordEdit *edit)
{
    if (edit == m_repeatpasswdEdit) {
        if (m_passwdEdit->lineEdit()->text() != m_repeatpasswdEdit->lineEdit()->text()) {
            m_repeatpasswdEdit->setAlert(true);
            m_repeatpasswdEdit->showAlertMessage(tr("Passwords do not match"), this->parentWidget(), 2000);
            return false;
        }
    }

    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_nameEdit->lineEdit()->text(),
                                                                                      edit->lineEdit()->text());

    if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
        m_passwdEdit->setAlert(true);
        m_passwdEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));

        // 企业版控制中心用户创建屏蔽安全中心登录安全的接口需求
        if ((DSysInfo::uosEditionType() == DSysInfo::UosEnterprise) || (DSysInfo::uosEditionType() == DSysInfo::UosEnterpriseC))
            return false;

        QDBusInterface interface(QStringLiteral("com.deepin.defender.daemonservice"),
                                                QStringLiteral("/com/deepin/defender/daemonservice"),
                                                QStringLiteral("com.deepin.defender.daemonservice"));
        if (!interface.isValid()) {
            return false;
        }
        QDBusReply<int> level = interface.call("GetPwdLimitLevel");

        if (level != 1) {
            QDBusReply<QString> errorTips = interface.call("GetPwdError");
            DDialog dlg("", errorTips, this);
            dlg.setIcon(QIcon::fromTheme("preferences-system"));
            dlg.addButton(tr("Go to Settings"));
            dlg.addButton(tr("Cancel"), true, DDialog::ButtonWarning);
            connect(&dlg, &DDialog::buttonClicked, this, [ = ] (int idx) {
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

        return false;
    } else {
        edit->setAlert(false);
        edit->hideAlertMessage();
    }

    return true;
}
