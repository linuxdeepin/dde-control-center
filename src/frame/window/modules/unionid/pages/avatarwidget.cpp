/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liuxueming <liuxueming@uniontech.com>
 *
 * Maintainer: liumaochuan <liumaochuan@uniontech.com>
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

#include "avatarwidget.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::unionid;

AvatarWidget::AvatarWidget(QWidget *parent)
    : QLabel(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);
    setObjectName("AvatarWidget");
}

AvatarWidget::AvatarWidget(const QString &avatar, QWidget *parent)
    : AvatarWidget(parent)
{
    setAvatarPath(avatar);
}

const QString AvatarWidget::avatarPath() const
{
    return m_avatarPath;
}

void AvatarWidget::setAvatarPath(const QString &avatar)
{
    const auto ratio = devicePixelRatioF();

    m_avatarPath = avatar;
    m_avatar = QPixmap(avatar).scaled(size() * ratio, Qt::KeepAspectRatio, Qt::FastTransformation);
    m_avatar.setDevicePixelRatio(ratio);

    setAccessibleName(m_avatarPath);

    update();
}

void AvatarWidget::paintEvent(QPaintEvent *e)
{
    //NOTE [HOME]
    Q_UNUSED(e)
    QRect r = rect();
    QPainterPath painterPath;
    painterPath.addEllipse(r);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipPath(painterPath);

    painter.drawPixmap(r, m_avatar);
}
