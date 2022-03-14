/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     wangfeia <wangfeia@uniontech.com>
 *
 * Maintainer: wangfeia <wangfeia@uniontech.com>
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

#include "passwordwidget.h"
#include "pwqualitymanager.h"

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
}

bool PasswordWidget::checkPassword()
{
    // 密码强度
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_userName,
                                                                                      m_newPasswordEdit->text());

    if (error != PW_NO_ERR) {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        return false;
    }

    // 新密码和重复密码是否一致
    if (m_newPasswordEdit->text() != m_repeatPasswordEdit->text()) {
        m_repeatPasswordEdit->setAlert(true);
        m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
        return false;
    }

    // 密码提示
    for (auto c : m_newPasswordEdit->text()) {
        if (m_passwordTipsEdit->text().contains(c)) {
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
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    setPasswdLevelIconModePath(type);

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

    for (int i = 0; i < PASSWORD_LEVEL_ICON_NUM; i++) {
        m_newPasswdLevelIcons[i] = new QLabel;
    }
    QImage img;
    img.load(m_newPasswdLevelIconModePath);
    m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(img));
    newPasswdLevelLayout->addWidget(m_newPasswdLevelIcons[0]);
    newPasswdLevelLayout->addSpacing(4);
    m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(img));
    newPasswdLevelLayout->addWidget(m_newPasswdLevelIcons[1]);
    newPasswdLevelLayout->addSpacing(4);
    m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));
    newPasswdLevelLayout->addWidget(m_newPasswdLevelIcons[2]);
    newPasswdLevelLayout->addSpacing(50);

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

    connect(m_newPasswordEdit, &DLineEdit::textChanged, this, &PasswordWidget::onNewPasswordLineEditChanged);
    connect(m_repeatPasswordEdit, &DPasswordEdit::textEdited, this, [ & ] { hideAlert(m_repeatPasswordEdit); });
    connect(m_repeatPasswordEdit, &DPasswordEdit::editingFinished, this, [ & ]() {
        if (m_newPasswordEdit->lineEdit()->text() != m_repeatPasswordEdit->lineEdit()->text()) {
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
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [=](DGuiApplicationHelper::ColorType themeType){
        setPasswdLevelIconModePath(themeType);
        QImage img;
        if (m_newPasswordEdit->text().isEmpty()) {
            img.load(m_newPasswdLevelIconModePath);
            m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(img));
            m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(img));
            m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));
            m_newPasswdLevelText->setText("");
            return ;
        }
        if (m_level == PASSWORD_STRENGTH_LEVEL_MIDDLE) {
            img.load(m_newPasswdLevelIconModePath);
            m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));
        } else if (m_level == PASSWORD_STRENGTH_LEVEL_LOW) {
            img.load(m_newPasswdLevelIconModePath);
            m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(img));
            m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));
        }
    });
}

void PasswordWidget::onNewPasswordLineEditChanged(const QString&)
{
    if (m_newPasswordEdit->text().isEmpty()) {
        m_newPasswordEdit->setAlert(false);
        m_newPasswordEdit->hideAlertMessage();
        DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
        switch (type) {
        case DGuiApplicationHelper::UnknownType:
            break;
        case DGuiApplicationHelper::LightType:
            m_newPasswdLevelIconModePath = PASSWORD_LEVEL_ICON_LIGHT_MODE_PATH;
            break;
        case DGuiApplicationHelper::DarkType:
            m_newPasswdLevelIconModePath = PASSWORD_LEVEL_ICON_DEEP_MODE_PATH;
            break;
        }
        QImage img;
        img.load(m_newPasswdLevelIconModePath);
        m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelText->setText("");
        return ;
    }
    PASSWORD_LEVEL_TYPE level = PwqualityManager::instance()->GetNewPassWdLevel(m_newPasswordEdit->text());
    updatePasswordStrengthLevelWidget(level);
    m_level = level;
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_userName,
                                                                                      m_newPasswordEdit->text());
    if (error != PwqualityManager::ERROR_TYPE::PW_NO_ERR) {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error), m_newPasswordEdit, 2000);
    }
}

void PasswordWidget::updatePasswordStrengthLevelWidget(PASSWORD_LEVEL_TYPE level)
{
    QPalette palette;
    QImage img;
    if (level == PASSWORD_STRENGTH_LEVEL_HIGH) {
        palette.setColor(QPalette::WindowText, QColor("#15BB18"));
        m_newPasswdLevelText->setPalette(palette);
        m_newPasswdLevelText->setText(tr("Strong"));

        img.load(PASSWORD_LEVEL_ICON_HIGH_PATH);
        m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));

    } else if (level == PASSWORD_STRENGTH_LEVEL_MIDDLE) {
        palette.setColor(QPalette::WindowText, QColor("#FFAA00"));
        m_newPasswdLevelText->setPalette(palette);
        m_newPasswdLevelText->setText(tr("Medium"));

        img.load(PASSWORD_LEVEL_ICON_MIDDLE_PATH);
        m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(img));
        img.load(m_newPasswdLevelIconModePath);
        m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));

        m_newPasswordEdit->setAlert(false);
        m_newPasswordEdit->showAlertMessage(tr("A stronger password is recommended: more than 8 characters, and contains 3 of the four character types: lowercase letters, uppercase letters, numbers, and symbols."));
    } else if (level == PASSWORD_STRENGTH_LEVEL_LOW) {
        palette.setColor(QPalette::WindowText, QColor("#FF5736"));
        m_newPasswdLevelText->setPalette(palette);
        m_newPasswdLevelText->setText(tr("Weak"));

        img.load(PASSWORD_LEVEL_ICON_LOW_PATH);
        m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(img));
        img.load(m_newPasswdLevelIconModePath);
        m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));

        m_newPasswordEdit->setAlert(false);
        m_newPasswordEdit->showAlertMessage(tr("A stronger password is recommended: more than 8 characters, and contains 3 of the four character types: lowercase letters, uppercase letters, numbers, and symbols."));
    } else {
        m_newPasswordEdit->showAlertMessage(tr("Error occurred when reading the configuration files of password rules!"));
    }
}

void PasswordWidget::setPasswdLevelIconModePath(DGuiApplicationHelper::ColorType themeType)
{
    switch (themeType) {
    case DGuiApplicationHelper::UnknownType:
        break;
    case DGuiApplicationHelper::LightType:
        m_newPasswdLevelIconModePath = PASSWORD_LEVEL_ICON_LIGHT_MODE_PATH;
        break;
    case DGuiApplicationHelper::DarkType:
        m_newPasswdLevelIconModePath = PASSWORD_LEVEL_ICON_DEEP_MODE_PATH;
        break;
    }
}

bool PasswordWidget::isPasswordEmpty()
{
    return isContentEmpty(m_newPasswordEdit) || isContentEmpty(m_repeatPasswordEdit);
}

void PasswordWidget::hideAlert(DLineEdit *edit)
{
    if (edit->isAlert()) {
        edit->hideAlertMessage();
        edit->setAlert(false);
    }
}
