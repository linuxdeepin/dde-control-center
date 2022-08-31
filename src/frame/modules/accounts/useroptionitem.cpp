// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "useroptionitem.h"

#include <QLabel>
#include <QDebug>
#include <QUrl>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>

using namespace dcc::widgets;
using namespace dcc::accounts;

UserOptionItem::UserOptionItem(QFrame *parent)
    : widgets::NextPageWidget(parent)
    , m_avatarLabel(new QLabel)
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
    QPixmap pixmap = QPixmap(url.toLocalFile()).scaled(s, Qt::KeepAspectRatio, Qt::FastTransformation);

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
