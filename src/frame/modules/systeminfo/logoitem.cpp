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

#include "logoitem.h"
#include "widgets/basiclistdelegate.h"

#include <QHBoxLayout>

#include "widgets/labels/tipslabel.h"
#include "../frame/frame.h"

namespace dcc{
namespace systeminfo {

LogoItem::LogoItem(QFrame *parent)
    : SettingsItem(parent)
{
    m_logo = new QLabel;
    m_description = new TipsLabel;
    m_description->setWordWrap(true);
    m_description->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    m_description->setMinimumWidth(FRAME_WIDTH - NAVBAR_WIDTH - 20);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(m_logo);
    layout->addWidget(m_description);
    layout->setSpacing(15);
    layout->setContentsMargins(0, 25, 0, 20);

    layout->setAlignment(m_logo, Qt::AlignCenter);
    layout->setAlignment(m_description, Qt::AlignCenter);
    setLayout(layout);
}

void LogoItem::setDescription(const QString &des)
{
    m_description->setText(des);
    QRect r = m_description->fontMetrics().boundingRect(QRect(0, 0, m_description->minimumWidth(), 100), Qt::TextWordWrap, des);
    m_description->setMinimumHeight(r.height());
}

void LogoItem::setLogo(const QString &logo)
{
    m_logo->setPixmap(loadPixmap(logo));
}

}
}
