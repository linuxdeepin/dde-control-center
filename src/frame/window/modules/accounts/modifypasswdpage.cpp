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

#include "modifypasswdpage.h"
#include "widgets/titlelabel.h"
#include "modules/accounts/usermodel.h"

#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSettings>
#include <QDebug>

using namespace dcc::accounts;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::accounts;

ModifyPasswdPage::ModifyPasswdPage(User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_oldPasswordEdit(new DPasswordEdit)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
{
    initWidget();
}

ModifyPasswdPage::~ModifyPasswdPage()
{
}

void ModifyPasswdPage::initWidget()
{
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->addSpacing(40);

    TitleLabel *titleLabel = new TitleLabel(tr("Change Password"));
    mainContentLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    mainContentLayout->addSpacing(40);

    QRegExp reg("^((?![\u4e00-\u9fa5]).)*");
    QValidator*validator = new QRegExpValidator(reg);
    m_oldPasswordEdit->lineEdit()->setAttribute(Qt::WA_InputMethodEnabled, false);
    m_oldPasswordEdit->lineEdit()->setValidator(validator);
    m_newPasswordEdit->lineEdit()->setAttribute(Qt::WA_InputMethodEnabled, false);
    m_newPasswordEdit->lineEdit()->setValidator(validator);
    m_repeatPasswordEdit->lineEdit()->setAttribute(Qt::WA_InputMethodEnabled, false);
    m_repeatPasswordEdit->lineEdit()->setValidator(validator);

    QLabel *oldPasswdLabel = new QLabel(tr("Current Password") + ":");
    mainContentLayout->addWidget(oldPasswdLabel);
    mainContentLayout->addWidget(m_oldPasswordEdit);

    QLabel *newPasswdLabel = new QLabel(tr("New Password") + ":");
    mainContentLayout->addWidget(newPasswdLabel);
    mainContentLayout->addWidget(m_newPasswordEdit);

    QLabel *repeatPasswdLabel = new QLabel(tr("Repeat Password") + ":");
    mainContentLayout->addWidget(repeatPasswdLabel);
    mainContentLayout->addWidget(m_repeatPasswordEdit);
    mainContentLayout->addStretch();

    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *saveBtn = new DSuggestButton(tr("Save"));
    QHBoxLayout *cansaveLayout = new QHBoxLayout;
    cansaveLayout->setSpacing(10);
    cansaveLayout->addWidget(cancleBtn);
    cansaveLayout->addWidget(saveBtn);
    mainContentLayout->addLayout(cansaveLayout);
    setLayout(mainContentLayout);
    cancleBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    saveBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(cancleBtn, &QPushButton::clicked, this, [&] {
        Q_EMIT requestBack();
    });

    connect(saveBtn, &DSuggestButton::clicked, this, &ModifyPasswdPage::clickSaveBtn);

    connect(m_curUser, &User::passwordModifyFinished, this, &ModifyPasswdPage::onPasswordChangeFinished);

    connect(m_curUser, &User::passwordStatusChanged, this, [ = ](const QString & status) {
        m_oldPasswordEdit->setVisible(status != NO_PASSWORD);
    });

    connect(m_oldPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_oldPasswordEdit->isAlert()) {
            m_oldPasswordEdit->hideAlertMessage();
            m_oldPasswordEdit->setAlert(false);
        }
    });
    connect(m_newPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_newPasswordEdit->isAlert()) {
            m_newPasswordEdit->hideAlertMessage();
            m_newPasswordEdit->setAlert(false);
        }
    });
    connect(m_repeatPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_repeatPasswordEdit->isAlert()) {
            m_repeatPasswordEdit->hideAlertMessage();
            m_repeatPasswordEdit->setAlert(false);
        }
    });

    m_oldPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_oldPasswordEdit->setAccessibleName("oldpasswordedit");
    m_newPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_newPasswordEdit->setAccessibleName("newpasswordedit");
    m_repeatPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_repeatPasswordEdit->setAccessibleName("repeatpasswordedit");

    cancleBtn->setMinimumSize(165, 36);
    saveBtn->setMinimumSize(165, 36);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T5);

    setFocusPolicy(Qt::StrongFocus);
}

void ModifyPasswdPage::clickSaveBtn()
{
    //校验输入密码
    if (m_oldPasswordEdit->lineEdit()->text().isEmpty()) {
        m_oldPasswordEdit->setAlert(true);
        return;
    }
    if (!onPasswordEditFinished(m_newPasswordEdit)) {
        return;
    }
    if (!onPasswordEditFinished(m_repeatPasswordEdit)) {
        return;
    }

    DaemonService *daemonservice = new DaemonService("com.deepin.defender.daemonservice",
                                                     "/com/deepin/defender/daemonservice",
                                                     QDBusConnection::sessionBus(), this);
    QString strPwd = m_newPasswordEdit->lineEdit()->text();
    if (strPwd.length() >= daemonservice->GetPwdLen() && m_curUser->charactertypes(strPwd) >= daemonservice->GetPwdTypeLen()) {
        Q_EMIT requestChangePassword(m_curUser, m_oldPasswordEdit->lineEdit()->text(), m_newPasswordEdit->lineEdit()->text());
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

void ModifyPasswdPage::onPasswordChangeFinished(const int exitCode)
{
    if (exitCode == ModifyPasswdPage::ModifyNewPwdSuccess) {
        DaemonService *daemonservice = new DaemonService("com.deepin.defender.daemonservice",
                                                         "/com/deepin/defender/daemonservice",
                                                         QDBusConnection::sessionBus(), this);
        daemonservice->PasswordUpdate();

        Q_EMIT requestBack(AccountsWidget::ModifyPwdSuccess);
        return;
    } if (exitCode == ModifyPasswdPage::InputOldPwdError) {
        m_oldPasswordEdit->setAlert(true);
        m_oldPasswordEdit->showAlertMessage(tr("Wrong password"), -1);
        return;
    } else {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(tr("Failed to change the password"), -1);
        qDebug() << Q_FUNC_INFO << "exit =" << exitCode;
    }
}

QString ModifyPasswdPage::validatePassword(const QString &password)
{
    // NOTE(justforlxz): 配置文件由安装器生成，后续改成PAM模块
    QSettings setting("/etc/deepin/dde.conf", QSettings::IniFormat);
    setting.beginGroup("Password");
    const bool strongPasswordCheck = setting.value("STRONG_PASSWORD", false).toBool();
    const int passwordMinLength = setting.value("PASSWORD_MIN_LENGTH").toInt();
    const int passwordMaxLength = setting.value("PASSWORD_MAX_LENGTH").toInt();
    const QStringList validatePolicy = setting.value("VALIDATE_POLICY").toString().split(";");
    //const int validateRequired = setting.value("VALIDATE_REQUIRED").toInt();
    QString reversusername;
    QStringList reversenamelist;

    for (int i = m_curUser->name().count() - 1; i > -1; i--) {
        reversenamelist << m_curUser->name().at(i);
    }
    reversusername = reversenamelist.join("");

    if (!strongPasswordCheck) {
        return "";
    }

    if (password.size() < passwordMinLength || password.size() > passwordMaxLength) {
        return QString(tr("Password must be between %1 and %2 characters")).arg(passwordMinLength).arg(passwordMaxLength);
    }
    // NOTE(justforlxz): 转换为set，如果密码中包含了不存在与validatePolicy中的字符，相减以后不为空。"[0-9]"[^\w\s]+
    int password_character_types = 0;
    if (password.contains(QRegExp("[0-9]")))
        password_character_types++;
    if (password.contains(QRegExp("[A-Z]")))
        password_character_types++;
    if (password.contains(QRegExp("[a-z]")))
        password_character_types++;
    if (password.contains(QRegExp("((?=[\x21-\x7e]+)[^A-Za-z0-9])")) || password.contains(" "))
        password_character_types++;
    if (password_character_types < 2)
        return QString(tr("The password must have at least 6 characters, and contain at least 2 of the four available character types: lowercase letters, uppercase letters, numbers, and symbols"));
    if (password == m_curUser->name() || password == reversusername) {
        return QString(tr("Password should not be the repeated or reversed username"));
    }
    return "";
}

bool ModifyPasswdPage::onPasswordEditFinished(Dtk::Widget::DPasswordEdit *edit)
{
    const QString &password = edit->lineEdit()->text();
    if (password.isEmpty()) {
        edit->setAlert(true);
        return false;
    }

    const int maxSize = 512;
    if (password.size() > maxSize) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password must be no more than %1 characters").arg(maxSize), -1);
        return false;
    }

    auto res = validatePassword(password);
    if (!res.isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(res);
        return false;
    }

    //重复密码
    if (edit == m_repeatPasswordEdit) {
        if (m_newPasswordEdit->lineEdit()->text() != password) {
            edit->setAlert(true);
            edit->showAlertMessage(tr("Passwords do not match"), -1);
            return false;
        }
    }



    if (m_oldPasswordEdit->lineEdit()->text() == password) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("New password should differ from the current one"), -1);
        return false;
    }



    return true;
}

void ModifyPasswdPage::showEvent(QShowEvent *event) {
    Q_UNUSED(event);
    m_oldPasswordEdit->lineEdit()->setFocus();
}
