// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "passwordwidget.h"
#include "pwqualitymanager.h"
#include "securitylevelitembinder.h"

#include <DDesktopServices>
#include <DMessageManager>

DGUI_USE_NAMESPACE

PasswordWidget::PasswordWidget(int margin, const QString &userName, QWidget *parent)
    : QWidget(parent)
    , m_userName(userName)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
    , m_passwordTipsEdit(new DLineEdit)
    , m_newPasswdLevelText(new QLabel)
{
    initWidget(margin);
    initData();

    qApp->setProperty("editing_username", m_userName);
}

bool PasswordWidget::checkPassword()
{
    // 密码强度
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_userName,
                                                                                      m_newPasswordEdit->text());

    if (error != PW_NO_ERR) {
        m_newPasswordEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", false);
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        return false;
    }

    // 新密码和重复密码是否一致
    if (m_newPasswordEdit->text() != m_repeatPasswordEdit->text()) {
        m_repeatPasswordEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", false);
        m_repeatPasswordEdit->setAlert(true);
        m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
        return false;
    }

    // 密码提示
    for (auto c : m_newPasswordEdit->text()) {
        if (m_passwordTipsEdit->text().contains(c)) {
            m_passwordTipsEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", false);
            m_passwordTipsEdit->setAlert(true);
            m_passwordTipsEdit->showAlertMessage(tr("The hint is visible to all users. Do not include the password here."), m_passwordTipsEdit, 2000);
            return false;
        }
    }
    if (!m_passwordTipsEdit->text().simplified().isEmpty()) {
        Q_EMIT requestSetPasswordHint(m_passwordTipsEdit->text());
    }
    return true;
}

void PasswordWidget::initWidget(int margin)
{
    this->setAccessibleName("PasswordWidget");

    QVBoxLayout *resetPasswordVLayout = new QVBoxLayout(this);
    resetPasswordVLayout->setSpacing(0);
    resetPasswordVLayout->setMargin(0);

    QLabel *newPasswordLabel = new QLabel(tr("New Password") + ":");
    QHBoxLayout *newPasswordLayout = new QHBoxLayout;
    newPasswordLayout->addWidget(newPasswordLabel);
    newPasswordLayout->addSpacing(80);

    QHBoxLayout *newPasswdLevelLayout = new QHBoxLayout;
    m_newPasswdLevelText->setFixedWidth(55);
    m_newPasswdLevelText->setFixedHeight(20);
    newPasswdLevelLayout->addWidget(m_newPasswdLevelText);
    newPasswdLevelLayout->setSpacing(4);
    SecurityLevelItem *securityLevelItem = new SecurityLevelItem(this);
    newPasswdLevelLayout->addWidget(securityLevelItem);
    DCC_NAMESPACE::accounts::SecurityLevelItemBinder::bind(securityLevelItem, m_newPasswordEdit);

    newPasswordLayout->addLayout(newPasswdLevelLayout);
    resetPasswordVLayout->addLayout(newPasswordLayout);
    resetPasswordVLayout->addSpacing(4);
    resetPasswordVLayout->addWidget(m_newPasswordEdit);
    resetPasswordVLayout->addSpacing(14);
    m_newPasswordEdit->setCopyEnabled(false);
    m_newPasswordEdit->setCutEnabled(false);

    QLabel *repeatPasswordLabel = new QLabel(tr("Repeat Password") + ":");
    resetPasswordVLayout->addWidget(repeatPasswordLabel);
    resetPasswordVLayout->addSpacing(4);
    resetPasswordVLayout->addWidget(m_repeatPasswordEdit);
    resetPasswordVLayout->addSpacing(14);
    m_repeatPasswordEdit->setCopyEnabled(false);
    m_repeatPasswordEdit->setCutEnabled(false);

    QLabel *passwdTipsLabel = new QLabel(tr("Password Hint") + ":");
    resetPasswordVLayout->addWidget(passwdTipsLabel);
    resetPasswordVLayout->addSpacing(4);
    resetPasswordVLayout->addWidget(m_passwordTipsEdit);
    resetPasswordVLayout->addSpacing(margin);
}

void PasswordWidget::initData()
{
    m_newPasswordEdit->setPlaceholderText(tr("Required"));
    m_repeatPasswordEdit->setPlaceholderText(tr("Required"));
    m_passwordTipsEdit->setPlaceholderText(tr("Optional"));
    m_newPasswordEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", true);
    m_repeatPasswordEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", true);
    m_passwordTipsEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", true);

    connect(m_newPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] {
        if (!m_newPasswordEdit->isAlert()) {
            m_newPasswordEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", true);
        }
    });
    connect(m_repeatPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] { hideAlert(m_repeatPasswordEdit); });
    connect(m_repeatPasswordEdit, &DPasswordEdit::editingFinished, this, [ & ]() {
        if (m_newPasswordEdit->lineEdit()->text() != m_repeatPasswordEdit->lineEdit()->text()) {
            m_repeatPasswordEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", false);
            m_repeatPasswordEdit->setAlert(true);
            m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
        }
    });
    connect(m_passwordTipsEdit, &DLineEdit::textEdited, this, [=](const QString &passwdTips) {
        if (passwdTips.size() > 14) {
            m_passwordTipsEdit->lineEdit()->backspace();
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (m_passwordTipsEdit->isAlert()) {
            hideAlert(m_passwordTipsEdit);
        }
    });
}

bool PasswordWidget::isPasswordEmpty()
{
    return isContentEmpty(m_newPasswordEdit) || isContentEmpty(m_repeatPasswordEdit);
}

void PasswordWidget::setEditNormal()
{
    if (m_newPasswordEdit) {
        m_newPasswordEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", true);
    }
    if (m_repeatPasswordEdit) {
        m_repeatPasswordEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", true);
    }
    if (m_passwordTipsEdit) {
        m_passwordTipsEdit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", true);
    }
}

void PasswordWidget::hideAlert(DLineEdit *edit)
{
    if (edit->isAlert()) {
        edit->hideAlertMessage();
        edit->lineEdit()->setProperty("_d_dtk_lineedit_opacity", true);
        edit->setAlert(false);
    }
}
