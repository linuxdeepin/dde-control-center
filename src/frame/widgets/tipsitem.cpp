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
