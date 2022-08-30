// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "avatarwidget.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

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
    m_avatar = QPixmap(avatar).scaled(size() * ratio, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPainter p(&m_avatar);
    p.setRenderHints(QPainter::Antialiasing);
    p.drawPixmap(0, 0, m_avatar);
    m_avatar.setDevicePixelRatio(ratio);

    setAccessibleName(m_avatarPath);

    update();
}

void AvatarWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QRect r = rect();
    QPainterPath painterPath;
    painterPath.addEllipse(r);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipPath(painterPath);

    painter.drawPixmap(r, m_avatar);
}
