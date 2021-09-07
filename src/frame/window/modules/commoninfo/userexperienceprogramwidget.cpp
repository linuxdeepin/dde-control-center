/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "userexperienceprogramwidget.h"
#include "window/modules/commoninfo/commoninfomodel.h"
#include "window/utils.h"
#include "widgets/switchwidget.h"
#include "widgets/labels/tipslabel.h"
#include "window/utils.h"

#include <DTipLabel>
#include <QDesktopServices>

#include <QUrl>
#include <QVBoxLayout>
#include <QTimer>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace commoninfo;
DWIDGET_USE_NAMESPACE

UserExperienceProgramWidget::UserExperienceProgramWidget(QWidget *parent)
    : QWidget(parent)
{
    setAccessibleName("UserExperienceProgramWidget");
    QVBoxLayout *vBoxLayout = new QVBoxLayout;

    m_joinUeProgram = new SwitchWidget();
    m_joinUeProgram->addBackground();
    //~ contents_path /commoninfo/User Experience Program
    m_joinUeProgram->setTitle(tr("Join User Experience Program"));

    QString text = "";
    QString http = IsCommunitySystem ? tr("https://www.deepin.org/en/agreement/privacy/") : tr("https://www.uniontech.com/agreement/privacy-en");
    if (IsCommunitySystem) {
        text = tr("<p>Joining User Experience Program means that you grant and authorize us to collect and use the information of your device, system and applications. "
                  "If you refuse our collection and use of the aforementioned information, do not join User Experience Program. "
                  "For details, please refer to Deepin Privacy Policy (<a href=\"%1\"> %1</a>).</p>")
               .arg(http);
    } else {
        text = tr("<p>Joining User Experience Program means that you grant and authorize us to collect and use the information of your device, system and applications. "
                  "If you refuse our collection and use of the aforementioned information, do not join User Experience Program. "
                  "To know more about the management of your data, please refer to UnionTech OS Privacy Policy (<a href=\"%1\"> %1</a>).</p>")
               .arg(http);
    }

    DTipLabel *label = new DTipLabel(text);

    label->setTextFormat(Qt::RichText);
    label->setAlignment(Qt::AlignJustify | Qt::AlignLeft);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setWordWrap(true);
    connect(label, &QLabel::linkActivated, this, [](const QString &link) {
        QDesktopServices::openUrl(QUrl(link));
    });

    vBoxLayout->setMargin(0);
    vBoxLayout->setContentsMargins(ThirdPageContentsMargins);
    vBoxLayout->setSpacing(0);
    vBoxLayout->addWidget(m_joinUeProgram);
    vBoxLayout->addSpacing(8);
    vBoxLayout->addWidget(label);
    vBoxLayout->addStretch();

    setLayout(vBoxLayout);

    connect(m_joinUeProgram, &SwitchWidget::checkedChanged, this, [this](bool state) {
        QTimer::singleShot(0, this, [ = ] {
            this->enableUeProgram(state);
        });
    });
}

void UserExperienceProgramWidget::setModel(CommonInfoModel *model)
{
    connect(model, &CommonInfoModel::ueProgramChanged, m_joinUeProgram, &SwitchWidget::setChecked);
}

void UserExperienceProgramWidget::setDefaultUeProgram(const bool enabled)
{
    m_joinUeProgram->setChecked(enabled);
}
