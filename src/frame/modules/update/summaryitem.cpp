/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#include "summaryitem.h"

#include <QVBoxLayout>

#include "widgets/labels/normallabel.h"
#include "widgets/labels/tipslabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace update {

SummaryItem::SummaryItem(QFrame *parent)
    :SettingsItem(parent),
      m_title(new NormalLabel),
      m_details(new TipsLabel)
{
    setFixedHeight(36 * 3);

    m_title->setObjectName("UpdateSummary");

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    m_title->setAlignment(Qt::AlignHCenter);
    m_title->setWordWrap(true);
    m_details->setAlignment(Qt::AlignHCenter);

    layout->addStretch();
    layout->addWidget(m_title);
    layout->addSpacing(4);
    layout->addWidget(m_details);
    layout->addStretch();

    setLayout(layout);
}

void SummaryItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void SummaryItem::setDetails(const QString &details)
{
    m_details->setText(details);
}

}
}
