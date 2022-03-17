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
#include "securityquestionswidget.h"
#include "unionidwidget.h"
#include "resetpasswordworker.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include <QDebug>
#include <QCloseEvent>

#include <DButtonBox>
#include <DMessageManager>
#include <DFloatingMessage>

DGUI_USE_NAMESPACE

ResetPasswordDialog::ResetPasswordDialog(QRect screenGeometry, const QString &userName, const QString &appName, const int &fd)
    : DDialog()
    , m_isSecurityQuestionsExist(true)
    , m_screenGeometry(screenGeometry)
    , m_userName(userName)
    , m_appName(appName)
    , m_fd(fd)
    , m_monitorTimer(new QTimer(this))
    , m_isClose(true)
    , m_stackedLayout(new QStackedLayout)
    , m_resetPasswordWorker(new ResetPasswordWorker(userName, this))
{
    initWidget(userName);
    initData();
}

void ResetPasswordDialog::showEvent(QShowEvent *event)
{
    DDialog::showEvent(event);

    move(m_screenGeometry.center() - rect().center());
}

void ResetPasswordDialog::hideEvent(QHideEvent *event)
{
    DDialog::hideEvent(event);
    auto isWayland = qEnvironmentVariable("XDG_SESSION_TYPE").contains("wayland");
    if (isWayland && m_isClose) {
        if (m_appName == "greeter" || m_appName == "lock") {
            m_client->write("close");
            m_client->flush();
        }
        this->close();
        qApp->quit();
    }
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

void ResetPasswordDialog::initWidget(const QString &userName)
{
    this->setMinimumSize(380, 590);
    this->setAccessibleName("ResetPasswordPage");
    this->setMessage(tr("Resetting the password will clear the data stored in the keyring"));
    this->setIcon(QIcon::fromTheme("dialog-warning"));
    this->addButton(tr("Cancel"));
    this->setOnButtonClickedClose(false);

    QWidget *mainContentWidget = new QWidget;
    QVBoxLayout *mainContentLayout = new QVBoxLayout(mainContentWidget);
    mainContentLayout->setSpacing(0);
    mainContentLayout->setMargin(0);

    m_buttonBox = new DButtonBox(this);
    m_buttonBox->setButtonList({new DButtonBoxButton(tr("Security Questions")), new DButtonBoxButton("Union ID")}, true);
    m_buttonBox->setId(m_buttonBox->buttonList().at(0), 0);
    m_buttonBox->setId(m_buttonBox->buttonList().at(1), 1);
    m_buttonBox->buttonList().at(0)->click();
    mainContentLayout->addSpacing(16);
    mainContentLayout->addWidget(m_buttonBox, 0, Qt::AlignHCenter);

    connect(this, &ResetPasswordDialog::requestSecurityQuestions, m_resetPasswordWorker, &ResetPasswordWorker::getSecurityQuestions);
    connect(m_resetPasswordWorker, &ResetPasswordWorker::getSecurityQuestionsReplied, this, &ResetPasswordDialog::onGetSecurityQuestionsReplied);
    Q_EMIT requestSecurityQuestions();

    m_SecurityQuestionsWidget = new SecurityQuestionsWidget(m_securityQuestions, userName, mainContentWidget);
    connect(m_SecurityQuestionsWidget, &SecurityQuestionsWidget::requestSetPasswordHint, m_resetPasswordWorker, &ResetPasswordWorker::setPasswordHint);
    connect(m_SecurityQuestionsWidget, &SecurityQuestionsWidget::requestVerifySecretQuestions, m_resetPasswordWorker, &ResetPasswordWorker::verifySecretQuestions);
    connect(m_resetPasswordWorker, &ResetPasswordWorker::verifySecretQuestionsReplied, m_SecurityQuestionsWidget, &SecurityQuestionsWidget::onVerifySecretQuestionsReplied);

    m_UnionIDWidget = new UnionIDWidget(m_userPath, userName, mainContentWidget);
    connect(m_UnionIDWidget, &UnionIDWidget::requestAsyncBindCheck, m_resetPasswordWorker, &ResetPasswordWorker::asyncBindCheck);
    connect(m_UnionIDWidget, &UnionIDWidget::requestAsyncVerficationCode, m_resetPasswordWorker, &ResetPasswordWorker::asyncRequestVerficationCode);
    connect(m_UnionIDWidget, &UnionIDWidget::requestVerifyVerficationCode, m_resetPasswordWorker, &ResetPasswordWorker::verifyVerficationCode);
    connect(m_resetPasswordWorker, &ResetPasswordWorker::requestBindCheckUbidReplied, m_UnionIDWidget, &UnionIDWidget::onBindCheckUbidReplied);
    connect(m_resetPasswordWorker, &ResetPasswordWorker::requestBindCheckReplied, m_UnionIDWidget, &UnionIDWidget::onBindCheckReplied);
    connect(m_resetPasswordWorker, &ResetPasswordWorker::requestVerficationCodeCountReplied, m_UnionIDWidget, &UnionIDWidget::onVerficationCodeCountReplied);
    connect(m_resetPasswordWorker, &ResetPasswordWorker::requestVerficationCodeReplied, m_UnionIDWidget, &UnionIDWidget::onRequestVerficationCodeReplied);
    connect(m_resetPasswordWorker, &ResetPasswordWorker::requestVerifyVerficationCodeReplied, m_UnionIDWidget, &UnionIDWidget::onRequestVerifyVerficationCodeReplied);

    m_stackedLayout->setSpacing(0);
    m_stackedLayout->setMargin(0);
    m_stackedLayout->addWidget(m_SecurityQuestionsWidget);
    m_stackedLayout->addWidget(m_UnionIDWidget);
    mainContentLayout->addLayout(m_stackedLayout);

    this->insertContent(0, mainContentWidget);

    if (!m_securityQuestions.isEmpty()) {
        this->setTitle(tr("Reset Password"));
        this->addButton(tr("Next"), true, ButtonRecommend);
        m_stackedLayout->setCurrentIndex(0);
    } else {
        this->setTitle(tr("Reset Password By Union ID"));
        this->addButton(tr("Reset"), true, ButtonRecommend);
        m_buttonBox->hide();
        m_stackedLayout->setCurrentIndex(1);
        m_UnionIDWidget->loadPage();
    }

    m_tipDialog.setMessage(tr("Resetting the password will clear the data stored in your keyring, and you should log in again, please save files in advance"));
    m_tipDialog.addButtons(QStringList() << tr("Cancel") << tr("Confirm and Reset"));
    m_tipDialog.setIcon(QIcon::fromTheme("dialog-warning"));
    auto isWayland = qEnvironmentVariable("XDG_SESSION_TYPE").contains("wayland");
    if (isWayland) {
        m_tipDialog.setWindowFlags(Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowStaysOnTopHint);
    } else {
        m_tipDialog.setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowStaysOnTopHint);
    }
    m_tipDialog.installEventFilter(this);
    m_tipDialog.setFixedSize(380, 189);
    m_tipDialog.setOnButtonClickedClose(false);

    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint | Qt::WindowStaysOnTopHint);
}

void ResetPasswordDialog::initData()
{
    filein.open(stdin, QIODevice::ReadOnly);
    QSocketNotifier* sn = new QSocketNotifier(filein.handle(), QSocketNotifier::Read, this);
    sn->setEnabled(true);
    connect(m_buttonBox, &DButtonBox::buttonClicked, this, [this](QAbstractButton *button) {
        if (m_buttonBox->id(button) == 0) {
            m_stackedLayout->setCurrentIndex(0);
            if (buttonCount() == 1) {
                this->addButton(m_SecurityQuestionsWidget->isAnswerRight() ? tr("Reset") : tr("Next"));
                connect(getButton(1), &QPushButton::clicked, this, &ResetPasswordDialog::onResetPasswordBtnClicked);
            } else {
                getButton(1)->setText(m_SecurityQuestionsWidget->isAnswerRight() ? tr("Reset") : tr("Next"));
            }
        } else {
            m_stackedLayout->setCurrentIndex(1);
            m_UnionIDWidget->loadPage();
        }
    });
    connect(m_SecurityQuestionsWidget, &SecurityQuestionsWidget::answersRight, this, [this] {
        getButton(1)->setText(tr("Reset"));
    });
    connect(m_UnionIDWidget, &UnionIDWidget::pageChanged, this, [this](bool isResetPasswordPage) {
        this->clearButtons();
        this->addButton(tr("Cancel"));
        connect(getButton(0), &QPushButton::clicked, this, &ResetPasswordDialog::onCancelBtnClicked);
        if (isResetPasswordPage) {
            this->addButton(tr("Reset"));
            connect(getButton(1), &QPushButton::clicked, this, &ResetPasswordDialog::onResetPasswordBtnClicked);
        }
    });
    connect(sn, SIGNAL(activated(int)), this, SLOT(onReadFromServerChanged(int)));
    connect(m_monitorTimer, &QTimer::timeout, this, &ResetPasswordDialog::updatePosition);
    connect(getButton(0), &QPushButton::clicked, this, &ResetPasswordDialog::onCancelBtnClicked);
    connect(getButton(1), &QPushButton::clicked, this, &ResetPasswordDialog::onResetPasswordBtnClicked);
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
            file.write((cryptUserPassword(getPassword())+ "\n").toStdString().c_str());
            file.flush();
            file.close();
        }
    });
    if (m_appName == "greeter" || m_appName == "lock") {
        m_monitorTimer->start(300);
        m_client = new QLocalSocket(this);
        m_client->abort();
        m_client->connectToServer("GrabKeyboard_" + m_appName);
        if(!m_client->waitForConnected(1000)) {
            qWarning() << "connect failed, server: " << "GrabKeyboard_" + m_appName << ", error: " << m_client->errorString();
            return;
        }
    }
}

QRect ResetPasswordDialog::screenGeometry() const
{
    return m_screenGeometry;
}

void ResetPasswordDialog::setScreenGeometry(const QRect &screenGeometry)
{
    m_screenGeometry = screenGeometry;
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

const QString ResetPasswordDialog::getPassword()
{
    return (m_buttonBox->checkedId() == 0) ? m_SecurityQuestionsWidget->getPassword():
                                             m_UnionIDWidget->getPassword();
}

void ResetPasswordDialog::onCancelBtnClicked()
{
    if (m_appName == "greeter" || m_appName == "lock") {
        m_client->write("close");
        m_client->flush();
    }
    this->close();
    qApp->quit();
}

void ResetPasswordDialog::onResetPasswordBtnClicked()
{
    bool success = false;
    if (m_buttonBox && m_buttonBox->checkedId() == 0) {
        success = m_SecurityQuestionsWidget->onResetPasswordBtnClicked();
    } else {
        success = m_UnionIDWidget->onResetPasswordBtnClicked();
    }
    qWarning() << "onResetPassowrdBtnClicked:" << success;
    if (success) {
        m_isClose = false;
        updatePosition();
        m_tipDialog.show();
        this->hide();
    }
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

void ResetPasswordDialog::onGetSecurityQuestionsReplied(const QList<int> securityQuestions)
{
    m_securityQuestions = securityQuestions;
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
