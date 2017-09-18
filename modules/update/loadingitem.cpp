/*
 * Copyright (C) 2016 ~ 2017 Deepin Technology Co., Ltd.
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

#include "loadingitem.h"

#include <QVBoxLayout>
#include <QTimer>

#include "loadingindicator.h"
#include "labels/normallabel.h"

using namespace dcc::widgets;

namespace dcc {
namespace update {

LoadingItem::LoadingItem(QFrame *parent)
    : SettingsItem(parent),
      m_messageLabel(new NormalLabel),
      m_indicator(new LoadingIndicator)
{
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addStretch();
    layout->addWidget(m_indicator, 0, Qt::AlignHCenter);
    layout->addWidget(m_messageLabel, 0, Qt::AlignHCenter);
    layout->addStretch();

    setLayout(layout);
    setFixedHeight(100);

    QTimer::singleShot(0, m_indicator, &LoadingIndicator::play);
}

void LoadingItem::setIndicatorVisible(bool visible)
{
    m_indicator->setVisible(visible);
}

void LoadingItem::setMessage(const QString &message)
{
    m_messageLabel->setText(message);
}

} // namespace widgets
} // namespace dcc
