/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     longqi <longqi@uniontech.com>
*
* Maintainer: longqi <longqi@uniontech.com>
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
#include "privacypolicywidget.h"
#include "widgets/translucentframe.h"
#include "widgets/titlelabel.h"

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
    TranslucentFrame *widget = new TranslucentFrame;
    auto layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);
    layout->addSpacing(20);

    TitleLabel *labelOutput = new TitleLabel(tr("Privacy Policy"));
    DFontSizeManager::instance()->bind(labelOutput, DFontSizeManager::T5, QFont::DemiBold);
    layout->addWidget(labelOutput, 0, Qt::AlignTop | Qt::AlignHCenter);
    layout->addSpacing(20);

    DTipLabel *label = new DTipLabel(tr("<p>We are deeply aware of the importance of your personal information to you. So we have the Privacy Policy that covers how we collect, use, share, transfer, publicly disclose, and store your information.</p>"
                                        "<p>You can <a href=\"https://www.uniontech.com/agreement/privacy-en\">click here</a> to view our latest privacy policy or view it online by visiting <a href=\"https://www.uniontech.com/agreement/privacy-en\"> https://www.uniontech.com/agreement/privacy-en</a>. Please read carefully and fully understand our practices on customer privacy. If you have any questions, please contact us at: support@uniontech.com.</p>"));

    label->setWordWrap(true);
    label->setTextFormat(Qt::RichText);
    label->setAlignment(Qt::AlignJustify);
    connect(label, &QLabel::linkActivated, this, [](const QString &link) {
        QDesktopServices::openUrl(QUrl(link));
    });

    layout->addWidget(label);
    widget->setLayout(layout);
    setContent(widget);
}
