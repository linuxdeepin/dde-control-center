/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liuxueming <liuxueming@uniontech.com>
 *
 * Maintainer: liuxueming <liuxueming@uniontech.com>
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

#include "login.h"

#include <DSuggestButton>
#include <DFontSizeManager>
#include <DTipLabel>
#include <DSysInfo>
#include <DSuggestButton>
#include <DFontSizeManager>

#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDBusInterface>

#include "../../../protocolfile.h"
#include "../../../mainwindow.h"
#include "../define.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

#include "window/utils.h"
namespace DCC_NAMESPACE {
namespace unionid {
QString systemName()
{
    if (SystemTypeName == "Professional")
        return "UOS";

    return "Deepin";
}
}
}
LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_bIsLoginActived = false;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    m_iconLabel = new QLabel;

    DGuiApplicationHelper::ColorType type  = DGuiApplicationHelper::instance()->themeType();

    m_iconLabel->setFixedSize(QSize(316,202));
    m_iconLabel->setAlignment(Qt::AlignCenter);

    m_titleLabel = new QLabel;
    m_titleLabel->setText(tr("Sign in or sign up Union ID"));
    m_titleLabel->setContentsMargins(24,0,0,8);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_tipLabel = new QLabel;
    m_tipLabel->setText(tr("To get more personalized services"));
    m_tipLabel->setAlignment(Qt::AlignCenter);
    m_tipLabel->setContentsMargins(0,0,0,32);

    if (type == DGuiApplicationHelper::LightType) {
        m_iconLabel->setPixmap(QPixmap(":/themes/light/icons/background"));

        QPalette pa = m_titleLabel->palette();
        pa.setColor(QPalette::Text,textTitleLightColor);
        m_titleLabel->setPalette(pa);

        pa = m_tipLabel->palette();
        pa.setColor(QPalette::Text,textTipLightColor);
        m_tipLabel->setPalette(pa);
    } else if (type == DGuiApplicationHelper::DarkType){
        m_iconLabel->setPixmap(QPixmap(":/themes/dark/icons/background"));

        QPalette pa = m_titleLabel->palette();
        pa.setColor(QPalette::Text,textTitleDarkColor);
        m_titleLabel->setPalette(pa);

        pa = m_tipLabel->palette();
        pa.setColor(QPalette::Text,textTiptDarkColor);
        m_tipLabel->setPalette(pa);
    }

    DSuggestButton *signInButton = new DSuggestButton(tr("Sign In"));
    signInButton->setMinimumSize(QSize(72,28));
    connect(signInButton,&DSuggestButton::clicked,this,&LoginPage::onSignInButtonClicked);

    DSuggestButton *signUpButton = new DSuggestButton(tr("Sign Up"));
    signUpButton->setMinimumSize(QSize(72,28));
    connect(signUpButton,&DSuggestButton::clicked,this,&LoginPage::onSignUpButtonClicked);

    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addWidget(signInButton);
    hlayout->addSpacing(16);
    hlayout->addWidget(signUpButton);
    hlayout->setContentsMargins(256,0,254,0);

    QWidget *contentWidget = new QWidget;
    contentWidget->setMinimumSize(QSize(670,548));

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->addWidget(m_iconLabel,0,Qt::AlignCenter);
    contentLayout->addWidget(m_titleLabel,0,Qt::AlignCenter);
    contentLayout->addWidget(m_tipLabel,0,Qt::AlignCenter);
    contentLayout->addLayout(hlayout);
    contentLayout->addStretch();
    contentWidget->setLayout(contentLayout);

    DFontSizeManager::instance()->bind(m_titleLabel, DFontSizeManager::T4,QFont::Bold);
    DFontSizeManager::instance()->bind(m_tipLabel, DFontSizeManager::T6,QFont::Normal);
    DFontSizeManager::instance()->bind(signInButton, DFontSizeManager::T6,QFont::Normal);
    DFontSizeManager::instance()->bind(signUpButton, DFontSizeManager::T6,QFont::Normal);

    m_mainLayout->addWidget(contentWidget);
    m_mainLayout->setContentsMargins(0,49,0,0);;
    setLayout(m_mainLayout);

    connect(DGuiApplicationHelper::instance(),&DGuiApplicationHelper::themeTypeChanged,this,&LoginPage::onThemeTypeChanged);
}

void LoginPage::setMainWindow(MainWindow *pMainWindow)
{
    m_pMainWindow = pMainWindow;
}

void LoginPage::login()
{
    onSignInButtonClicked();
}

void LoginPage::licenceDialog()
{
//    if (m_bIsLoginActived) {
//        return;
//    }

    // 打开license-dialog必要的三个参数:标题、license文件路径、checkBtn的Text
    QString title("《隐私政策》");
    QString allowContent("我已阅读并同意《隐私政策》");
    // license路径
    QString zhCN_Content = "";
    QString enUS_Content = "";

    ProtocolFile::getPrivacyFile(zhCN_Content, enUS_Content);

    m_licenceProcess = new QProcess(this);
    //添加-u和-b选项显示隐私政策页面表头和选项英文
    m_licenceProcess->start("dde-license-dialog",
                            QStringList() << "-t" << title << "-u" << "Privacy Policy"  << "-c" << zhCN_Content
                            << "-b" << "I have read and agree to the Privacy Policy"
                            << "-e" << enUS_Content << "-a" << allowContent);
    qDebug() << "Deliver content QStringList() = " << "dde-license-dialog"
             << "-t" << title << "-u" << "Privacy Policy"  << "-c" << zhCN_Content
             << "-b" << "I have read and agree to the Privacy Policy"
             << "-e" << enUS_Content << "-a" << allowContent;
    connect(m_licenceProcess, &QProcess::stateChanged, this, [this](QProcess::ProcessState state) {
        if (m_pMainWindow) {
            m_pMainWindow->setEnabled(state != QProcess::Running);
        } else {
            qDebug() << "licence dialog pMainWindow is nullptr";
        }
    });
    connect(m_licenceProcess, static_cast<void (QProcess::*)(QProcess::ProcessError)>(&QProcess::errorOccurred), this, [](QProcess::ProcessError error) {
        qDebug() << "error is " << error;
    });
    connect(m_licenceProcess, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, [=](int result) {
        if (96 == result) {
            Q_EMIT requestLoginUser();
        }
        m_licenceProcess->deleteLater();
        m_licenceProcess = nullptr;
    });
}

void LoginPage::onThemeTypeChanged(DGuiApplicationHelper::ColorType themeType)
{
    if (themeType == DGuiApplicationHelper::LightType) {
        m_iconLabel->setPixmap(QPixmap(":/themes/light/icons/background"));

        QPalette pa = m_titleLabel->palette();
        pa.setColor(QPalette::Text,textTitleLightColor);
        m_titleLabel->setPalette(pa);

        pa = m_tipLabel->palette();
        pa.setColor(QPalette::Text,textTipLightColor);
        m_tipLabel->setPalette(pa);
    } else if (themeType == DGuiApplicationHelper::DarkType){
        m_iconLabel->setPixmap(QPixmap(":/themes/dark/icons/background"));

        QPalette pa = m_titleLabel->palette();
        pa.setColor(QPalette::Text,textTitleDarkColor);
        m_titleLabel->setPalette(pa);

        pa = m_tipLabel->palette();
        pa.setColor(QPalette::Text,textTiptDarkColor);
        m_tipLabel->setPalette(pa);
    }
}

void LoginPage::onSignInButtonClicked()
{
    Q_EMIT requestLoginUser();
}

void LoginPage::onSignUpButtonClicked()
{
    qInfo() << "onSignInButtonClicked";
    QDBusInterface interface("com.deepin.deepinid.Client",
                              "/com/deepin/deepinid/Client",
                              "com.deepin.deepinid.Client");

    QList<QVariant> argumentList;
    argumentList << "388340d186f311eb983b0242ac130002";
    argumentList << "com.deepin.dde.ControlCenter";
    argumentList << "/com/deepin/dde/ControlCenter";
    argumentList << "com.deepin.dde.ControlCenter";
    interface.callWithArgumentList(QDBus::NoBlock, "Register", argumentList);

    argumentList = {};
    argumentList << "388340d186f311eb983b0242ac130002";
    argumentList << QStringList{"base","user.api:contact","user:contact:read"};
    argumentList << "https://uosvip-pre.uniontech.com/account/unionid/callback/uid-managment";
    argumentList << "state";
    interface.callWithArgumentList(QDBus::NoBlock, "createAccount", argumentList);
}
