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

#include "unionidwidget.h"
#include "pwqualitymanager.h"

#include <QtConcurrent>

#include <DMessageManager>
#include <DFloatingMessage>
#include <DSpinner>

DGUI_USE_NAMESPACE

UnionIDWidget::UnionIDWidget(const QString &userPath, const QString &userName, QWidget *parent)
    : QWidget(parent)
    , m_unBindIcon(new QLabel)
    , m_stackedLayout(new QStackedLayout)
    , m_phoneEmailEdit (new DLineEdit )
    , m_verificationCodeEdit(new DLineEdit)
    , m_sendCodeBtn(new DSuggestButton)
    , m_passwordWidget(new PasswordWidget(81, userName))
    , m_count(0)
    , m_userPath(userPath)
    , m_userName(userName)
    , m_codeTimer(new QTimer(this))
    , m_verifyCodeSuccess(false)
{
    initWidget();
    initData();
}

void UnionIDWidget::initWidget()
{
    this->setAccessibleName("ResetPasswordPage");
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    setIconPath(type);

    QWidget *resetPasswordWidget = new QWidget;
    QVBoxLayout *resetPasswordVLayout = new QVBoxLayout(resetPasswordWidget);
    resetPasswordVLayout->setSpacing(0);
    resetPasswordVLayout->setMargin(0);
    resetPasswordVLayout->addSpacing(10);

    QLabel *phoneEmaillabel = new QLabel(tr("Phone/Email") + ":");
    resetPasswordVLayout->addWidget(phoneEmaillabel);
    resetPasswordVLayout->addSpacing(4);
    resetPasswordVLayout->addWidget(m_phoneEmailEdit);
    resetPasswordVLayout->addSpacing(14);

    QLabel *codeLabel = new QLabel(tr("Get Code") + ":");
    resetPasswordVLayout->addWidget(codeLabel);
    resetPasswordVLayout->addSpacing(4);
    QHBoxLayout *codeLayout = new QHBoxLayout;
    codeLayout->setSpacing(0);
    codeLayout->addWidget(m_verificationCodeEdit);
    codeLayout->addSpacing(10);
    codeLayout->addWidget(m_sendCodeBtn);
    resetPasswordVLayout->addLayout(codeLayout);
    resetPasswordVLayout->addSpacing(14);

    resetPasswordVLayout->addWidget(m_passwordWidget);

    QWidget *connectingWidget = new QWidget;
    QVBoxLayout *connectingVLayout = new QVBoxLayout(connectingWidget);
    connectingVLayout->setSpacing(0);
    connectingVLayout->setMargin(0);
    DSpinner *connectingIcon = new DSpinner;
    connectingIcon->setFixedSize(48, 48);
    connectingIcon->start();
    QLabel *connectingLabel = new QLabel(tr("Connecting to the Internet..."));
    connectingVLayout->addSpacing(104);
    connectingVLayout->addWidget(connectingIcon, 0, Qt::AlignCenter);
    connectingVLayout->addSpacing(72);
    connectingVLayout->addWidget(connectingLabel, 0, Qt::AlignCenter);
    connectingVLayout->addSpacing(164);

    QWidget *unBindWidget = new QWidget;
    QVBoxLayout *unBindVLayout = new QVBoxLayout(unBindWidget);
    unBindVLayout->setSpacing(0);
    unBindVLayout->setMargin(0);
    m_unBindIcon->setPixmap(QIcon::fromTheme(m_iconPath).pixmap({128, 128}));
    QLabel *unBindLabel = new QLabel(tr("The user account is not linked to Union ID"));
    unBindVLayout->addSpacing(64);
    unBindVLayout->addWidget(m_unBindIcon, 0, Qt::AlignCenter);
    unBindVLayout->addSpacing(32);
    unBindVLayout->addWidget(unBindLabel, 0, Qt::AlignCenter);
    unBindVLayout->addSpacing(164);

    QWidget *connectFailedWidget = new QWidget;
    QVBoxLayout *connectFailedVLayout = new QVBoxLayout(connectFailedWidget);
    connectFailedVLayout->setSpacing(0);
    connectFailedVLayout->setMargin(0);
    QLabel *connectFailedIcon = new QLabel;
    connectFailedIcon->setPixmap(QIcon::fromTheme("dialog-error").pixmap({128, 128}));
    QLabel *connectFailedLabel = new QLabel(tr("Network disconnected, please retry after connected"));
    connectFailedVLayout->addSpacing(64);
    connectFailedVLayout->addWidget(connectFailedIcon, 0, Qt::AlignCenter);
    connectFailedVLayout->addSpacing(32);
    connectFailedVLayout->addWidget(connectFailedLabel, 0, Qt::AlignCenter);
    connectFailedVLayout->addSpacing(164);

    m_stackedLayout->addWidget(connectingWidget);
    m_stackedLayout->addWidget(connectFailedWidget);
    m_stackedLayout->addWidget(resetPasswordWidget);
    m_stackedLayout->addWidget(unBindWidget);

    QVBoxLayout *mainContentLayout = new QVBoxLayout(this);
    mainContentLayout->setSpacing(0);
    mainContentLayout->setMargin(0);
    mainContentLayout->addLayout(m_stackedLayout);
}

void UnionIDWidget::initData()
{
    m_phoneEmailEdit->setPlaceholderText(tr("Required"));
    m_verificationCodeEdit->setPlaceholderText(tr("Required"));
    m_sendCodeBtn->setText(tr("Get Code"));

    connect(m_stackedLayout, &QStackedLayout::currentChanged, this, [ this ](int index) {
         Q_EMIT pageChanged(index == UNION_ID_STATUS_RESET_PASSWD);
    });
    connect(m_phoneEmailEdit, &DLineEdit::focusChanged, this, &UnionIDWidget::onPhoneEmailLineEditFocusChanged);
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

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [this](DGuiApplicationHelper::ColorType themeType) {
        setIconPath(themeType);
        m_unBindIcon->setPixmap(QIcon::fromTheme(m_iconPath).pixmap({128, 128}));
    });
    connect(m_sendCodeBtn, &QPushButton::clicked, this, &UnionIDWidget::onVerificationCodeBtnClicked);
    connect(m_codeTimer, &QTimer::timeout, this, &UnionIDWidget::startCount);
}

void UnionIDWidget::onPhoneEmailLineEditFocusChanged(bool onFocus)
{
    if (!onFocus && !m_phoneEmailEdit->text().isEmpty()) {
        if (checkPhoneEmailFormat(m_phoneEmailEdit->text())) {
            m_phoneEmailEdit->setAlert(false);
        } else {
            m_phoneEmailEdit->setAlert(true);
            m_phoneEmailEdit->showAlertMessage(tr("Phone/Email format is incorrect"), m_phoneEmailEdit, 2000);
        }
    }
}

void UnionIDWidget::onVerificationCodeBtnClicked()
{
    if (isContentEmpty(m_phoneEmailEdit)) {
        return;
    }

    if (checkPhoneEmailFormat(m_phoneEmailEdit->text())) {
        m_phoneEmailEdit->setAlert(false);
    } else {
        m_phoneEmailEdit->setAlert(true);
        m_phoneEmailEdit->showAlertMessage(tr("Phone/Email format is incorrect"), m_phoneEmailEdit, 2000);
        return;
    }

    m_verifyCodeSuccess = false;
    Q_EMIT requestAsyncVerficationCode(m_phoneEmailEdit->text());
}

bool UnionIDWidget::onResetPasswordBtnClicked()
{
    if (isContentEmpty(m_verificationCodeEdit) || m_passwordWidget->isPasswordEmpty()) {
        return false;
    }

    // 验证码
    if (!m_verifyCodeSuccess) {
        Q_EMIT requestVerifyVerficationCode(m_phoneEmailEdit->text(), m_verificationCodeEdit->text());
        if (!m_verifyCodeSuccess) {
            return false;
        }
    }

    return m_passwordWidget->checkPassword();
}

void UnionIDWidget::loadPage()
{
    m_stackedLayout->setCurrentIndex(UNION_ID_STATUS_CONNECTING);
    Q_EMIT m_stackedLayout->currentChanged(UNION_ID_STATUS_CONNECTING);
    Q_EMIT requestAsyncBindCheck();
}

int UnionIDWidget::parseError(const QString& errorMsg)
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

QString UnionIDWidget::getErrorTips(UnionIDWidget::UNION_ID_ERROR_TYPE errorType)
{
    QMap<int, QString> errorTypeMap = {
        { UNION_ID_ERROR_NO_ERR,   "" },
        { UNION_ID_ERROR_SYSTEM_ERROR, tr("System error") },
        { UNION_ID_ERROR_PARA_ERROR, "parameter error" },
        { UNION_ID_ERROR_LOGIN_EXPIRED, tr("Login expired, please sign in to the Union ID again") },
        { UNION_ID_ERROR_NO_PERMISSION, "no permission" },
        { UNION_ID_ERROR_NETWORK_ERROR, tr("Network error") },
        { UNION_ID_ERROR_CONFIGURE_ERROR, "Configuration error" },
        { UNION_ID_ERROR_REQUEST_REACHED, tr("You have reached the number limit to get the code today") },
        { UNION_ID_ERROR_USER_UNBIND, tr("The Union ID is not linked to a user account") },
    };
    return errorTypeMap.value(errorType);
}

bool UnionIDWidget::isContentEmpty(DLineEdit *edit)
{
    if (edit->text().isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("It cannot be empty"), edit, 2000);
    } else {
        edit->setAlert(false);
    }

    return edit->text().isEmpty();
}

bool UnionIDWidget::checkPhoneEmailFormat(const QString &content)
{
    QRegExp phoneRegExp("^1\\d{10}$");
    QRegExp emailRegExp("^[a-zA-Z0-9_-]+@[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9_-]+)+$");
    return phoneRegExp.exactMatch(content) || emailRegExp.exactMatch(content);
}

void UnionIDWidget::setIconPath(DGuiApplicationHelper::ColorType themeType)
{
    switch (themeType) {
    case DGuiApplicationHelper::UnknownType:
        break;
    case DGuiApplicationHelper::LightType:
        m_iconPath = BIND_ICON_LIGHT;
        break;
    case DGuiApplicationHelper::DarkType:
        m_iconPath = BIND_ICON_DARK;
        break;
    }
}

void UnionIDWidget::startCount()
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

void UnionIDWidget::onBindCheckUbidReplied(const QString& ubid)
{
    m_ubid = ubid;
}

void UnionIDWidget::onBindCheckReplied(int ret)
{
    if (ret == UNION_ID_ERROR_NO_ERR) {
        m_stackedLayout->setCurrentIndex(m_ubid.isEmpty() ? UNION_ID_STATUS_UNBIND : UNION_ID_STATUS_RESET_PASSWD);
    } else if (ret == UNION_ID_ERROR_USER_UNBIND) {
        m_stackedLayout->setCurrentIndex(UNION_ID_STATUS_UNBIND);
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
        m_stackedLayout->setCurrentIndex(UNION_ID_STATUS_CONNECT_FAILED);
        DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                 getErrorTips(UNION_ID_ERROR_NETWORK_ERROR));
    } else {
        m_stackedLayout->setCurrentIndex(UNION_ID_STATUS_CONNECT_FAILED);
        qWarning() << "onBindCheckReplied:" << ret;
    }
}

void UnionIDWidget::onVerficationCodeCountReplied(int count)
{
    m_count = count;
}

void UnionIDWidget::onRequestVerficationCodeReplied(int ret)
{
    if (ret == UNION_ID_ERROR_NO_ERR) {
        m_sendCodeBtn->setText(tr("Resend (%1s)").arg(m_count));
        m_codeTimer->start(1000);
    } else if (ret == UNION_ID_ERROR_USER_UNBIND) {
        m_phoneEmailEdit->setAlert(true);
        m_phoneEmailEdit->showAlertMessage(getErrorTips(UNION_ID_ERROR_USER_UNBIND), m_phoneEmailEdit, 2000);
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

void UnionIDWidget::onRequestVerifyVerficationCodeReplied(int ret)
{
    if ( ret == UNION_ID_ERROR_NO_ERR) {
        m_verifyCodeSuccess = true;
        m_verificationCodeEdit->setAlert(false);
    } else if (ret == UNION_ID_ERROR_USER_UNBIND) {
        m_phoneEmailEdit->setAlert(true);
        m_phoneEmailEdit->showAlertMessage(getErrorTips(UNION_ID_ERROR_USER_UNBIND), m_phoneEmailEdit, 2000);
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
    }
}

