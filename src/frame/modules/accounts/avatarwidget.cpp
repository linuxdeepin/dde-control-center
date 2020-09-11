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

#include "avatarwidget.h"

#include <QDebug>
#include <QUrl>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QApplication>
#include <QRect>
#include <QColor>

using namespace dcc::accounts;

AvatarWidget::AvatarWidget(QWidget *parent)
    : QLabel(parent)
    , m_hover(false)
    , m_deleable(false)
    , m_selected(false)
    , m_arrowed(false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);
    setFixedSize(PIX_SIZE, PIX_SIZE);
    setObjectName("AvatarWidget");
}

AvatarWidget::AvatarWidget(const QString &avatar, QWidget *parent)
    : AvatarWidget(parent)
{
    setAvatarPath(avatar);
}

void AvatarWidget::setSelected(const bool selected)
{
    m_selected = selected;
    update();
}

void AvatarWidget::setDeletable(const bool deletable)
{
    m_deleable = deletable;
    update();
}

void AvatarWidget::setArrowed(const bool arrowed)
{
    m_arrowed = arrowed;
    update();
}

const QString AvatarWidget::avatarPath() const
{
    const auto ratio = devicePixelRatioF();

    if (ratio > 1.0)
        return QString(m_avatarPath).replace("icons/bigger/", "icons/");

    return m_avatarPath;
}

void AvatarWidget::setAvatarPath(const QString &avatar)
{
    const auto ratio = devicePixelRatioF();

    QString avatarPath = avatar;
    if (ratio > 1.0)
        avatarPath.replace("icons/", "icons/bigger/");

    QUrl url(avatarPath);
    if (!QFile(url.toLocalFile()).exists())
        url = QUrl(avatar);

    m_avatarPath = url.toString();

    if (!QPixmap(url.toLocalFile()).isNull()) {
        m_avatar = QPixmap(url.toLocalFile()).scaled(size() * ratio, Qt::KeepAspectRatio, Qt::FastTransformation);
        m_avatar.setDevicePixelRatio(ratio);
    }

    setAccessibleName(m_avatarPath);

    update();
}

void AvatarWidget::mouseReleaseEvent(QMouseEvent *e)
{
    if (rect().contains(e->pos()))
        Q_EMIT clicked(avatarPath());

    QWidget::mouseReleaseEvent(e);
}

void AvatarWidget::paintEvent(QPaintEvent *e)
{
    QPainterPath painterPath;
    painterPath.addEllipse(QRect(0, 0, width(), height()));

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setClipPath(painterPath);

    painter.drawPixmap(e->rect(), m_avatar);

    if (m_selected) {
        setAccessibleDescription("selectedIcon");
        QPen pen(Qt::transparent);
        pen.setWidth(4);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.setBrush(Qt::transparent);
        painter.drawEllipse(rect());
    };

    //当鼠标移动到图像上面
    if (m_hover) {
        painter.setPen(Qt::NoPen);
        //宽高
        int w = this->rect().width();
        int h = this->rect().height();
        //矩形
        QRect rect(0, h - h / 4, w, h - h / 4);
        // 反走样
        painter.setRenderHint(QPainter::Antialiasing, true);
        // 设置渐变色
        QLinearGradient linear(QPoint(0, h - h / 4), QPoint(0, h));
        linear.setColorAt(0, QColor(0, 0, 0, 0.00 * 255));
        linear.setColorAt(1, QColor(0, 0, 0, 0.50 * 255));

        // 设置显示模式
        linear.setSpread(QGradient::PadSpread);
        painter.setBrush(linear);
        painter.drawRect(rect);
    }

    if (!m_arrowed) {
        QPen pen(Qt::transparent);
        pen.setWidth(2);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        //把直径平均分成10份
        int portion = this->rect().width() / 10;
        //圆中心点坐标
        QPoint cpt = this->rect().center();
        //绘制左边直线
        painter.drawLine(QPoint(cpt.x() - portion / 2, cpt.y() + portion * 4 - portion / 2),
                         QPoint(cpt.x(), cpt.y() + portion * 4));
        //绘制右边直线
        painter.drawLine(QPoint(cpt.x() + portion / 2, cpt.y() + portion * 4 - portion / 2),
                         QPoint(cpt.x(), cpt.y() + portion * 4));
    } else {
        QPen pen(Qt::transparent);
        pen.setWidth(2);
        pen.setColor(Qt::white);
        painter.setPen(pen);
        //把直径平均分成10份
        int portion = this->rect().width() / 10;
        //圆中心点坐标
        QPoint cpt = this->rect().center();
        //绘制左边直线
        painter.drawLine(QPoint(cpt.x() - portion / 2, cpt.y() + portion * 4),
                         QPoint(cpt.x(), cpt.y() + portion * 4 - portion / 2));
        //绘制右边直线
        painter.drawLine(QPoint(cpt.x() + portion / 2, cpt.y() + portion * 4),
                         QPoint(cpt.x(), cpt.y() + portion * 4 - portion / 2));
    }

    QWidget::paintEvent(e);
}

void AvatarWidget::enterEvent(QEvent *)
{
    m_hover = true;
    update();
}

void AvatarWidget::leaveEvent(QEvent *)
{
    m_hover = false;
    update();
}

void AvatarWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    const auto ratio = devicePixelRatioF();

    QUrl url(m_avatarPath);
    m_avatar = QPixmap(url.toLocalFile()).scaled(size() * ratio, Qt::KeepAspectRatio, Qt::FastTransformation);
    m_avatar.setDevicePixelRatio(ratio);

    update();
}
