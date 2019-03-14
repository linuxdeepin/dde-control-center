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

#include "resultitem.h"

#include <QVBoxLayout>
#include <QLabel>
#include <dhidpihelper.h>

#include "widgets/labels/normallabel.h"
#include "widgets/basiclistdelegate.h"

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace update {

ResultItem::ResultItem(QFrame *parent)
    : SettingsItem(parent),
      m_message(new dcc::widgets::NormalLabel),
      m_icon(new QLabel)
{
    m_icon->setFixedSize(50, 50);

    m_message->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(15);
    layout->addWidget(m_icon, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(m_message, 0, Qt::AlignHCenter);
    layout->addSpacing(15);

    setLayout(layout);
}

void ResultItem::setSuccess(bool success)
{
    const QString pix = success ? ":/update/themes/common/icons/success.svg" :
                                  ":/update/themes/common/icons/failed.svg";
    const QString message = success ? tr("Updated successfully") :
                                      tr("Failed to update");
    m_icon->setPixmap(DHiDPIHelper::loadNxPixmap(pix));
    setMessage(message);
}

void ResultItem::setMessage(const QString &message)
{
    m_message->setText(message);
}

} // namespace update
} // namespace dcc
