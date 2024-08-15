//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "modifypasswdpage.h"
#include "widgets/titlelabel.h"
#include "pwqualitymanager.h"
#include "createaccountpage.h"
#include "securitylevelitem.h"
#include "widgets/accessibleinterface.h"

#include "deepin_pw_check.h"
#include "unionidbindreminderdialog.h"
#include <DDialog>
#include <DDBusSender>
#include <DDesktopServices>
#include <DMessageManager>
#include <DSysInfo>
#include <DTitlebar>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSettings>
#include <QDebug>
#include <QDBusReply>

#include <unistd.h>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
using namespace DCC_NAMESPACE;
SET_FORM_ACCESSIBLE(ModifyPasswdPage,"ModifyPasswdPage")
ModifyPasswdPage::ModifyPasswdPage(User *user, bool isCurrent, QWidget *parent)
    : DAbstractDialog(false, parent)
    , m_curUser(user)
    , m_oldPasswordEdit(new DPasswordEdit)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
    , m_passwordTipsEdit(new DLineEdit)
    , m_isCurrent(isCurrent)
    , m_isBindCheckError(false)
    , m_securityLevelItem(new SecurityLevelItem(this))
{
    initWidget();
    resize(460, -1);
}

ModifyPasswdPage::~ModifyPasswdPage()
{

}

void ModifyPasswdPage::initWidget()
{
    QVBoxLayout *mainContentLayout = new QVBoxLayout;

    mainContentLayout->setSpacing(4);

    auto titleBar = new DTitlebar(this);

    titleBar->setBackgroundTransparent(true);

    mainContentLayout->addWidget(titleBar);

    TitleLabel *titleLabel = new TitleLabel(tr("Change Password"));
    mainContentLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    if (!m_isCurrent) {
        titleLabel->setText(tr("Reset Password"));
        QLabel *label = new QLabel(tr("Resetting the password will clear the data stored in the keyring."));
        label->setWordWrap(true);
        mainContentLayout->addWidget(label, 0, Qt::AlignHCenter);
    }
    mainContentLayout->addSpacing(40);

    if (m_isCurrent) {
        QLabel *oldPasswdLabel = new QLabel(tr("Current Password") + ":");
        m_forgetPasswordBtn = new DCommandLinkButton(tr("Forgot password?"));
        DFontSizeManager::instance()->bind(m_forgetPasswordBtn, DFontSizeManager::T8);
        m_forgetPasswordBtn->setVisible(!(DSysInfo::UosCommunity == DSysInfo::uosEditionType()) && getuid() < 9999); // 如果当前账户是域账号,则屏蔽重置密码入口
        connect(m_forgetPasswordBtn, &QPushButton::clicked, this, &ModifyPasswdPage::onForgetPasswordBtnClicked);
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->addWidget(oldPasswdLabel);
        hLayout->addStretch();
        hLayout->addWidget(m_forgetPasswordBtn);
        hLayout->addSpacing(45);
        mainContentLayout->addLayout(hLayout);
        mainContentLayout->addWidget(m_oldPasswordEdit);
    }

    QHBoxLayout *newPasswdLayout = new QHBoxLayout;
    QLabel *newPasswdLabel = new QLabel(tr("New Password") + ":");
    newPasswdLayout->addWidget(newPasswdLabel);
    newPasswdLayout->addSpacing(80);

    newPasswdLayout->addWidget(m_securityLevelItem);
    mainContentLayout->addSpacing(6);
    mainContentLayout->addLayout(newPasswdLayout);
    mainContentLayout->addWidget(m_newPasswordEdit);

    QLabel *repeatPasswdLabel = new QLabel(tr("Repeat Password") + ":");
    mainContentLayout->addSpacing(6);
    mainContentLayout->addWidget(repeatPasswdLabel);
    mainContentLayout->addWidget(m_repeatPasswordEdit);

    QLabel *passwdTipsLabel = new QLabel(tr("Password Hint") + ":");
    mainContentLayout->addSpacing(6);
    mainContentLayout->addWidget(passwdTipsLabel);
    mainContentLayout->addWidget(m_passwordTipsEdit);
    mainContentLayout->addStretch();

    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *saveBtn = new DSuggestButton(tr("Save"));
    QHBoxLayout *cansaveLayout = new QHBoxLayout;
    cansaveLayout->addWidget(cancleBtn);
    cansaveLayout->addWidget(saveBtn);
    mainContentLayout->addLayout(cansaveLayout);
    setLayout(mainContentLayout);
    cancleBtn->setDefault(true);
    saveBtn->setDefault(true);
    cancleBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    saveBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    setPasswordEditAttribute(m_oldPasswordEdit);
    setPasswordEditAttribute(m_newPasswordEdit);
    setPasswordEditAttribute(m_repeatPasswordEdit);

    connect(cancleBtn, &QPushButton::clicked, this, &ModifyPasswdPage::reject);

    connect(saveBtn, &DSuggestButton::clicked, this, &ModifyPasswdPage::clickSaveBtn);

    connect(m_curUser, &User::passwordModifyFinished, this, &ModifyPasswdPage::onPasswordChangeFinished);

    connect(m_curUser, &User::passwordStatusChanged, this, [ = ](const QString & status) {
        m_oldPasswordEdit->setVisible(status != NO_PASSWORD);
    });
    connect(m_curUser, &User::passwordResetFinished, this, &ModifyPasswdPage::resetPasswordFinished);
    connect(m_curUser, &User::startResetPasswordReplied, this, &ModifyPasswdPage::onStartResetPasswordReplied);
    connect(m_curUser, &User::startSecurityQuestionsCheckReplied, this, &ModifyPasswdPage::onSecurityQuestionsCheckReplied);
    connect(m_oldPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_oldPasswordEdit->isAlert())
        {
            m_oldPasswordEdit->hideAlertMessage();
            m_oldPasswordEdit->setAlert(false);
        }
    });
    connect(m_repeatPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (m_repeatPasswordEdit->isAlert())
        {
            m_repeatPasswordEdit->hideAlertMessage();
            m_repeatPasswordEdit->setAlert(false);
        }
    });
    connect(m_passwordTipsEdit, &DLineEdit::textEdited, this, [ = ](const QString & passwdTips) {
        if (passwdTips.size() > 14) {
            m_passwordTipsEdit->lineEdit()->backspace();
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (m_passwordTipsEdit->isAlert()) {
            m_passwordTipsEdit->setAlert(false);
        }
    });

    m_securityLevelItem->setUser(m_curUser->name());
    m_securityLevelItem->bind(m_newPasswordEdit);

    connect(m_repeatPasswordEdit, &DPasswordEdit::editingFinished, this, [ = ]() {
        if (m_newPasswordEdit->lineEdit()->text() != m_repeatPasswordEdit->lineEdit()->text()) {
            m_repeatPasswordEdit->setAlert(true);
            m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
        }
    });

    m_oldPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_oldPasswordEdit->setAccessibleName("oldpasswordedit");
    m_newPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_newPasswordEdit->setAccessibleName("newpasswordedit");
    m_repeatPasswordEdit->lineEdit()->setPlaceholderText(tr("Required"));
    m_repeatPasswordEdit->setAccessibleName("repeatpasswordedit");
    m_passwordTipsEdit->lineEdit()->setPlaceholderText(tr("Optional"));
    m_passwordTipsEdit->setAccessibleName("passwordtipsedit");

    cancleBtn->setMinimumSize(165, 36);
    saveBtn->setMinimumSize(165, 36);
    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T5);

    setFocusPolicy(Qt::StrongFocus);
}

bool ModifyPasswdPage::judgeTextEmpty(DPasswordEdit *edit)
{
    if (edit->text().isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Password cannot be empty"), edit, 2000);
    }

    return edit->text().isEmpty();
}

void ModifyPasswdPage::clickSaveBtn()
{
    //校验输入密码
    if ((judgeTextEmpty(m_oldPasswordEdit) && m_isCurrent) || judgeTextEmpty(m_newPasswordEdit) || judgeTextEmpty(m_repeatPasswordEdit))
        return;

    if (m_isCurrent) {
        for (auto c : m_newPasswordEdit->text()) {
            if (m_passwordTipsEdit->text().contains(c)) {
                m_passwordTipsEdit->setAlert(true);
                m_passwordTipsEdit->showAlertMessage(tr("The hint is visible to all users. Do not include the password here."), m_passwordTipsEdit, 2000);
                return;
            }
        }

        Q_EMIT requestChangePassword(m_curUser, m_oldPasswordEdit->lineEdit()->text(), m_newPasswordEdit->lineEdit()->text(), m_repeatPasswordEdit->lineEdit()->text());
    } else {
        resetPassword(m_newPasswordEdit->text(), m_repeatPasswordEdit->text());
    }
}

void ModifyPasswdPage::onPasswordChangeFinished(const int exitCode, const QString &errorTxt)
{
    Q_UNUSED(exitCode)
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_curUser->name(),
                                                                                      m_newPasswordEdit->lineEdit()->text());
    qDebug() << "exit code:" << exitCode << "error text:" << errorTxt << "error type:" << error
            << "error tips:" << PwqualityManager::instance()->getErrorTips(error);
    if (exitCode != 0) {
        if (errorTxt.startsWith("Current Password: passwd:", Qt::CaseInsensitive)) {
            m_oldPasswordEdit->setAlert(true);
            m_oldPasswordEdit->showAlertMessage(tr("Wrong password"));
            return;
        }

        if (m_newPasswordEdit->lineEdit()->text() == m_oldPasswordEdit->lineEdit()->text()) {
            m_newPasswordEdit->setAlert(true);
            m_newPasswordEdit->showAlertMessage(tr("New password should differ from the current one"), m_oldPasswordEdit, 2000);
            return;
        }

        if (error == PW_NO_ERR) {
            if (m_newPasswordEdit->lineEdit()->text() != m_repeatPasswordEdit->lineEdit()->text()) {
                m_repeatPasswordEdit->setAlert(true);
                m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
                return;
            }
        }

        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        // 企业版控制中心修改密码屏蔽安全中心登录安全的接口需求
        if ((DSysInfo::uosEditionType() == DSysInfo::UosEnterprise) || (DSysInfo::uosEditionType() == DSysInfo::UosEnterpriseC))
            return;

        Q_EMIT requestCheckPwdLimitLevel();
    } else if (error != PW_NO_ERR) {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        Q_EMIT requestChangePassword(m_curUser, m_newPasswordEdit->lineEdit()->text(), m_oldPasswordEdit->lineEdit()->text(), m_oldPasswordEdit->lineEdit()->text(), false);
    } else {
        if (!m_passwordTipsEdit->text().simplified().isEmpty())
            requestSetPasswordHint(m_curUser, m_passwordTipsEdit->text());
        close();
    }
}

void ModifyPasswdPage::setPasswordEditAttribute(DPasswordEdit *edit)
{
    edit->setAttribute(Qt::WA_InputMethodEnabled, false);
    edit->lineEdit()->setValidator(new QRegExpValidator(QRegExp("[^\\x4e00-\\x9fa5]+"), edit));
    edit->setCopyEnabled(false);
    edit->setCutEnabled(false);
}

void ModifyPasswdPage::resetPassword(const QString &password, const QString &repeatPassword)
{
    bool check = false;
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_curUser->name(), password);

    if (error != PW_NO_ERR) {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        check = true;
    }

    if (password != repeatPassword) {
        m_repeatPasswordEdit->setAlert(true);
        m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
        check = true;
    }

    for (auto c : password) {
        if (m_passwordTipsEdit->text().contains(c)) {
            m_passwordTipsEdit->setAlert(true);
            m_passwordTipsEdit->showAlertMessage(tr("The hint is visible to all users. Do not include the password here."), m_passwordTipsEdit, 2000);
            check = true;
        }
    }

    if (check) {
        // 企业版控制中心修改密码屏蔽安全中心登录安全的接口需求
        if ((DSysInfo::uosEditionType() == DSysInfo::UosEnterprise)
            || (DSysInfo::uosEditionType() == DSysInfo::UosEnterpriseC)) {
            return;
        }

        if (error != PW_NO_ERR) {
            Q_EMIT requestCheckPwdLimitLevel();
        }
        return;
    }

    if (!m_passwordTipsEdit->text().simplified().isEmpty())
        requestSetPasswordHint(m_curUser, m_passwordTipsEdit->text());

    Q_EMIT requestResetPassword(m_curUser, password);
}

//在修改密码页面当前密码处设置焦点
void ModifyPasswdPage::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    DPasswordEdit *passwordEdit = m_isCurrent ? m_oldPasswordEdit : m_newPasswordEdit;
    if (passwordEdit && !passwordEdit->hasFocus()) {
        passwordEdit->lineEdit()->setFocus();
    }
}

void ModifyPasswdPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QDialog::paintEvent(event);
}

void ModifyPasswdPage::resetPasswordFinished(const QString &errorText)
{
    if (errorText.isEmpty()) {
        close();
    } else {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(errorText, m_newPasswordEdit, 2000);
    }
}

void ModifyPasswdPage::onForgetPasswordBtnClicked()
{
    m_forgetPasswordBtn->setEnabled(false);
    Q_EMIT requestSecurityQuestionsCheck(m_curUser);
}

void ModifyPasswdPage::onStartResetPasswordReplied(const QString &errorText)
{
    if (!errorText.isEmpty()) {
        m_forgetPasswordBtn->setEnabled(true);
    } else {
        m_enableBtnTimer.singleShot(5000, this, [this]{ m_forgetPasswordBtn->setEnabled(true); });
    }
    qDebug() << "Resetpassword reply:" << errorText;
}

void ModifyPasswdPage::onSecurityQuestionsCheckReplied(const QList<int> &questions)
{
    if (!questions.isEmpty()) {
        Q_EMIT requestStartResetPasswordExec(m_curUser);
    } else {
        QString uosid;
        Q_EMIT requestUOSID(uosid);
        if (uosid.isEmpty()) {
            return;
        }

        QString uuid;
        Q_EMIT requestUUID(uuid);
        if (uuid.isEmpty()) {
            return;
        }
        Q_EMIT requestLocalBindCheck(m_curUser, uosid, uuid);
    }
    qDebug() << "IsSecurityQuestionsExist:" << !questions.isEmpty();
}

void ModifyPasswdPage::onLocalBindCheckUbid(const QString &ubid)
{
    if (!ubid.isEmpty()) {
        m_isBindCheckError = false;
        Q_EMIT requestStartResetPasswordExec(m_curUser);
    } else if (!m_isBindCheckError) {
        UnionIDBindReminderDialog dlg;
        dlg.exec();
        m_forgetPasswordBtn->setEnabled(true);
    }
}

void ModifyPasswdPage::onLocalBindCheckError(const QString &error)
{
    m_isBindCheckError = true;
    m_forgetPasswordBtn->setEnabled(true);
    QString tips;
    if (error.contains("7500")) {
        tips = tr("System error");
    } else if (error.contains("7506")) {
        tips = tr("Network error");
    }
    if (!tips.isEmpty()) {
        DMessageManager::instance()->sendMessage(this,
                                                 style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                 tips);
    }
}
