// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "privacypolicywidget.h"
#include "widgets/translucentframe.h"
#include "widgets/titlelabel.h"
#include "window/utils.h"

#include <DFontSizeManager>
#include <DTipLabel>

#include <QVBoxLayout>
#include <QDesktopServices>
#include <QUrl>

using namespace dcc;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::systeminfo;

PrivacyPolicyWidget::PrivacyPolicyWidget(QWidget *parent)
    : ContentWidget(parent)
{
    setAccessibleName("PrivacyPolicyWidget");
    TranslucentFrame *widget = new TranslucentFrame;
    auto layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    layout->addSpacing(20);

    TitleLabel *labelOutput = new TitleLabel(tr("Privacy Policy"));
    DFontSizeManager::instance()->bind(labelOutput, DFontSizeManager::T5, QFont::DemiBold);
    layout->addWidget(labelOutput, 0, Qt::AlignTop | Qt::AlignHCenter);
    layout->addSpacing(20);

    QString text;
    QString http = IsCommunitySystem ? tr("https://www.deepin.org/en/agreement/privacy/") : tr("https://www.uniontech.com/agreement/privacy-en");
    text = tr("<p>We are deeply aware of the importance of your personal information to you. So we have the Privacy Policy that covers how we collect, use, share, transfer, publicly disclose, and store your information.</p>"
              "<p>You can <a href=\"%1\">click here</a> to view our latest privacy policy and/or view it online by visiting <a href=\"%1\"> %1</a>. Please read carefully and fully understand our practices on customer privacy. If you have any questions, please contact us at: support@uniontech.com.</p>")
               .arg(http);

    DTipLabel *label = new DTipLabel(text);

    label->setTextFormat(Qt::RichText);
    label->setAlignment(Qt::AlignJustify | Qt::AlignLeft);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setWordWrap(true);
    connect(label, &QLabel::linkActivated, this, &PrivacyPolicyWidget::onLinkActivated);

    layout->addWidget(label);
    widget->setLayout(layout);
    setContent(widget);
    setContentsMargins(0, 8, 0, 8);
}

void PrivacyPolicyWidget::onLinkActivated(const QString link)
{
    QDesktopServices::openUrl(QUrl(link));
}
