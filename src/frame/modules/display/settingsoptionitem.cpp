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

#include "settingsoptionitem.h"
#include "widgets/basiclistdelegate.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QPen>
#include <QPainterPath>

namespace dcc {

namespace display {

SettingsOptionItem::SettingsOptionItem(QWidget *parent)
    : QFrame(parent),

      m_title(new QLabel),
      m_icon(new QLabel)
{
    m_icon->setPixmap(loadPixmap(":/display/themes/common/icon/list_select.png"));
    m_icon->setVisible(false);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_title);
    mainLayout->addStretch();
    mainLayout->addWidget(m_icon);
    mainLayout->setSpacing(0);

    mainLayout->setContentsMargins(15, 0, 10, 0);

    setFixedHeight(36);

    setLayout(mainLayout);
}

void SettingsOptionItem::setTitle(const QString &title)
{
    m_title->setText(title);
}

void SettingsOptionItem::setSelected(const bool checked)
{
    m_icon->setVisible(checked);
}

void SettingsOptionItem::mouseReleaseEvent(QMouseEvent *e)
{
    Q_EMIT clicked();

    QFrame::mouseReleaseEvent(e);
}

} // namespace display

} // namespace dcc
