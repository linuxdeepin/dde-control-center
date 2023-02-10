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

#include <dhidpihelper.h>

#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

ResultItem::ResultItem(QFrame *parent)
    : SettingsItem(parent)
    , m_message(new QLabel)
    , m_icon(new QLabel)
    , m_pix("")
{
    m_icon->setFixedSize(128, 128);

    m_message->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(15);
    layout->addWidget(m_icon, 0, Qt::AlignHCenter);
    layout->addSpacing(15);
    layout->addWidget(m_message, 0, Qt::AlignHCenter);
    layout->addSpacing(15);

    setLayout(layout);
}

void ResultItem::setSuccess(ShowStatus type)
{
    switch (type) {
    case ShowStatus::NoActive:
        m_pix = ":/icons/deepin/builtin/icons/noactive.svg";
        m_icon->setPixmap(DHiDPIHelper::loadNxPixmap(m_pix));
        setMessage(tr("Your system is not authorized, please activate first"));
        break;
    case ShowStatus::IsSuccessed:
        m_pix = ":/icons/deepin/builtin/icons/success.svg";
        m_icon->setPixmap(DHiDPIHelper::loadNxPixmap(m_pix));
        setMessage(tr("Update successful"));
        break;
    case ShowStatus::IsFailed:
        m_pix = ":/icons/deepin/builtin/icons/failed.svg";
        m_icon->setPixmap(DHiDPIHelper::loadNxPixmap(m_pix));
        setMessage(tr("Failed to update"));
        break;
    default:
        qDebug() << "unknown status!!!";
        break;
    }
}

void ResultItem::setMessage(const QString &message)
{
    m_message->setText(message);
}
