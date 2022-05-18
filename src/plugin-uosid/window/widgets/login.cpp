/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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
#include "utils.h"
#include "widgets/utils.h"

#include <DSuggestButton>
#include <DFontSizeManager>
#include <DTipLabel>
#include <DSysInfo>

#include <QProcess>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <DCommandLinkButton>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

QString systemName() {
    if (IsProfessionalSystem)
        return "UOS";

    return "Deepin";
}

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    DSuggestButton *loginBtn = new DSuggestButton(tr("Sign In"));
    loginBtn->setMinimumWidth(302);
    DTipLabel *tip = new DTipLabel(tr("Manage your Union ID and sync system settings between devices"));
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignCenter);

    m_mainLayout->addStretch(3);
    QLabel *pLogo = new QLabel;
    pLogo->setPixmap(QIcon::fromTheme("dcc_cloud").pixmap(128, 100));
    m_mainLayout->addWidget(pLogo, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(10);
    QLabel *signintip = nullptr;
    if (DSysInfo::isCommunityEdition())
        signintip = new QLabel(tr("Deepin ID Sign In"));
    else
        signintip = new QLabel(tr("Union ID Sign In"));
    QFont font = signintip->font();
    font.setBold(true);
    signintip->setFont(font);
    DFontSizeManager::instance()->bind(signintip, DFontSizeManager::T3);

    m_mainLayout->addWidget(signintip, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(6);
    m_mainLayout->addWidget(tip);
    m_mainLayout->addSpacing(35);
    m_mainLayout->addWidget(loginBtn, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(20);
    DTipLabel *sps = new DTipLabel(tr("UOS Cloud is only available in Mainland China currently"));
    m_mainLayout->addWidget(sps, 0, Qt::AlignHCenter);
    m_mainLayout->addStretch(5);

    // 底部按钮
    if (IsProfessionalSystem) {
        QStringList agreementHttp = getAgreementHttp();
        QHBoxLayout *agreementLayout = new QHBoxLayout;

        QString text = tr("Learn about");
        DTipLabel *label = new DTipLabel(text, this);
        label->setWordWrap(true);
        DCommandLinkButton *useAgreementBtn = new DCommandLinkButton(tr("Union ID Service Agreement"));
        DCommandLinkButton *privacyAgreementBtn = new DCommandLinkButton(tr("Union ID Service Privacy Policy"));

        agreementLayout->addStretch();
        agreementLayout->addWidget(label, 0, Qt::AlignCenter);
        agreementLayout->addWidget(useAgreementBtn, 0, Qt::AlignCenter);
        agreementLayout->addWidget(privacyAgreementBtn, 0, Qt::AlignCenter);
        agreementLayout->addStretch();

        DFontSizeManager::instance()->bind(useAgreementBtn, DFontSizeManager::SizeType::T8);
        DFontSizeManager::instance()->bind(privacyAgreementBtn, DFontSizeManager::SizeType::T8);

        m_mainLayout->addLayout(agreementLayout);
        m_mainLayout->addSpacing(10);

        connect(useAgreementBtn, &QPushButton::clicked, [=](){
            onLinkActivated(agreementHttp.at(0));
        });
        connect(privacyAgreementBtn, &QPushButton::clicked, [=](){
            onLinkActivated(agreementHttp.at(1));
        });
    }
    setBackgroundRole(QPalette::Base);

    setLayout(m_mainLayout);
    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::requestLoginUser);
}

QStringList LoginPage::getAgreementHttp()
{
    const QString& locale { QLocale::system().name() };
    QString useAgreement;
    QString privacyAgreement;
    QStringList agreement;
    if (locale == "zh_CN") {
        useAgreement = "https://www.uniontech.com/agreement/agreement-cn";
        privacyAgreement = "https://www.uniontech.com/agreement/id-privacy-cn";
    } else if (locale == "zh_HK") {
        useAgreement = "https://www.uniontech.com/agreement/agreement-hk";
        privacyAgreement = "https://www.uniontech.com/agreement/id-privacy-hk";
    } else if (locale == "zh_TW") {
        useAgreement = "https://www.uniontech.com/agreement/agreement-tw";
        privacyAgreement = "https://www.uniontech.com/agreement/id-privacy-tw";
    } else if (locale == "ug_CN") {
        useAgreement = "https://www.uniontech.com/agreement/agreement-uy";
        privacyAgreement = "https://www.uniontech.com/agreement/id-privacy-uy";
    } else if (locale == "bo_CN") {
        useAgreement = "https://www.uniontech.com/agreement/agreement-ti";
        privacyAgreement = "https://www.uniontech.com/agreement/id-privacy-ti";
    } else {
        useAgreement = "https://www.uniontech.com/agreement/agreement-en";
        privacyAgreement = "https://www.uniontech.com/agreement/id-privacy-en";
    }

    agreement.append(useAgreement);
    agreement.append(privacyAgreement);
    return agreement;
}

void LoginPage::onLinkActivated(const QString link)
{
    QDesktopServices::openUrl(QUrl(link));
}


