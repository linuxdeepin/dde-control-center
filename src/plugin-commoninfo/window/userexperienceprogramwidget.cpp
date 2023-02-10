//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "userexperienceprogramwidget.h"
#include "src/plugin-commoninfo/operation/commoninfomodel.h"
#include "src/frame/utils.h"
#include "widgets/switchwidget.h"

#include <DTipLabel>
#include <DSysInfo>

#include <QDesktopServices>
#include <QUrl>
#include <QVBoxLayout>
#include <QTimer>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE
UserExperienceProgramWidget::UserExperienceProgramWidget(QWidget *parent)
    : QWidget(parent)
{
    setAccessibleName("UserExperienceProgramWidget");
    QVBoxLayout *vBoxLayout = new QVBoxLayout;

    m_joinUeProgram = new SwitchWidget();
    m_joinUeProgram->addBackground();
    //~ contents_path /commoninfo/User Experience Program
    //~ child_page User Experience Program
    m_joinUeProgram->setTitle(tr("Join User Experience Program"));

    QString text = "";
    QString http = IS_COMMUNITY_SYSTEM ? tr("https://www.deepin.org/en/agreement/privacy/") : tr("https://www.uniontech.com/agreement/privacy-en");
    if (IS_COMMUNITY_SYSTEM) {
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
    label->setWordWrap(true);
    connect(label, &QLabel::linkActivated, this, [](const QString &link) {
        QDesktopServices::openUrl(QUrl(link));
    });

    vBoxLayout->setMargin(0);
    vBoxLayout->setContentsMargins(0, 10, 0, 10);
    vBoxLayout->setSpacing(0);
    vBoxLayout->addWidget(m_joinUeProgram);
    vBoxLayout->addSpacing(8);
    vBoxLayout->addWidget(label);
    vBoxLayout->addStretch();

    setLayout(vBoxLayout);

    connect(m_joinUeProgram, &SwitchWidget::checkedChanged, this, [this](bool state) {
        m_joinUeProgram->setEnabled(false);
        QTimer::singleShot(0, this, [ = ] {
            this->enableUeProgram(state);
        });
    });
}

void UserExperienceProgramWidget::setModel(CommonInfoModel *model)
{
    setDefaultUeProgram(model->ueProgram());
    connect(model, &CommonInfoModel::ueProgramChanged, m_joinUeProgram, [this](const bool enable) {
        m_joinUeProgram->setEnabled(true);
        m_joinUeProgram->setChecked(enable);
    });
}

void UserExperienceProgramWidget::setDefaultUeProgram(const bool enabled)
{
    m_joinUeProgram->setChecked(enabled);
}
