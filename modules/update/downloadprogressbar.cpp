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

#include "downloadprogressbar.h"

#include <QMouseEvent>
#include <QHBoxLayout>

#include "labels/normallabel.h"

namespace dcc{
namespace update{

DownloadProgressBar::DownloadProgressBar(QWidget* parent)
    :QProgressBar(parent),
      m_message(new dcc::widgets::NormalLabel)
{
    setRange(0, 100);
    setValue(maximum());
    setTextVisible(false);
    setFixedHeight(36);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addStretch();
    layout->addWidget(m_message, 0, Qt::AlignVCenter);
    layout->addStretch();

    setLayout(layout);
}

void DownloadProgressBar::setMessage(const QString &message)
{
    m_message->setText(message);
}

void DownloadProgressBar::mouseReleaseEvent(QMouseEvent *e)
{
    e->accept();
    emit clicked();

    QProgressBar::mouseReleaseEvent(e);
}

}
}
