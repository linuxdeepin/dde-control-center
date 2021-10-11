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
#include "pwqualitymanager.h"
#include "../../utils.h"
#include "createaccountpage.h"

#include "deepin_pw_check.h"

#include <DDialog>
#include <DFontSizeManager>
#include <DDBusSender>
#include <DDesktopServices>
#include <DApplicationHelper>

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

ModifyPasswdPage::ModifyPasswdPage(User *user, bool isCurrent, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_oldPasswordEdit(new DPasswordEdit)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
    , m_passwordTipsEdit(new DLineEdit)
    , m_newPasswdLevelText(new QLabel)
    , m_passwdLevelImg(new QImage)
    , m_level(PASSWORD_STRENGTH_LEVEL_HIGH)
    , m_focusOut(false)
    , m_isCurrent(isCurrent)
    , m_newPasswdLevelIconModePath(PASSWORD_LEVEL_ICON_LIGHT_MODE_PATH)

{
    //密码强度label初始化，３个label设置icon用于显示密码强度
    for (int i = 0; i < PASSWORD_LEVEL_ICON_NUM; i++) {
        m_newPasswdLevelIcons[i] = new QLabel;
    }
    m_oldPasswordEdit->setCopyEnabled(false);
    m_oldPasswordEdit->setCutEnabled(false);

    m_newPasswordEdit->setCopyEnabled(false);
    m_newPasswordEdit->setCutEnabled(false);

    m_repeatPasswordEdit->setCopyEnabled(false);
    m_repeatPasswordEdit->setCutEnabled(false);

    initWidget();
}

ModifyPasswdPage::~ModifyPasswdPage()
{
    for (int i = 0; i < PASSWORD_LEVEL_ICON_NUM; i++) {
        if (m_newPasswdLevelIcons[i] != nullptr)
            delete m_newPasswdLevelIcons[i];
    }

}

void ModifyPasswdPage::initWidget()
{
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
    this->setAccessibleName("ModifyPasswdPage");
    QVBoxLayout *mainContentLayout = new QVBoxLayout;
    mainContentLayout->addSpacing(40);

    TitleLabel *titleLabel = new TitleLabel(tr("Change Password"));
    mainContentLayout->addWidget(titleLabel, 0, Qt::AlignHCenter);
    if (!m_isCurrent) {
        titleLabel->setText(tr("Reset Password"));
        QLabel *label = new QLabel(tr("Resetting passwords do not change the login keyring. You can install seahorse to manage it."));
        label->setWordWrap(true);
        mainContentLayout->addWidget(label, 0, Qt::AlignHCenter);
    }
    mainContentLayout->addSpacing(40);

    if (m_isCurrent) {
        QLabel *oldPasswdLabel = new QLabel(tr("Current Password") + ":");
        mainContentLayout->addWidget(oldPasswdLabel);
        mainContentLayout->addWidget(m_oldPasswordEdit);
    }

    QHBoxLayout *newPasswdLayout = new QHBoxLayout;
    QLabel *newPasswdLabel = new QLabel(tr("New Password") + ":");
    newPasswdLayout->addWidget(newPasswdLabel);
    newPasswdLayout->addSpacing(80);

    QHBoxLayout *newPasswdLevelLayout = new QHBoxLayout;
    m_newPasswdLevelText->setFixedWidth(55);
    m_newPasswdLevelText->setFixedHeight(20);
    newPasswdLevelLayout->addWidget(m_newPasswdLevelText);
    newPasswdLevelLayout->setSpacing(4);

    m_newPasswdLevelIcons[0]->setFixedWidth(8);
    m_newPasswdLevelIcons[0]->setFixedHeight(4);
    m_passwdLevelImg->load(m_newPasswdLevelIconModePath);
    m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
    newPasswdLevelLayout->addWidget(m_newPasswdLevelIcons[0]);
    newPasswdLevelLayout->setSpacing(4);

    m_newPasswdLevelIcons[1]->setFixedWidth(8);
    m_newPasswdLevelIcons[1]->setFixedHeight(4);
    m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
    newPasswdLevelLayout->addWidget(m_newPasswdLevelIcons[1]);
    newPasswdLevelLayout->setSpacing(4);

    m_newPasswdLevelIcons[2]->setFixedWidth(8);
    m_newPasswdLevelIcons[2]->setFixedHeight(4);
    m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
    newPasswdLevelLayout->addWidget(m_newPasswdLevelIcons[2]);
    newPasswdLevelLayout->addSpacing(50);

    newPasswdLayout->addLayout(newPasswdLevelLayout);

    mainContentLayout->addLayout(newPasswdLayout);
    mainContentLayout->addWidget(m_newPasswordEdit);

    QLabel *repeatPasswdLabel = new QLabel(tr("Repeat Password") + ":");
    mainContentLayout->addWidget(repeatPasswdLabel);
    mainContentLayout->addWidget(m_repeatPasswordEdit);

    QLabel *passwdTipsLabel = new QLabel(tr("Password Hint") + ":");
    mainContentLayout->addWidget(passwdTipsLabel);
    mainContentLayout->addWidget(m_passwordTipsEdit);
    mainContentLayout->addStretch();

    QPushButton *cancleBtn = new QPushButton(tr("Cancel"));
    DSuggestButton *saveBtn = new DSuggestButton(tr("Save"));
    QHBoxLayout *cansaveLayout = new QHBoxLayout;
    cansaveLayout->setSpacing(10);
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

    connect(cancleBtn, &QPushButton::clicked, this, [&] {
        Q_EMIT requestBack();
    });

    connect(saveBtn, &DSuggestButton::clicked, this, &ModifyPasswdPage::clickSaveBtn);

    connect(m_curUser, &User::passwordModifyFinished, this, &ModifyPasswdPage::onPasswordChangeFinished);

    connect(m_curUser, &User::passwordStatusChanged, this, [ = ](const QString & status) {
        m_oldPasswordEdit->setVisible(status != NO_PASSWORD);
    });
    connect(m_curUser, &User::passwordResetFinished, this, &ModifyPasswdPage::resetPasswordFinished);

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
    connect(m_passwordTipsEdit, &DLineEdit::textEdited, this, [=](const QString &passwdTips) {
        if (passwdTips.size() > 14) {
            m_passwordTipsEdit->lineEdit()->backspace();
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        } else if (m_passwordTipsEdit->isAlert()) {
            m_passwordTipsEdit->setAlert(false);
        }
    });
    connect(m_newPasswordEdit, &DPasswordEdit::editingFinished, this, [=]() {
        QPalette palette;
        m_focusOut = true;
        m_level = PwqualityManager::instance()->GetNewPassWdLevel(m_newPasswordEdit->text());

        if (m_level == PASSWORD_STRENGTH_LEVEL_HIGH) {
            palette.setColor(QPalette::Text, QColor("#15BB18"));
            m_newPasswdLevelText->setPalette(palette);
            m_newPasswdLevelText->setText(tr("Strong"));

            m_passwdLevelImg->load(PASSWORD_LEVEL_ICON_HIGH_PATH);
            m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
            m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
            m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));

        } else if (m_level == PASSWORD_STRENGTH_LEVEL_MIDDLE) {
            palette.setColor(QPalette::Text, QColor("#FFAA00"));
            m_newPasswdLevelText->setPalette(palette);
            m_newPasswdLevelText->setText(tr("Medium"));

            m_passwdLevelImg->load(PASSWORD_LEVEL_ICON_MIDDLE_PATH);
            m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
            m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
            m_passwdLevelImg->load(m_newPasswdLevelIconModePath);
            m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));

            m_newPasswordEdit->showAlertMessage(tr("A stronger password is recommended: more than 8 characters, and contains 3 of the four character types: lowercase letters, uppercase letters, numbers, and symbols"));
        } else if (m_level == PASSWORD_STRENGTH_LEVEL_LOW) {
            palette.setColor(QPalette::Text, QColor("#FF5736"));
            m_newPasswdLevelText->setPalette(palette);
            m_newPasswdLevelText->setText(tr("Weak"));

            m_passwdLevelImg->load(PASSWORD_LEVEL_ICON_LOW_PATH);
            m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
            m_passwdLevelImg->load(m_newPasswdLevelIconModePath);
            m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
            m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));

            m_newPasswordEdit->showAlertMessage(tr("A stronger password is recommended: more than 8 characters, and contains 3 of the four character types: lowercase letters, uppercase letters, numbers, and symbols"));
        } else {
            m_newPasswordEdit->showAlertMessage(tr("Error occurred when reading the configuration files of password rules!"));
        }
    });

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [=](DGuiApplicationHelper::ColorType themeType){
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
        if (m_focusOut == false) {
            m_passwdLevelImg->load(m_newPasswdLevelIconModePath);
            m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
            m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
            m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
        } else {
            if (m_level == PASSWORD_STRENGTH_LEVEL_MIDDLE) {
                m_passwdLevelImg->load(m_newPasswdLevelIconModePath);
                m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
            } else if (m_level == PASSWORD_STRENGTH_LEVEL_LOW) {
                m_passwdLevelImg->load(m_newPasswdLevelIconModePath);
                m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
                m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(*m_passwdLevelImg));
            }
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
    if (errorTxt.contains("password unchanged")) {
        if (!errorTxt.contains("password right", Qt::CaseInsensitive)) {
            m_oldPasswordEdit->setAlert(true);
            m_oldPasswordEdit->showAlertMessage(tr("Wrong password"));
            return;
        }

        if (m_newPasswordEdit->lineEdit()->text() == m_oldPasswordEdit->lineEdit()->text() ) {
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

        // 密码校验失败并且安全中心密码安全等级不为低，弹出跳转到安全中心的对话框，低、中、高等级分别对应的值为1、2、3
        QDBusInterface interface(QStringLiteral("com.deepin.defender.daemonservice"),
                                 QStringLiteral("/com/deepin/defender/daemonservice"),
                                 QStringLiteral("com.deepin.defender.daemonservice"));
        QDBusReply<int> level = interface.call("GetPwdLimitLevel");
        if (!interface.isValid()) {
            return;
        }
        if (level != 1) {
            QDBusReply<QString> errorTips = interface.call("GetPwdError");
            DDialog dlg("", errorTips, this);
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
    } else if (error != PW_NO_ERR) {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        Q_EMIT requestChangePassword(m_curUser, m_newPasswordEdit->lineEdit()->text(), m_oldPasswordEdit->lineEdit()->text(), m_oldPasswordEdit->lineEdit()->text(), false);
    } else {
        if (!m_passwordTipsEdit->text().simplified().isEmpty())
            requestSetPasswordHint(m_curUser, m_passwordTipsEdit->text());
        Q_EMIT requestBack();
    }
}

void ModifyPasswdPage::setPasswordEditAttribute(DPasswordEdit *edit)
{
    edit->setAttribute(Qt::WA_InputMethodEnabled, false);
    edit->lineEdit()->setValidator(new QRegExpValidator(QRegExp("[^\\x4e00-\\x9fa5]+")));
}

void ModifyPasswdPage::resetPassword(const QString &password, const QString &repeatPassword)
{
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_curUser->name(),
                                                                                      password);

    if (error != PW_NO_ERR) {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        return;
    }

    if (password != repeatPassword) {
        m_repeatPasswordEdit->setAlert(true);
        m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
        return;
    }

    for (auto c : password) {
        if (m_passwordTipsEdit->text().contains(c)) {
            m_passwordTipsEdit->setAlert(true);
            m_passwordTipsEdit->showAlertMessage(tr("The hint is visible to all users. Do not include the password here."), m_passwordTipsEdit, 2000);
            return;
        }
    }

    if (!m_passwordTipsEdit->text().simplified().isEmpty())
        requestSetPasswordHint(m_curUser, m_passwordTipsEdit->text());

    Q_EMIT requestResetPassword(m_curUser, password);
}

//在修改密码页面当前密码处设置焦点
void ModifyPasswdPage::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    if (m_oldPasswordEdit && !m_oldPasswordEdit->hasFocus()) {
        m_oldPasswordEdit->lineEdit()->setFocus();
    }
}

void ModifyPasswdPage::resetPasswordFinished(const QString &errorText)
{
    if (errorText.isEmpty()) {
        Q_EMIT requestBack();
    } else {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(errorText, m_newPasswordEdit, 2000);
    }
}
