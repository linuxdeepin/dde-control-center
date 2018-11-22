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

#include "useroptionitem.h"

#include <QLabel>
#include <QDebug>
#include <QUrl>
#include <QHBoxLayout>
#include <QPainter>

using namespace dcc::widgets;
using namespace dcc::accounts;

UserOptionItem::UserOptionItem(QFrame *parent)
    : widgets::NextPageWidget(parent),

      m_avatarLabel(new QLabel)
{
    m_avatarLabel->setFixedSize(24, 24);

    QHBoxLayout *mainLayout = static_cast<QHBoxLayout *>(layout());
    mainLayout->insertWidget(0, m_avatarLabel);
    mainLayout->setSpacing(10);
}

void UserOptionItem::setAvatar(const QString &avatar)
{
    const QSize s = m_avatarLabel->size() * devicePixelRatioF();

    const QUrl url(avatar);
    QPixmap pixmap = QPixmap(url.toLocalFile()).scaled(s, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap pic(s);
    pic.fill(Qt::transparent);

    QPainterPath painterPath;
    painterPath.addEllipse(QRect(0, 0, pic.width(), pic.height()));

    QPainter painter(&pic);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipPath(painterPath);

    painter.drawPixmap(0, 0, pixmap);
    pic.setDevicePixelRatio(devicePixelRatioF());

    m_avatarLabel->setPixmap(pic);
}
