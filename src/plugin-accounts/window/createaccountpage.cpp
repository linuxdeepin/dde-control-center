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
#include "widgets/dcclistview.h"
#include "groupitem.h"
#include "pwqualitymanager.h"
#include "securitylevelitem.h"

#include <DFontSizeManager>
#include <DDesktopServices>
#include <DDBusSender>
#include <DDialog>
#include <DSysInfo>

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
#include <QScrollArea>
#include <QDBusReply>

#include <widgets/comboxwidget.h>
#include <widgets/lineeditwidget.h>
#include <widgets/settingsgroup.h>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
DCC_USE_NAMESPACE

CreateAccountPage::CreateAccountPage(AccountsWorker *accountsWorker, QWidget *parent)
    : QDialog(parent)
    , m_newUser{nullptr}
    , m_accountWorker(accountsWorker)
    , m_nameEdit(new LineEditWidget)
    , m_fullnameEdit(new LineEditWidget)
    , m_passwdEdit(new DPasswordEdit)
    , m_repeatpasswdEdit(new DPasswordEdit)
    , m_passwdTipsEdit(new DLineEdit)
    , m_accountChooser(new ComboxWidget())
    , m_groupListView(nullptr)
    , m_groupItemModel(nullptr)
    , m_groupTip(new QLabel(tr("Group")))
    , m_securityLevelItem(new SecurityLevelItem(this))
{
    m_passwdEdit->setCopyEnabled(false);
    m_passwdEdit->setCutEnabled(false);

    m_repeatpasswdEdit->setCopyEnabled(false);
    m_repeatpasswdEdit->setCutEnabled(false);

    m_groupListView = new DCCListView(this);
    m_isServerSystem = DSysInfo::UosServer == DSysInfo::uosType();
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    setLayout(mainContentLayout);

    m_tw = new QWidget(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(m_tw);
    contentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    mainContentLayout->addWidget(m_tw);

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

    connect(cancleBtn, &QPushButton::clicked, this, &CreateAccountPage::reject);
    connect(addBtn, &DSuggestButton::clicked, this, &CreateAccountPage::createUser);
    resize(460, -1);
}

CreateAccountPage::~CreateAccountPage()
{
    m_repeatpasswdEdit->hideAlertMessage();
}

void CreateAccountPage::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e);
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
    layout->addSpacing(10);
    layout->addWidget(m_groupListView);

    if (m_accountChooser->comboBox()->currentIndex() != 2) {
        m_groupTip->setVisible(false);
        m_groupListView->setVisible(false);
    }
}

void CreateAccountPage::initWidgets(QVBoxLayout *layout)
{
    TitleLabel *titleLabel = new TitleLabel(tr("New Account"));
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->addWidget(titleLabel);

    /* 用户类型 */
    m_accountChooser->setTitle(tr("Account Type"));
    m_accountChooser->addBackground();

    layout->addWidget(m_accountChooser);
    layout->addSpacing(7);

    SettingsGroup *nameGroup = new SettingsGroup(nullptr,SettingsGroup::GroupBackground);
    m_nameEdit->setTitle(tr("Username") + ':');
    m_nameEdit->setAccessibleName("username_edit");
    nameGroup->insertWidget(m_nameEdit);

    m_fullnameEdit->setTitle(tr("Full Name") + ':');
    m_fullnameEdit->setAccessibleName("fullname_edit");
    nameGroup->insertWidget(m_fullnameEdit);
    layout->addWidget(nameGroup);
    layout->addSpacing(7);

    layout->addWidget(m_securityLevelItem,0,Qt::AlignRight|Qt::AlignVCenter);

    SettingsGroup *passwdGroup = new SettingsGroup(nullptr,SettingsGroup::GroupBackground);
    QLabel *passwdLabel = new QLabel(tr("Password") + ':');
    passwdLabel->setFixedWidth(110);
    m_passwdEdit->setAccessibleName("passwd_edit");

    QHBoxLayout *passwdLayout = new QHBoxLayout;
    passwdLayout->setContentsMargins(10, 0, 10, 0);
    passwdLayout->addWidget(passwdLabel, 0, Qt::AlignVCenter);
    passwdLayout->addWidget(m_passwdEdit, 0, Qt::AlignVCenter);
    SettingsItem *passwdItem = new SettingsItem();
    passwdItem->setLayout(passwdLayout);
    passwdGroup->appendItem(passwdItem);

    QLabel *repeatpasswdLabel = new QLabel(tr("Repeat Password") + ':');
    repeatpasswdLabel->setFixedWidth(110);
    m_repeatpasswdEdit->setAccessibleName("repeatpasswd_edit");

    QHBoxLayout *repeatpasswdLayout = new QHBoxLayout;
    repeatpasswdLayout->setContentsMargins(10, 0, 10, 0);
    repeatpasswdLayout->addWidget(repeatpasswdLabel, 0, Qt::AlignVCenter);
    repeatpasswdLayout->addWidget(m_repeatpasswdEdit, 0, Qt::AlignVCenter);
    SettingsItem *repeatpasswdItem = new SettingsItem();
    repeatpasswdItem->setLayout(repeatpasswdLayout);
    passwdGroup->appendItem(repeatpasswdItem);

    QLabel *passwdTipsLabel = new QLabel(tr("Password Hint") + ':');
    passwdTipsLabel->setFixedWidth(110);
    m_passwdTipsEdit->setAccessibleName("password_hint");

    QHBoxLayout *passwdTipsLayout = new QHBoxLayout;
    passwdTipsLayout->setContentsMargins(10, 0, 10, 0);
    passwdTipsLayout->addWidget(passwdTipsLabel, 0, Qt::AlignVCenter);
    passwdTipsLayout->addWidget(m_passwdTipsEdit, 0, Qt::AlignVCenter);
    SettingsItem *passwdTipsItem = new SettingsItem();
    passwdTipsItem->setLayout(passwdTipsLayout);
    passwdGroup->appendItem(passwdTipsItem);

    layout->addWidget(passwdGroup);
    layout->addSpacing(27);

    connect(m_nameEdit->dTextEdit(), &DLineEdit::textEdited, this, [ = ](const QString &strText) {
        Q_UNUSED(strText);
        if (m_nameEdit->dTextEdit()->isAlert()) {
            m_nameEdit->hideAlertMessage();
            m_nameEdit->dTextEdit()->setAlert(false);
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
            }
        }

        m_nameEdit->dTextEdit()->lineEdit()->blockSignals(true);
        int cursorIndex = m_nameEdit->dTextEdit()->lineEdit()->cursorPosition();
        m_nameEdit->dTextEdit()->lineEdit()->setText(strTemp);
        m_nameEdit->dTextEdit()->lineEdit()->setCursorPosition(cursorIndex);
        m_nameEdit->dTextEdit()->lineEdit()->blockSignals(false);
    });

    connect(m_nameEdit->dTextEdit(), &DLineEdit::editingFinished, this, &CreateAccountPage::checkName);

    connect(m_fullnameEdit->dTextEdit(), &DLineEdit::textEdited, this, [ = ](const QString &userFullName) {
        /* 90401:在键盘输入下禁止冒号的输入，粘贴情况下自动识别冒号自动删除 */
        QString fullName = userFullName;
        fullName.remove(":");
        if (fullName != userFullName) {
            m_fullnameEdit->setText(fullName);
        }
        /* 在输入的过程中仅检查全名的长度，输入完成后检查其它规则 */
        if (fullName.size() > 32) {
            m_fullnameEdit->dTextEdit()->lineEdit()->backspace();
            m_fullnameEdit->dTextEdit()->setAlert(true);
            m_fullnameEdit->dTextEdit()->showAlertMessage(tr("The full name is too long"), m_fullnameEdit, 2000);
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (m_fullnameEdit->dTextEdit()->isAlert()) {
            m_fullnameEdit->dTextEdit()->setAlert(false);
            m_fullnameEdit->hideAlertMessage();
        }
    });

    connect(m_fullnameEdit->dTextEdit(), &DLineEdit::editingFinished, this, &CreateAccountPage::checkFullname);

    //失焦后就提示
    connect(m_passwdEdit, &DLineEdit::editingFinished, this, [ = ] {
        PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_nameEdit->dTextEdit()->lineEdit()->text(),
                                                                                          m_passwdEdit->lineEdit()->text());
        if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
            m_passwdEdit->setAlert(true);
            m_passwdEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error), m_passwdEdit, 2000);
        }
    });

    //失焦后就提示，只检查密码一致性
    connect(m_repeatpasswdEdit, &DLineEdit::editingFinished, this, [ = ] {
        if (m_passwdEdit->lineEdit()->text() != m_repeatpasswdEdit->lineEdit()->text()) {
            m_repeatpasswdEdit->setAlert(true);
            m_repeatpasswdEdit->showAlertMessage(tr("Passwords do not match"), m_repeatpasswdEdit, 2000);
        }
    });


    connect(m_passwdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_passwdEdit->isAlert()) {
            m_passwdEdit->hideAlertMessage();
            m_passwdEdit->setAlert(false);
        }
    });

    connect(m_passwdEdit, &DPasswordEdit::textChanged, this, [ = ] {
        if (m_passwdEdit->text().isEmpty()) {
            m_securityLevelItem->setLevel(SecurityLevelItem::NoneLevel);
            m_passwdEdit->setAlert(false);
            m_passwdEdit->hideAlertMessage();
            return ;
        }
        PASSWORD_LEVEL_TYPE m_level = PwqualityManager::instance()->GetNewPassWdLevel(m_passwdEdit->text());
        PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_passwdEdit->lineEdit()->text(),
                                                                                          m_passwdEdit->lineEdit()->text());

        if (m_level == PASSWORD_STRENGTH_LEVEL_HIGH) {
            m_securityLevelItem->setLevel(SecurityLevelItem::HighLevel);
        } else if (m_level == PASSWORD_STRENGTH_LEVEL_MIDDLE) {
            m_securityLevelItem->setLevel(SecurityLevelItem::MidLevel);
        } else if (m_level == PASSWORD_STRENGTH_LEVEL_LOW) {
            m_securityLevelItem->setLevel(SecurityLevelItem::LowLevel);
        }
    });

    connect(m_repeatpasswdEdit, &DPasswordEdit::textEdited, this, [ = ] {
        if (m_repeatpasswdEdit->isAlert()) {
            m_repeatpasswdEdit->hideAlertMessage();
            m_repeatpasswdEdit->setAlert(false);
        }
    });

    connect(m_passwdTipsEdit, &DLineEdit::textEdited, this, [=](const QString &passwdTips) {
        if (passwdTips.size() > 14) {
            m_passwdTipsEdit->lineEdit()->backspace();
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (m_passwdTipsEdit->isAlert()) {
            m_passwdTipsEdit->setAlert(false);
        }
    });

    connect(m_accountChooser->comboBox(), &DComboBox::currentTextChanged, this, &CreateAccountPage::showGroupList);

    m_accountChooser->comboBox()->addItem(tr("Standard User"));
    m_accountChooser->comboBox()->addItem(tr("Administrator"));
    /* 仅在服务器模式下创建用户才能自定义用户组 */
    if (m_isServerSystem) {
        m_accountChooser->comboBox()->addItem(tr("Customized"));
    }

    m_nameEdit->dTextEdit()->lineEdit()->setPlaceholderText(tr("Required"));//必填
    m_fullnameEdit->dTextEdit()->lineEdit()->setPlaceholderText(tr("optional"));//选填
    m_passwdEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填
    m_repeatpasswdEdit->lineEdit()->setPlaceholderText(tr("Required"));//必填
    m_passwdTipsEdit->lineEdit()->setPlaceholderText(tr("optional"));//选填
}

void CreateAccountPage::showGroupList(const QString &index)
{
    Q_UNUSED(index)

    if (m_accountChooser->comboBox()->currentIndex() == 2) {
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
    m_accountChooser->setCurrentIndex(user->userType());
}

//在修改密码页面设置默认焦点
void CreateAccountPage::showEvent(QShowEvent *event)
{
    if (m_accountChooser && m_accountChooser->isVisible() && m_accountChooser->isEnabled())
        m_accountChooser->setFocus();
    else if (m_nameEdit && !m_nameEdit->hasFocus()) {
        m_nameEdit->dTextEdit()->lineEdit()->setFocus();
    }
    QWidget::showEvent(event);
}

void CreateAccountPage::createUser()
{
    bool check = false;
    //校验输入的用户名和密码
    if (!checkName()) {
        check = true;
    }

    if (!checkFullname()) {
        check = true;
    }

    bool needShowSafetyPage = false;
    if (!checkPassword(m_repeatpasswdEdit, needShowSafetyPage)) {
        check = true;
    }

    if (!checkPassword(m_passwdEdit, needShowSafetyPage)) {
        check = true;
    }

    if (check) {
        if (needShowSafetyPage) {
            QDBusInterface interface(QStringLiteral("com.deepin.defender.daemonservice"),
                                         QStringLiteral("/com/deepin/defender/daemonservice"),
                                         QStringLiteral("com.deepin.defender.daemonservice"));
            if (!interface.isValid()) {
                return;
            }

            QDBusReply<int> level = interface.call("GetPwdLimitLevel");
            if (level != PwdLimitLowestLevel) {
                QDBusReply<QString> errorTips = interface.call("GetPwdError");
                DDialog dlg("", errorTips, this);
                dlg.setIcon(QIcon::fromTheme("preferences-system"));
                dlg.addButton(tr("Go to Settings"));
                dlg.addButton(tr("Cancel"), true, DDialog::ButtonWarning);
                connect(&dlg, &DDialog::buttonClicked, this, [ = ](int idx) {
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
        }
        return;
    }

    for (auto c : m_passwdEdit->text()) {
        if (m_passwdTipsEdit->text().contains(c)) {
            m_passwdTipsEdit->setAlert(true);
            m_passwdTipsEdit->showAlertMessage(tr("The hint is visible to all users. Do not include the password here."), m_passwdTipsEdit, 2000);
            return;
        }
    }

    //如果用户没有选图像
    auto avatarPaht = AvatarListWidget(m_newUser).getAvatarPath();
    m_newUser->setCurrentAvatar(avatarPaht);
    m_newUser->setName(m_nameEdit->dTextEdit()->lineEdit()->text().simplified());
    m_newUser->setFullname(m_fullnameEdit->dTextEdit()->lineEdit()->text());
    m_newUser->setPassword(m_passwdEdit->lineEdit()->text());
    m_newUser->setRepeatPassword(m_repeatpasswdEdit->lineEdit()->text());
    m_newUser->setPasswordHint(m_passwdTipsEdit->lineEdit()->text());

    /* 设置用户组 */
    if (m_accountChooser->comboBox()->currentIndex() == 1) {
        m_newUser->setUserType(User::UserType::Administrator);
    } else if (m_accountChooser->comboBox()->currentIndex() == 0) {
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
    setEnabled(false);
    Q_EMIT requestCreateUser(m_newUser); // 请求创建用户
}

void CreateAccountPage::setCreationResult(CreationResult *result)
{
    setEnabled(true);
    switch (result->type()) {
    case CreationResult::NoError:
        accept();
        break;
    case CreationResult::UserNameError:
        m_nameEdit->dTextEdit()->setAlert(true);
        m_nameEdit->dTextEdit()->showAlertMessage(result->message(), m_nameEdit, 2000);
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
        m_nameEdit->dTextEdit()->setAlert(true);
        if (result->message() == "Policykit authentication failed") {
            m_nameEdit->dTextEdit()->showAlertMessage(tr("Policykit authentication failed"), m_nameEdit, 2000);
        } else {
            m_nameEdit->dTextEdit()->showAlertMessage(result->message(), m_nameEdit, 2000);
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
    const QString &userName = m_nameEdit->dTextEdit()->lineEdit()->text();

    if (userName.size() < 3 || userName.size() > 32) {
        m_nameEdit->dTextEdit()->setAlert(true);
        m_nameEdit->dTextEdit()->showAlertMessage(tr("Username must be between 3 and 32 characters"), m_nameEdit, 2000);
        return false;
    }

    const QString compStr = QString("1234567890") + QString("abcdefghijklmnopqrstuvwxyz") + QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    if (!compStr.contains(userName.at(0))) {
        m_nameEdit->dTextEdit()->setAlert(true);
        m_nameEdit->dTextEdit()->showAlertMessage(tr("The first character must be a letter or number"), m_nameEdit, 2000);
        return false;
    }

    if (userName.contains(QRegExp("^\\d+$"))) {
        m_nameEdit->dTextEdit()->setAlert(true);
        m_nameEdit->dTextEdit()->showAlertMessage(tr("Your username should not only have numbers"), m_nameEdit, 2000);
        return false;
    }

    if (!m_accountWorker->isUsernameValid(userName).argumentAt(0).toBool() && NAME_ALREADY == m_accountWorker->isUsernameValid(userName).argumentAt(2).toInt()) {
        m_nameEdit->dTextEdit()->setAlert(true);
        m_nameEdit->dTextEdit()->showAlertMessage(tr("The username has been used by other user accounts"), m_nameEdit, 2000);
        return false;
    }

     QList<User *> userList = m_userModel->userList();
     for (User *user : userList) {
         if (userName == user->fullname()) {
             m_nameEdit->dTextEdit()->setAlert(true);
             m_nameEdit->dTextEdit()->showAlertMessage(tr("The username has been used by other user accounts"), m_nameEdit, 2000);
             return false;
         }
     }

    if (m_nameEdit->dTextEdit()->isAlert()) {
        m_nameEdit->dTextEdit()->setAlert(false);
        m_nameEdit->hideAlertMessage();
    }

    return true;
}

bool CreateAccountPage::checkFullname()
{
    QString userFullName = m_fullnameEdit->dTextEdit()->lineEdit()->text();

    if (userFullName.size() > 32) {
        m_fullnameEdit->dTextEdit()->setAlert(true);
        m_fullnameEdit->dTextEdit()->showAlertMessage(tr("The full name is too long"), m_fullnameEdit, 2000);
        return false;
    }

    //欧拉版会自己创建shutdown等root组账户且不会添加到userList中，导致无法重复性算法无效，先通过isUsernameValid校验这些账户再通过重复性算法校验
    //vaild == false && code ==6 是用户名已存在
    if (!m_accountWorker->isUsernameValid(userFullName).argumentAt(0).toBool() && ErrCodeSystemUsed == m_accountWorker->isUsernameValid(userFullName).argumentAt(2).toInt()) {
        m_fullnameEdit->dTextEdit()->setAlert(true);
        m_fullnameEdit->dTextEdit()->showAlertMessage(tr("The full name has been used by other user accounts"), m_fullnameEdit, 2000);
        m_fullnameEdit->dTextEdit()->lineEdit()->selectAll();
        return false;
    }

    if (!userFullName.simplified().isEmpty()) {
        QList<User *> userList = m_userModel->userList();
        /* 与已有的用户全名和用户名进行重复性校验 */
        for (User *user : userList) {
            if (userFullName == user->fullname() || userFullName == user->name()) {
                m_fullnameEdit->dTextEdit()->setAlert(true);
                m_fullnameEdit->dTextEdit()->showAlertMessage(tr("The full name has been used by other user accounts"), m_fullnameEdit, 2000);
                m_fullnameEdit->dTextEdit()->lineEdit()->selectAll();
                return false;
            }
        }
        QList<QString> groupList = m_userModel->getAllGroups();
        for (QString &group : groupList) {
            if (userFullName == group) {
                m_fullnameEdit->dTextEdit()->setAlert(true);
                m_fullnameEdit->dTextEdit()->showAlertMessage(tr("The full name has been used by other user accounts"), m_fullnameEdit, 2000);
                m_fullnameEdit->dTextEdit()->lineEdit()->selectAll();
                return false;
            }
        }
    } else {
        m_fullnameEdit->dTextEdit()->lineEdit()->clear(); // 输入全空格不保存
    }

    if (m_fullnameEdit->dTextEdit()->isAlert()) {
        m_fullnameEdit->dTextEdit()->setAlert(false);
        m_fullnameEdit->hideAlertMessage();
    }

    return true;
}

bool CreateAccountPage::checkPassword(DPasswordEdit *edit, bool &needShowSafetyPage)
{
    if (edit == m_repeatpasswdEdit) {
        if (m_passwdEdit->lineEdit()->text() != m_repeatpasswdEdit->lineEdit()->text()) {
            m_repeatpasswdEdit->setAlert(true);
            m_repeatpasswdEdit->showAlertMessage(tr("Passwords do not match"), this->parentWidget(), 2000);
            return false;
        }
    }

    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_nameEdit->dTextEdit()->lineEdit()->text(),
                                                                                      edit->lineEdit()->text());

    if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
        m_passwdEdit->setAlert(true);
        m_passwdEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error), m_passwdEdit, 2000);

        // 企业版控制中心用户创建屏蔽安全中心登录安全的接口需求
        if ((DSysInfo::uosEditionType() == DSysInfo::UosEnterprise) || (DSysInfo::uosEditionType() == DSysInfo::UosEnterpriseC))
            return false;

        needShowSafetyPage = true;

        return false;
    } else {
        edit->setAlert(false);
        edit->hideAlertMessage();
    }

    return true;
}
