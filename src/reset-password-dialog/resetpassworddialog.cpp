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
#include <pwd.h>
#include <unistd.h>
#include <libintl.h>
#include <random>
#include <crypt.h>
#include <DDesktopServices>
#include <DGuiApplicationHelper>
#include <DMessageManager>
#include <QStyle>
#include <DFloatingMessage>
#include <iostream>

DGUI_USE_NAMESPACE

ResetPasswordDialog::ResetPasswordDialog(QRect screenGeometry, QString uuid, QString app)
    : DDialog(tr("Reset Password"), tr("By Union ID"))
    , m_screenGeometry(screenGeometry)
    , m_phoneEmailEdit (new DLineEdit )
    , m_verificationCodeEdit(new DLineEdit)
    , m_sendCodeBtn(new DSuggestButton)
    , m_newPasswordEdit(new DPasswordEdit)
    , m_repeatPasswordEdit(new DPasswordEdit)
    , m_passwordTipsEdit(new DLineEdit)
    , m_newPasswdLevelText(new QLabel)
    , m_timer(new QTimer(this))
    , m_count(0)
    , m_isCodeCorrect(false)
    , m_uuid(uuid)
    , m_monitorTimer(new QTimer(this))
    , m_app(app)
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

    QLabel *codeLabel = new QLabel(tr("Send Code") + ":");
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

    QLabel *repeatPasswordLabel = new QLabel(tr("Repeat Password") + ":");
    mainContentLayout->addWidget(repeatPasswordLabel);
    mainContentLayout->addWidget(m_repeatPasswordEdit);

    QLabel *passwdTipsLabel = new QLabel(tr("Password Hint") + ":");
    mainContentLayout->addWidget(passwdTipsLabel);
    mainContentLayout->addWidget(m_passwordTipsEdit);
    this->insertContent(0, mainContentWidget);
    this->setIcon(QIcon::fromTheme("dialog-warning"));
    QStringList buttons;
    buttons << tr("Cancel");
    this->addButtons(buttons);
    this->addButton(tr("Reset"), true, ButtonRecommend);

    Qt::WindowFlags m_flags = windowFlags();
    setWindowFlags(m_flags | Qt::WindowStaysOnTopHint);
    setOnButtonClickedClose(false);
    setAttribute(Qt::WA_DeleteOnClose, true);
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

    const QString AccountsService("com.deepin.daemon.Accounts");
    const QString path = QString("/com/deepin/daemon/Accounts/User%1").arg(getuid());
    m_user = new com::deepin::daemon::accounts::User(AccountsService, path, QDBusConnection::systemBus());

    connect(sn, SIGNAL(activated(int)), this, SLOT(onReadFromServerChanged(int)));
    connect(m_phoneEmailEdit, &DLineEdit::focusChanged, this, &ResetPasswordDialog::onPhoneEmailLineEditFocusChanged);
    connect(m_verificationCodeEdit, &DLineEdit::focusChanged, this, &ResetPasswordDialog::onVerificationCodeLineEditFocusChanged);
    connect(m_newPasswordEdit, &DLineEdit::focusChanged, this, &ResetPasswordDialog::onNewPasswordLineEditFocusChanged);
    connect(m_repeatPasswordEdit, &DLineEdit::focusChanged, this, &ResetPasswordDialog::onRepeatPasswordLineEditFocusChanged);
    connect(m_passwordTipsEdit, &DLineEdit::focusChanged, this, &ResetPasswordDialog::onPasswordTipLineEditFocusChanged);
    connect(getButton(0), &QPushButton::clicked, this, [this]{ this->close(); });
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
    connect(m_timer, &QTimer::timeout, this, &ResetPasswordDialog::startCount);
    connect(m_monitorTimer, &QTimer::timeout, this, &ResetPasswordDialog::startMonitor);
    if (m_app != "control-center") {
        m_monitorTimer->start(300);
    }
}

void ResetPasswordDialog::onPhoneEmailLineEditFocusChanged(bool onFocus)
{
    if (!onFocus) {
        if(isContentEmpty(m_phoneEmailEdit)) {
            return;
        }
        if (checkPhoneEmailFormat(m_phoneEmailEdit->text())) {
            m_phoneEmailEdit->setAlert(false);
        } else {
            m_phoneEmailEdit->setAlert(true);
            m_phoneEmailEdit->showAlertMessage(tr("Phone/Email format is not correct!"), m_repeatPasswordEdit, 2000);
        }
    }
}

void ResetPasswordDialog::onVerificationCodeBtnClicked()
{
    bool ret = requestVerficationCode();
    if (ret) {
        m_sendCodeBtn->setText(tr("Resend (%1s)").arg(m_count));
        m_timer->start(1000);
    }
}

void ResetPasswordDialog::onVerificationCodeLineEditFocusChanged(bool onFocus)
{
    if (!onFocus) {
        if (isContentEmpty(m_verificationCodeEdit)) {
            return;
        }
        //        m_isCodeCorrect = verifyVerficationCode();
        //        if (m_isCodeCorrect) {
        //            m_verificationCodeEdit->setEnabled(false);
        //            m_verificationCodeEdit->setAlert(false);
        //        } else {
        //            m_verificationCodeEdit->setAlert(true);
        //            m_verificationCodeEdit->showAlertMessage(tr("Verify verification code failed!"), m_verificationCodeEdit, 2000);
        //        }
    }
}

void ResetPasswordDialog::onNewPasswordLineEditFocusChanged(bool onFocus)
{
    if (!onFocus) {
        if (isContentEmpty(m_newPasswordEdit)) {
            m_newPasswordEdit->setText("");
            return;
        }
        updatePasswordStrengthLevelWidget();
    }
}

void ResetPasswordDialog::onRepeatPasswordLineEditFocusChanged(bool onFocus)
{
    if (!onFocus) {
        if (isContentEmpty(m_newPasswordEdit)) {
            return;
        }
    }
}

void ResetPasswordDialog::onPasswordTipLineEditFocusChanged(bool onFocus)
{
    if (!onFocus) {
        for (auto c : m_newPasswordEdit->text()) {
            if (m_passwordTipsEdit->text().contains(c)) {
                m_passwordTipsEdit->setAlert(true);
                m_passwordTipsEdit->showAlertMessage(tr("The hint is visible to all users. Do not include the password here."), m_passwordTipsEdit->parentWidget(), 2000);
                return;
            }
        }
    }
}

void ResetPasswordDialog::onResetPasswordBtnClicked()
{
    if (isContentEmpty(m_verificationCodeEdit) ||isContentEmpty(m_newPasswordEdit) || isContentEmpty(m_repeatPasswordEdit)) {
        return;
    }
    if (verifyVerficationCode()) {
        m_verificationCodeEdit->setAlert(false);
    } else {
        m_verificationCodeEdit->setAlert(true);
        m_verificationCodeEdit->showAlertMessage(tr("Verify verification code failed!"), m_verificationCodeEdit, 2000);
        return;
    }
    if (m_newPasswordEdit->text() != m_repeatPasswordEdit->text()) {
        m_repeatPasswordEdit->setAlert(true);
        m_repeatPasswordEdit->showAlertMessage(tr("Passwords do not match!"), m_repeatPasswordEdit, 2000);
        return;
    }
    for (auto c : m_newPasswordEdit->text()) {
        if (m_passwordTipsEdit->text().contains(c)) {
            m_passwordTipsEdit->setAlert(true);
            m_passwordTipsEdit->showAlertMessage(tr("The hint is visible to all users. Do not include the password here."), m_passwordTipsEdit, 2000);
            return;
        }
    }
    if (!m_passwordTipsEdit->text().simplified().isEmpty()) {
        m_user->SetPasswordHint(m_passwordTipsEdit->text()).waitForFinished();
    }
    std::cout << cryptUserPassword(m_newPasswordEdit->text()).toStdString().c_str() << std::endl;
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

QRect ResetPasswordDialog::screenGeometry() const
{
    return m_screenGeometry;
}

void ResetPasswordDialog::setScreenGeometry(const QRect &screenGeometry)
{
    m_screenGeometry = screenGeometry;
}

void ResetPasswordDialog::clearInfo()
{
    m_phoneEmailEdit->clear();
    m_phoneEmailEdit->setAlert(false);
    m_verificationCodeEdit->clear();
    m_verificationCodeEdit->setAlert(false);
    m_sendCodeBtn->setText(tr("Required Verification Code"));
    m_newPasswordEdit->clear();
    m_newPasswordEdit->setAlert(false);
    m_repeatPasswordEdit->clear();
    m_repeatPasswordEdit->setAlert(false);
    m_passwordTipsEdit->clear();
    m_passwordTipsEdit->setAlert(false);
    m_newPasswdLevelText->setText("");
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
}

bool ResetPasswordDialog::isContentEmpty(DLineEdit *edit)
{
    if (edit->text().isEmpty()) {
        edit->setAlert(true);
        edit->showAlertMessage(tr("Content cannot be empty"), edit, 2000);
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

ResetPasswordDialog::PasswordStrengthLevel ResetPasswordDialog::getPasswordStrengthLevel(const QString &password)
{
    QRegExp strongRegExp("^(?=.*[\\da-z])(?=.*[\\dA-Z])(?=.*[\\d\\W_])(?=.*[A-Z\\W_])(?=.*[a-zA-Z])(?=.*[a-z\\W_]).{8,}$");
    QRegExp middleRegExp("^(?=.*[\\da-zA-Z])(?=.*[\\da-z\\W_])(?=.*[\\dA-Z\\W_])(?=.*[a-zA-Z\\W_]).{6,}$");
    if (strongRegExp.exactMatch(password)) {
        return PASSWORD_STRENGTH_LEVEL_HIGH;
    } else if (middleRegExp.exactMatch(password)) {
        return PASSWORD_STRENGTH_LEVEL_MIDDLE;
    } else {
        return PASSWORD_STRENGTH_LEVEL_LOW;
    }
}

void ResetPasswordDialog::updatePasswordStrengthLevelWidget()
{
    QPalette palette;
    m_level = getPasswordStrengthLevel(m_newPasswordEdit->text());
    QImage img;
    if (m_level == PASSWORD_STRENGTH_LEVEL_HIGH) {
        palette.setColor(QPalette::WindowText, QColor("#15BB18"));
        m_newPasswdLevelText->setPalette(palette);
        m_newPasswdLevelText->setText(tr("Strong"));

        img.load(PASSWORD_LEVEL_ICON_HIGH_PATH);
        m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));

    } else if (m_level == PASSWORD_STRENGTH_LEVEL_MIDDLE) {
        palette.setColor(QPalette::WindowText, QColor("#FFAA00"));
        m_newPasswdLevelText->setPalette(palette);
        m_newPasswdLevelText->setText(tr("Medium"));

        img.load(PASSWORD_LEVEL_ICON_MIDDLE_PATH);
        m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(img));
        img.load(m_newPasswdLevelIconModePath);
        m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));

        m_newPasswordEdit->showAlertMessage(tr("A stronger password is recommended: more than 8 characters, and contains 3 of the four character types: lowercase letters, uppercase letters, numbers, and symbols."));
    } else if (m_level == PASSWORD_STRENGTH_LEVEL_LOW) {
        palette.setColor(QPalette::WindowText, QColor("#FF5736"));
        m_newPasswdLevelText->setPalette(palette);
        m_newPasswdLevelText->setText(tr("Weak"));

        img.load(PASSWORD_LEVEL_ICON_LOW_PATH);
        m_newPasswdLevelIcons[0]->setPixmap(QPixmap::fromImage(img));
        img.load(m_newPasswdLevelIconModePath);
        m_newPasswdLevelIcons[1]->setPixmap(QPixmap::fromImage(img));
        m_newPasswdLevelIcons[2]->setPixmap(QPixmap::fromImage(img));

        m_newPasswordEdit->showAlertMessage(tr("A stronger password is recommended: more than 8 characters, and contains 3 of the four character types: lowercase letters, uppercase letters, numbers, and symbols."));
    } else {
        m_newPasswordEdit->showAlertMessage(tr("Error occurred when reading the configuration files of password rules!"));
    }
}

bool ResetPasswordDialog::requestVerficationCode()
{
    QDBusInterface syncHelperInter("com.deepin.sync.Helper", "/com/deepin/sync/Helper",
                                   "com.deepin.sync.Helper", QDBusConnection::systemBus());
    if (!syncHelperInter.isValid()) {
        qWarning() << "syncHelper interface:" << syncHelperInter.lastError();
        return false;
    }
    QDBusReply<QString> retUOSID = syncHelperInter.call("UOSID");
    QString uosid;
    if (retUOSID.error().message().isEmpty()) {
        uosid = retUOSID.value();
        qDebug() << "UOSID:" << uosid;
    } else {
        qDebug() << "UOSID failed:" << retUOSID.error().message();
        return false;
    }

    QDBusReply<QString> retLocalBindCheck= syncHelperInter.call("LocalBindCheck", uosid, m_uuid);
    if (retLocalBindCheck.error().message().isEmpty()) {
        m_ubid = retLocalBindCheck.value();
        qDebug() << "isBinded:" << m_ubid;
    } else {
        qDebug() << "isBinded failed:" << retLocalBindCheck.error().message();
        return false;
    }

    QDBusReply<int> retResetCaptcha = syncHelperInter.call("SendResetCaptcha", m_ubid, m_phoneEmailEdit->text());
    if (retResetCaptcha.error().message().isEmpty()) {
        m_count = retResetCaptcha.value();
        qDebug() << "SendResetCaptcha success:" << m_count;
    } else {
        qDebug() << "SendResetCaptcha failed:" << retResetCaptcha.error().message();
        int code = parseError(retResetCaptcha.error().message());
        if (code == UNREGISTERED) {
            m_phoneEmailEdit->setAlert(true);
            m_phoneEmailEdit->showAlertMessage(tr("Phone/Email not registered!"), m_repeatPasswordEdit, 2000);
        } else {
            DMessageManager::instance()->sendMessage(this, style()->standardIcon(QStyle::SP_MessageBoxWarning),
                                                     tr("request verification code failed!"));
        }
        return false;
    }
    return true;
}

bool ResetPasswordDialog::verifyVerficationCode()
{
    QDBusInterface syncHelperInter("com.deepin.sync.Helper",
                                   "/com/deepin/sync/Helper",
                                   "com.deepin.sync.Helper",
                                   QDBusConnection::systemBus());
    if (!syncHelperInter.isValid()) {
        qWarning() << "syncHelper interface:" << syncHelperInter.lastError();
        return false;
    }
    QDBusReply<bool> retVerifyResetCaptcha = syncHelperInter.call("VerifyResetCaptcha",
                                                                  m_ubid,
                                                                  m_phoneEmailEdit->text(),
                                                                  m_verificationCodeEdit->text());
    bool ret = false;
    if (retVerifyResetCaptcha.error().message().isEmpty()) {
        ret = retVerifyResetCaptcha.value();
        qDebug() << "VerifyResetCaptcha success";
    } else {
        qWarning() << "phoneEmail:" << m_phoneEmailEdit->text();
        qWarning() << "verification code:" << m_verificationCodeEdit->text();
        qWarning() << "VerifyResetCaptcha failed:" << retVerifyResetCaptcha.error().message();
    }
    return ret;
}

void ResetPasswordDialog::startCount()
{
    if (m_count == 1) {
        m_timer->stop();
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
    m_resetPasswordFloatingMessage->setMessage(tr("Successfully reset, please log in and unlock with the new password!"));
    m_resetPasswordFloatingMessage->setDuration(2000);
    DMessageManager::instance()->sendMessage(this, m_resetPasswordFloatingMessage);
    QTimer::singleShot(3000, [this]{ this->close(); });
}

void ResetPasswordDialog::onReadFromServerChanged(int fd)
{
    if(fd != filein.handle())
        return;
    char buffer[1024];
    read(filein.handle(), buffer, 1024);
    QString content = buffer;
    if (content == "success") {
        quit();
    } else {
        DMessageManager::instance()->sendMessage(this, QIcon::fromTheme("dialog-warning"), content);
    }
}

void ResetPasswordDialog::startMonitor()
{
    QRect deskRt = QApplication::desktop()->screenGeometry(QCursor::pos());
    int x = deskRt.left() + (deskRt.width() - width()) / 2;
    int y = deskRt.top() + (deskRt.height() - height()) / 2;
    move(x, y);
}

Manager::Manager(QString uuid, QString app)
    : QObject()
    , m_dialog(nullptr)
    , m_uuid(uuid)
    , m_app(app)
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
            if (!m_dialog){
                m_dialog = new ResetPasswordDialog(realRect, m_uuid, m_app);
            }
            else
                m_dialog->setScreenGeometry(realRect);

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
