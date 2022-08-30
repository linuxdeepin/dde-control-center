// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/tipsitem.h"
#include "widgets/labels/normallabel.h"

#include <QVBoxLayout>

namespace dcc {

namespace widgets {

TipsItem::TipsItem(QWidget *parent)
    : SettingsItem(parent),

      m_tipsLabel(new NormalLabel)
{
    m_tipsLabel->setAlignment(Qt::AlignCenter);
    m_tipsLabel->setWordWrap(true);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_tipsLabel);
    centralLayout->setContentsMargins(15, 0, 15, 0);

    setLayout(centralLayout);
}

void TipsItem::setText(const QString &txt)
{
    m_tipsLabel->setText(txt);
}

}

}
