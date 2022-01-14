/*
 * Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
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

#include "resetpassworddialog.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QLabel>
#include <QDebug>
#include <QStyle>
#include <QWindow>

#include <pwd.h>
#include <unistd.h>
#include <libintl.h>
#include <random>
#include <crypt.h>
#include <iostream>

#include <DDesktopServices>
#include <DGuiApplicationHelper>
#include <DMessageManager>
#include <DFloatingMessage>
#include <QCloseEvent>

DGUI_USE_NAMESPACE

PwqualityManager::PwqualityManager()
    : m_passwordMinLen(0)
    , m_passwordMaxLen(0)
{
}

PwqualityManager *PwqualityManager::instance()
{
    static PwqualityManager pwquality;
    return &pwquality;
}

PwqualityManager::ERROR_TYPE PwqualityManager::verifyPassword(const QString &user, const QString &password, CheckType checkType)
{
    switch (checkType) {
    case PwqualityManager::Default: {
        ERROR_TYPE error = deepin_pw_check(user.toLocal8Bit().data(), password.toLocal8Bit().data(), LEVEL_STRICT_CHECK, nullptr);

        if (error == PW_ERR_PW_REPEAT) {
            error = PW_NO_ERR;
        }
        return error;
    }
    case PwqualityManager::Grub2: {
        // LEVEL_STRICT_CHECK?
        ERROR_TYPE error = deepin_pw_check_grub2(user.toLocal8Bit().data(), password.toLocal8Bit().data(), LEVEL_STANDARD_CHECK, nullptr);

        if (error == PW_ERR_PW_REPEAT) {
            error = PW_NO_ERR;
        }
        return error;
    }
    }

    return PW_NO_ERR;
}

PASSWORD_LEVEL_TYPE PwqualityManager::GetNewPassWdLevel(const QString &newPasswd)
{
    return get_new_passwd_strength_level(newPasswd.toLocal8Bit().data());
}

QString PwqualityManager::getErrorTips(PwqualityManager::ERROR_TYPE type, CheckType checkType)
{
    int passwordPalimdromeNum = (checkType == Default ? get_pw_palimdrome_num(LEVEL_STRICT_CHECK) : get_pw_palimdrome_num_grub2(LEVEL_STRICT_CHECK));
    int passwordMonotoneCharacterNum = (checkType == Default ? get_pw_monotone_character_num(LEVEL_STRICT_CHECK) : get_pw_monotone_character_num_grub2(LEVEL_STRICT_CHECK));
    int passwordConsecutiveSameCharacterNum = (checkType == Default ? get_pw_consecutive_same_character_num(LEVEL_STRICT_CHECK) : get_pw_consecutive_same_character_num_grub2(LEVEL_STRICT_CHECK));
    m_passwordMinLen = (checkType == Default ? get_pw_min_length(LEVEL_STRICT_CHECK) : get_pw_min_length_grub2(LEVEL_STRICT_CHECK));
    m_passwordMaxLen = (checkType == Default ? get_pw_max_length(LEVEL_STRICT_CHECK) : get_pw_max_length_grub2(LEVEL_STRICT_CHECK));

    //通用校验规则
    QMap<int, QString> PasswordFlagsStrMap = {
        {PW_ERR_PASSWORD_EMPTY, tr("Password cannot be empty")},
        {PW_ERR_LENGTH_SHORT, tr("Password must have at least %1 characters").arg(m_passwordMinLen)},
        {PW_ERR_LENGTH_LONG, tr("Password must be no more than %1 characters").arg(m_passwordMaxLen)},
        {PW_ERR_CHARACTER_INVALID, tr("Password can only contain English letters (case-sensitive), numbers or special symbols (~`!@#$%^&*()-_+=|\\{}[]:\"'<>,.?/)")},
        {PW_ERR_PALINDROME, tr("No more than %1 palindrome characters please").arg(passwordPalimdromeNum)},
        {PW_ERR_PW_MONOTONE, tr("No more than %1 monotonic characters please").arg(passwordMonotoneCharacterNum)},
        {PW_ERR_PW_CONSECUTIVE_SAME, tr("No more than %1 repeating characters please").arg(passwordConsecutiveSameCharacterNum)},
    };

    //服务器版校验规则
    if (IsServerSystem) {
        PasswordFlagsStrMap[PW_ERR_CHARACTER_INVALID] = tr("Password must contain uppercase letters, lowercase letters, numbers and symbols (~`!@#$%^&*()-_+=|\\{}[]:\"'<>,.?/)");
        PasswordFlagsStrMap[PW_ERR_PALINDROME] = tr("Password must not contain more than 4 palindrome characters");
        PasswordFlagsStrMap[PW_ERR_WORD] = tr("Do not use common words and combinations as password");
        PasswordFlagsStrMap[PW_ERR_PW_MONOTONE] = tr("Create a strong password please");
        PasswordFlagsStrMap[PW_ERR_PW_CONSECUTIVE_SAME] = tr("Create a strong password please");
        PasswordFlagsStrMap[PW_ERR_PW_FIRST_UPPERM] = tr("Do not use common words and combinations as password");
    }

    //规则校验以外的情况统一返回密码不符合安全要求
    if (PasswordFlagsStrMap.value(type).isEmpty()) {
        PasswordFlagsStrMap[type] = tr("It does not meet password rules");
    }

    return PasswordFlagsStrMap.value(type);
}

ResetPasswordDialog::ResetPasswordDialog(QRect screenGeometry, const QString &userName, const QString &appName, const int &fd)
    : DDialog(tr("Reset Password"), tr("By Union ID"))
    , m_screenGeometry(screenGeometry)
    , m_phoneEmailEdit (new DLineEdit )
    , m_verificationCodeEdit(new DLineEdit)
    , m_sendCodeBtn(new DSuggestButton)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
    , m_passwordTipsEdit(new DLineEdit)
    , m_newPasswdLevelText(new QLabel)
    , m_count(0)
    , m_isCodeCorrect(false)
    , m_userName(userName)
    , m_appName(appName)
    , m_fd(fd)
    , m_codeTimer(new QTimer(this))
    , m_monitorTimer(new QTimer(this))
    , m_verifyCodeSuccess(false)
{
    initWidget();
    initData();
}

void ResetPasswordDialog::showEvent(QShowEvent *event)
{
    DDialog::showEvent(event);

    move(m_screenGeometry.center() - rect().center());
}

void ResetPasswordDialog::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
}

void ResetPasswordDialog::mousePressEvent(QMouseEvent *event)
{
    if (this->geometry().contains(event->globalPos())) {
        DDialog::mousePressEvent(event);
    }
}

bool ResetPasswordDialog::eventFilter(QObject *o, QEvent *e)
{
    if (o == &m_tipDialog) {
        if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseButtonDblClick) {
            return true;
        }
    }
    return DDialog::eventFilter(o, e);
}

void ResetPasswordDialog::initWidget()
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

    this->setMinimumSize(380, 520);
    this->setAccessibleName("ResetPasswordPage");

    QWidget *mainContentWidget = new QWidget;
    QVBoxLayout *mainContentLayout = new QVBoxLayout(mainContentWidget);

    QLabel *phoneEmaillabel = new QLabel(tr("Phone/Email") + ":");
    mainContentLayout->addWidget(phoneEmaillabel);
    mainContentLayout->addWidget(m_phoneEmailEdit);

    QLabel *codeLabel = new QLabel(tr("Get Code") + ":");
    mainContentLayout->addWidget(codeLabel);
    QHBoxLayout *codeLayout = new QHBoxLayout;
    codeLayout->addWidget(m_verificationCodeEdit);
    codeLayout->addWidget(m_sendCodeBtn);
    mainContentLayout->addLayout(codeLayout);

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
    mainContentLayout->addLayout(newPasswordLayout);
    mainContentLayout->addWidget(m_newPasswordEdit);
    m_newPasswordEdit->setCopyEnabled(false);
    m_newPasswordEdit->setCutEnabled(false);

    QLabel *repeatPasswordLabel = new QLabel(tr("Repeat Password") + ":");
    mainContentLayout->addWidget(repeatPasswordLabel);
    mainContentLayout->addWidget(m_repeatPasswordEdit);
    m_repeatPasswordEdit->setCopyEnabled(false);
    m_repeatPasswordEdit->setCutEnabled(false);

    QLabel *passwdTipsLabel = new QLabel(tr("Password Hint") + ":");
    mainContentLayout->addWidget(passwdTipsLabel);
    mainContentLayout->addWidget(m_passwordTipsEdit);
    this->insertContent(0, mainContentWidget);
    this->setIcon(QIcon::fromTheme("dialog-warning"));
    QStringList buttons;
    buttons << tr("Cancel");
    this->addButtons(buttons);
    this->addButton(tr("Reset"), true, ButtonRecommend);

    m_tipDialog.setMessage(tr("Resetting the password will clear the data stored in your keyring, and you should log in again, please save files in advance"));
    m_tipDialog.addButtons(QStringList() << tr("Cancel") << tr("Confirm and Reset"));
    m_tipDialog.setIcon(QIcon::fromTheme("dialog-warning"));
    m_tipDialog.setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowStaysOnTopHint);
    m_tipDialog.installEventFilter(this);
    m_tipDialog.setFixedSize(380, 189);
    m_tipDialog.setOnButtonClickedClose(false);

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowStaysOnTopHint);
    setOnButtonClickedClose(false);
}

void ResetPasswordDialog::initData()
{
    m_phoneEmailEdit->setPlaceholderText(tr("Required"));
    m_verificationCodeEdit->setPlaceholderText(tr("Required"));
    m_sendCodeBtn->setText(tr("Get Code"));
    m_newPasswordEdit->setPlaceholderText(tr("Required"));
    m_repeatPasswordEdit->setPlaceholderText(tr("Required"));
    m_passwordTipsEdit->setPlaceholderText(tr("Optional"));

    filein.open(stdin, QIODevice::ReadOnly);
    QSocketNotifier* sn = new QSocketNotifier(filein.handle(), QSocketNotifier::Read, this);
    sn->setEnabled(true);

    m_accountInter = new com::deepin::daemon::Accounts("com.deepin.daemon.Accounts", "/com/deepin/daemon/Accounts", QDBusConnection::systemBus());
    auto reply = m_accountInter->FindUserByName(m_userName);
    reply.waitForFinished();
    if (reply.isValid()) {
        m_user = reply.value();
        m_userInter = new com::deepin::daemon::accounts::User("com.deepin.daemon.Accounts", m_user, QDBusConnection::systemBus());
    } else {
        qWarning() << QString("get user (%1) failed:").arg(m_user) << reply.error();
    }

    connect(sn, SIGNAL(activated(int)), this, SLOT(onReadFromServerChanged(int)));
    connect(m_phoneEmailEdit, &DLineEdit::focusChanged, this, &ResetPasswordDialog::onPhoneEmailLineEditFocusChanged);
    connect(m_newPasswordEdit, &DLineEdit::textChanged, this, &ResetPasswordDialog::onNewPasswordLineEditChanged);
    connect(getButton(0), &QPushButton::clicked, this, [this]{
        if (m_appName == "greeter" || m_appName == "lock") {
            m_client->write("close");
            m_client->flush();
        }
        this->close();
        qApp->quit();
    });
    connect(getButton(1), &QPushButton::clicked, this, &ResetPasswordDialog::onResetPasswordBtnClicked);
    connect(m_phoneEmailEdit, &DLineEdit::textEdited, this, [ & ] {
        if (m_phoneEmailEdit->isAlert()) {
            m_phoneEmailEdit->hideAlertMessage();
            m_phoneEmailEdit->setAlert(false);
        }
    });
    connect(m_verificationCodeEdit, &DLineEdit::textEdited, this, [ & ] {
        if (m_verificationCodeEdit->isAlert()) {
            m_verificationCodeEdit->hideAlertMessage();
            m_verificationCodeEdit->setAlert(false);
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
            m_passwordTipsEdit->setAlert(false);
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
    connect(m_sendCodeBtn, &QPushButton::clicked, this, &ResetPasswordDialog::onVerificationCodeBtnClicked);
    connect(m_codeTimer, &QTimer::timeout, this, &ResetPasswordDialog::startCount);
    connect(m_monitorTimer, &QTimer::timeout, this, &ResetPasswordDialog::updatePosition);
    connect(m_tipDialog.getButton(0), &QPushButton::clicked, this, [this]{
        updatePosition();
        this->show();
        m_tipDialog.hide();
    });
    connect(m_tipDialog.getButton(1), &QPushButton::clicked, this, [this]{
        updatePosition();
        this->show();
        m_tipDialog.hide();
        QFile file;
        if (file.open(m_fd, QIODevice::WriteOnly)) {
            file.write((cryptUserPassword(m_newPasswordEdit->text())+ "\n").toStdString().c_str());
            file.flush();
            file.close();
        }
    });
    if (m_appName == "greeter" || m_appName == "lock") {
        m_monitorTimer->start(300);
        m_client = new QLocalSocket(this);
        m_client->abort();
        m_client->connectToServer("GrabKeyboard");
        if(!m_client->waitForConnected(1000)) {
            qWarning() << "connect failed!" << m_client->errorString();
            return;
        }
    }
}

void ResetPasswordDialog::onPhoneEmailLineEditFocusChanged(bool onFocus)
{
    if (!onFocus && !m_phoneEmailEdit->text().isEmpty()) {
        if (checkPhoneEmailFormat(m_phoneEmailEdit->text())) {
            m_phoneEmailEdit->setAlert(false);
        } else {
            m_phoneEmailEdit->setAlert(true);
            m_phoneEmailEdit->showAlertMessage(tr("Phone/Email format is incorrect"), m_repeatPasswordEdit, 2000);
        }
    }
}

void ResetPasswordDialog::onVerificationCodeBtnClicked()
{
    if (isContentEmpty(m_phoneEmailEdit)) {
        return;
    }

    if (checkPhoneEmailFormat(m_phoneEmailEdit->text())) {
        m_phoneEmailEdit->setAlert(false);
    } else {
        m_phoneEmailEdit->setAlert(true);
        m_phoneEmailEdit->showAlertMessage(tr("Phone/Email format is incorrect"), m_repeatPasswordEdit, 2000);
        return;
    }

    m_verifyCodeSuccess = false;
    int ret = requestVerficationCode();
    if (ret == UNION_ID_ERROR_NO_ERR) {
        m_sendCodeBtn->setText(tr("Resend (%1s)").arg(m_count));
        m_codeTimer->start(1000);
    } else if (ret == UNION_ID_ERROR_USER_UNBIND) {
        m_phoneEmailEdit->setAlert(true);
        m_phoneEmailEdit->showAlertMessage(getErrorTips(UNION_ID_ERROR_USER_UNBIND), m_repeatPasswordEdit, 2000);
    } else if (ret == UNION_ID_ERROR_REQUEST_REACHED) {
        DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                 getErrorTips(UNION_ID_ERROR_REQUEST_REACHED));
    } else if (ret == UNION_ID_ERROR_SYSTEM_ERROR) {
        DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                 getErrorTips(UNION_ID_ERROR_SYSTEM_ERROR));
    } else if (ret == UNION_ID_ERROR_LOGIN_EXPIRED) {
        DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                 getErrorTips(UNION_ID_ERROR_LOGIN_EXPIRED));
    } else if (ret == UNION_ID_ERROR_NETWORK_ERROR) {
        DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                 getErrorTips(UNION_ID_ERROR_NETWORK_ERROR));
    } else {
        DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                 tr("Failed to get the code"));
    }
}

void ResetPasswordDialog::onNewPasswordLineEditChanged(const QString&)
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

void ResetPasswordDialog::onResetPasswordBtnClicked()
{
    if (isContentEmpty(m_verificationCodeEdit) ||isContentEmpty(m_newPasswordEdit) || isContentEmpty(m_repeatPasswordEdit)) {
        return;
    }

    // 密码强度
    PwqualityManager::ERROR_TYPE error = PwqualityManager::instance()->verifyPassword(m_userName,
                                                                                      m_newPasswordEdit->text());

    if (error != PW_NO_ERR) {
        m_newPasswordEdit->setAlert(true);
        m_newPasswordEdit->showAlertMessage(PwqualityManager::instance()->getErrorTips(error));
        return;
    }

    // 验证码
    if (!m_verifyCodeSuccess) {
        int ret = verifyVerficationCode();
        if ( ret == UNION_ID_ERROR_NO_ERR) {
            m_verifyCodeSuccess = true;
            m_verificationCodeEdit->setAlert(false);
        } else if (ret == UNION_ID_ERROR_USER_UNBIND) {
            m_phoneEmailEdit->setAlert(true);
            m_phoneEmailEdit->showAlertMessage(getErrorTips(UNION_ID_ERROR_USER_UNBIND), m_repeatPasswordEdit, 2000);
        } else if (ret == UNION_ID_ERROR_SYSTEM_ERROR) {
            DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                     getErrorTips(UNION_ID_ERROR_SYSTEM_ERROR));
        } else if (ret == UNION_ID_ERROR_LOGIN_EXPIRED) {
            DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                     getErrorTips(UNION_ID_ERROR_LOGIN_EXPIRED));
        } else if (ret == UNION_ID_ERROR_NETWORK_ERROR) {
            DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                     getErrorTips(UNION_ID_ERROR_NETWORK_ERROR));
        } else {
            m_verificationCodeEdit->setAlert(true);
            m_verificationCodeEdit->showAlertMessage(tr("Wrong verification code"), m_verificationCodeEdit, 2000);
            return;
        }
    }

    // 新密码和重复密码是否一致
    if (m_newPasswordEdit->text() != m_repeatPasswordEdit->text()) {
        m_repeatPasswordEdit->setAlert(true);
        m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match"), m_repeatPasswordEdit, 2000);
        return;
    }

    // 密码提示
    for (auto c : m_newPasswordEdit->text()) {
        if (m_passwordTipsEdit->text().contains(c)) {
            m_passwordTipsEdit->setAlert(true);
            m_passwordTipsEdit->showAlertMessage(tr("The hint is visible to all users. Do not include the password here."), getButton(1), 2000);
            return;
        }
    }
    if (!m_passwordTipsEdit->text().simplified().isEmpty()) {
        m_userInter->SetPasswordHint(m_passwordTipsEdit->text()).waitForFinished();
    }

    updatePosition();
    m_tipDialog.show();
    this->hide();
}

int ResetPasswordDialog::parseError(const QString& errorMsg)
{
    QJsonParseError jsonError;
    QJsonDocument doucment = QJsonDocument::fromJson(errorMsg.toLatin1(), &jsonError);
    if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {
        if (doucment.isObject()) {
            QJsonObject object = doucment.object();
            if (object.contains("code")) {
                QJsonValue value = object.value("code");
                if (value.isDouble()) {
                    return value.toInt();
                }
            }
        }
    }
    return -1;
}

QString ResetPasswordDialog::getErrorTips(ResetPasswordDialog::UNION_ID_ERROR_TYPE errorType)
{
    QMap<int, QString> errorTypeMap = {
        {UNION_ID_ERROR_NO_ERR,   ""},
        {UNION_ID_ERROR_SYSTEM_ERROR, tr("System error")},
        {UNION_ID_ERROR_PARA_ERROR, "parameter error"},
        {UNION_ID_ERROR_LOGIN_EXPIRED, tr("Login expired, please sign in to the Union ID again")},
        {UNION_ID_ERROR_NO_PERMISSION, "no permission"},
        {UNION_ID_ERROR_NETWORK_ERROR, tr("Network error")},
        {UNION_ID_ERROR_CONFIGURE_ERROR, "Configuration error"},
        {UNION_ID_ERROR_REQUEST_REACHED, tr("You have reached the number limit to get the code today")},
        {UNION_ID_ERROR_USER_UNBIND, tr("The Union ID is not linked to a user account")},
    };
    return errorTypeMap.value(errorType);
}

QRect ResetPasswordDialog::screenGeometry() const
{
    return m_screenGeometry;
}

void ResetPasswordDialog::setScreenGeometry(const QRect &screenGeometry)
{
    m_screenGeometry = screenGeometry;
}

bool ResetPasswordDialog::isContentEmpty(DLineEdit *edit)
{
    if (edit->text().isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("It cannot be empty"), edit, 2000);
    } else {
        edit->setAlert(false);
    }

    return edit->text().isEmpty();
}

QString ResetPasswordDialog::cryptUserPassword(const QString &password)
{
    /*
        NOTE(kirigaya): Password is a combination of salt and crypt function.
        slat is begin with $6$, 16 byte of random values, at the end of $.
        crypt function will return encrypted values.
     */

    const QString seedchars("./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
    char salt[] = "$6$................$";

    std::random_device r;
    std::default_random_engine e1(r());
    std::uniform_int_distribution<int> uniform_dist(0, seedchars.size() - 1); //seedchars.size()是64，生成随机数的范围应该写成[0, 63]。

    // Random access to a character in a restricted list
    for (int i = 0; i != 16; i++) {
        salt[3 + i] = seedchars.at(uniform_dist(e1)).toLatin1();
    }

    return crypt(password.toUtf8().data(), salt);
}

bool ResetPasswordDialog::checkPhoneEmailFormat(const QString &content)
{
    QRegExp phoneRegExp("^1(3[0-9]|4[57]|5[0-35-9]|7[0135678]|8[0-9])\\d{8}$");
    QRegExp emailRegExp("^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$");
    return phoneRegExp.exactMatch(content) || emailRegExp.exactMatch(content);
}

void ResetPasswordDialog::updatePasswordStrengthLevelWidget(PASSWORD_LEVEL_TYPE level)
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

int ResetPasswordDialog::requestVerficationCode()
{
    QDBusInterface syncHelperInter("com.deepin.sync.Helper", "/com/deepin/sync/Helper",
                                   "com.deepin.sync.Helper", QDBusConnection::systemBus());
    if (!syncHelperInter.isValid()) {
        qWarning() << "syncHelper interface:" << syncHelperInter.lastError();
        return -1;
    }
    QDBusReply<QString> retUOSID = syncHelperInter.call("UOSID");
    QString uosid;
    if (retUOSID.error().message().isEmpty()) {
        uosid = retUOSID.value();
        qDebug() << "UOSID success!";
    } else {
        qWarning() << "UOSID failed:" << retUOSID.error().message();
        return -1;
    }

    if (m_user.isEmpty()) {
        return -1;
    }
    QDBusInterface accountsInter("com.deepin.daemon.Accounts",
                                 m_user,
                                 "com.deepin.daemon.Accounts.User",
                                 QDBusConnection::systemBus());
    if (!accountsInter.isValid()) {
        return -1;
    }

    QVariant retUUID = accountsInter.property("UUID");
    QString uuid = retUUID.toString();

    QDBusReply<QString> retLocalBindCheck= syncHelperInter.call("LocalBindCheck", uosid, uuid);
    if (retLocalBindCheck.error().message().isEmpty()) {
        m_ubid = retLocalBindCheck.value();
        qDebug() << "isBinded:" << !m_ubid.isEmpty();
    } else {
        qWarning() << "isBinded failed:" << retLocalBindCheck.error().message();
        int code = parseError(retLocalBindCheck.error().message());
        return code;
    }

    QDBusReply<int> retResetCaptcha = syncHelperInter.call("SendResetCaptcha", m_ubid, m_phoneEmailEdit->text());
    if (retResetCaptcha.error().message().isEmpty()) {
        m_count = retResetCaptcha.value();
        qDebug() << "SendResetCaptcha success:" << m_count;
    } else {
        qWarning() << "SendResetCaptcha failed:" << retResetCaptcha.error().message();
        int code = parseError(retResetCaptcha.error().message());
        return code;
    }
    return 0;
}

int ResetPasswordDialog::verifyVerficationCode()
{
    QDBusInterface syncHelperInter("com.deepin.sync.Helper",
                                   "/com/deepin/sync/Helper",
                                   "com.deepin.sync.Helper",
                                   QDBusConnection::systemBus());
    if (!syncHelperInter.isValid()) {
        qWarning() << "syncHelper interface:" << syncHelperInter.lastError();
        return -1;
    }
    QDBusReply<bool> retVerifyResetCaptcha = syncHelperInter.call("VerifyResetCaptcha",
                                                                  m_ubid,
                                                                  m_phoneEmailEdit->text(),
                                                                  m_verificationCodeEdit->text());
    int ret = -1;
    if (retVerifyResetCaptcha.error().message().isEmpty()) {
        ret = retVerifyResetCaptcha.value();
        qDebug() << "VerifyResetCaptcha success";
        ret = 0;
    } else {
        qWarning() << "VerifyResetCaptcha failed:" << retVerifyResetCaptcha.error().message();
        ret = parseError(retVerifyResetCaptcha.error().message());
    }

    return ret;
}

void ResetPasswordDialog::startCount()
{
    if (m_count == 1) {
        m_codeTimer->stop();
        m_sendCodeBtn->setEnabled(true);
        m_sendCodeBtn->setText(tr("Get Code"));
    } else {
        --m_count;
        m_sendCodeBtn->setText(tr("Resend (%1s)").arg(m_count));
    }
}

void ResetPasswordDialog::quit()
{
    DFloatingMessage *m_resetPasswordFloatingMessage = new DFloatingMessage(DFloatingMessage::MessageType::TransientType);
    m_resetPasswordFloatingMessage->setIcon(QIcon::fromTheme("dialog-ok"));
    m_resetPasswordFloatingMessage->setMessage(tr("Successfully reset, please log in and unlock with the new password"));
    m_resetPasswordFloatingMessage->setDuration(2000);
    DMessageManager::instance()->sendMessage(this, m_resetPasswordFloatingMessage);
    QTimer::singleShot(3000, [this] {
        this->close();
        qApp->quit();
    });
}

void ResetPasswordDialog::onReadFromServerChanged(int fd)
{
    if(fd != filein.handle())
        return;
    char buffer[1024];
    ssize_t n = read(filein.handle(), buffer, 1024);
    buffer[n]='\0';
    QString content = buffer;
    if (content.startsWith("success")) {
        quit();
    } else {
        DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("dialog-warning"), content);
    }
}

void ResetPasswordDialog::updatePosition()
{
    const QRect deskRt = QApplication::desktop()->screenGeometry(QCursor::pos());
    int x = deskRt.left() + (deskRt.width() - width()) / 2;
    int y = deskRt.top() + (deskRt.height() - height()) / 2;
    move(x, y);
    int tx = deskRt.left() + (deskRt.width() - m_tipDialog.width()) / 2;
    int ty = deskRt.top() + (deskRt.height() - m_tipDialog.height()) / 2;
    m_tipDialog.move(tx, ty);
}

Manager::Manager(const QString &userName, const QString &appName, const int &fd)
    : QObject()
    , m_dialog(nullptr)
    , m_usrName(userName)
    , m_appName(appName)
    , m_fd(fd)
{
}

void Manager::setupDialog()
{
    const auto ratio = qApp->devicePixelRatio();
    const QPoint cp(QCursor::pos());
    for (const auto *s : qApp->screens()) {
        const QRect &g = s->geometry();
        const QRect realRect(g.topLeft() / ratio, g.size());
        // Without using the processed geometry and mouse coordinates,
        // the results can be obtained using the original information.
        // If the original screen contains the original mouse, save the scaled geometry.
        if (g.contains(cp)) {
            if (!m_dialog) {
                m_dialog = new ResetPasswordDialog(realRect, m_usrName, m_appName, m_fd);
            } else {
                m_dialog->setScreenGeometry(realRect);
            }
            break;
        }
    }
}

void Manager::showDialog()
{
    m_dialog->show();
}

void Manager::start()
{
    setupDialog();
    showDialog();
}
