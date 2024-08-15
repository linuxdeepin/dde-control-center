//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "privacypolicywidget.h"
#include "widgets/titlelabel.h"

#include <DFontSizeManager>
#include <DTipLabel>
#include <DSysInfo>

#include <QVBoxLayout>
#include <QDesktopServices>
#include <QUrl>


DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
using namespace DCC_NAMESPACE;

static bool useDeepinLicense () {
    static bool useDeepinLicense = DSysInfo::productType() != DSysInfo::ProductType::Uos;
    return useDeepinLicense;
}

PrivacyPolicyWidget::PrivacyPolicyWidget(QWidget *parent)
    : QWidget(parent)
{
    setAccessibleName("PrivacyPolicyWidget");
    QFrame *widget = new QFrame(this);
    auto layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->addSpacing(20);

    TitleLabel *labelOutput = new TitleLabel(tr("Privacy Policy"),this);
    DFontSizeManager::instance()->bind(labelOutput, DFontSizeManager::T5, QFont::DemiBold);
    layout->addWidget(labelOutput, 0, Qt::AlignTop | Qt::AlignHCenter);
    layout->addSpacing(20);

    QString text;
    QString http = useDeepinLicense() ? tr("https://www.deepin.org/en/agreement/privacy/") : tr("https://www.uniontech.com/agreement/privacy-en");
    text = tr("<p>We are deeply aware of the importance of your personal information to you. So we have the Privacy Policy that covers how we collect, use, share, transfer, publicly disclose, and store your information.</p>"
              "<p>You can <a href=\"%1\">click here</a> to view our latest privacy policy and/or view it online by visiting <a href=\"%1\"> %1</a>. Please read carefully and fully understand our practices on customer privacy. If you have any questions, please contact us at: support@uniontech.com.</p>")
               .arg(http);

    DTipLabel *label = new DTipLabel(text,this);

    label->setTextFormat(Qt::RichText);
    label->setAlignment(Qt::AlignJustify | Qt::AlignLeft);
    label->setWordWrap(true);
    QObject::connect(label, &QLabel::linkActivated, this, &PrivacyPolicyWidget::onLinkActivated);

    layout->addWidget(label);
    widget->setLayout(layout);
    setLayout(layout);
    setContentsMargins(0, 8, 0, 8);
}

void PrivacyPolicyWidget::onLinkActivated(const QString link)
{
    QDesktopServices::openUrl(QUrl(link));
}
