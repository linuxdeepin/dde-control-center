// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "monitorproxywidget.h"
#include "displaymodel.h"

#include <QPainter>
#include <QMouseEvent>
#include <QScroller>
#include <QScrollArea>
#include <QIcon>

#include <DGuiApplicationHelper>
#include <DPlatformTheme>

using namespace dcc::display;

DGUI_USE_NAMESPACE

MonitorProxyWidget::MonitorProxyWidget(Monitor *mon, DisplayModel *model)
    : m_monitor(mon)
    , m_model(model)
    , m_movedX(m_monitor->x())
    , m_movedY(m_monitor->y())
    , m_preCenter(QPointF(0,0))
    , m_selected(false)
    , m_isMoving(false)
    , m_reSplicing(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
}

void MonitorProxyWidget::setMovedX(const int x)
{
    m_movedX = x;
}
void MonitorProxyWidget::setMovedY(const int y)
{
    m_movedY = y;
}


int MonitorProxyWidget::w() const
{
    //90度和270度
    if (m_monitor->rotate() == 2 || m_monitor->rotate() == 8) {
        return m_monitor->currentMode().height();
    } else {
        return m_monitor->currentMode().width();
    }
}

int MonitorProxyWidget::h() const
{
    //90度和270度
    if (m_monitor->rotate() == 2 || m_monitor->rotate() == 8) {
        return m_monitor->currentMode().width();
    } else {
        return m_monitor->currentMode().height();
    }
}

const QString MonitorProxyWidget::name() const
{
    return m_monitor->name();
}

void MonitorProxyWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // 根据系统主题颜色设置屏幕示意图背景色和边框色
    QColor outsideBorderColor;
    QColor insideBorderColor;
    QColor backgroundColor;
    if (DGuiApplicationHelper::DarkType == DGuiApplicationHelper::instance()->themeType()) {
        outsideBorderColor.setRgb(124, 124, 124);
        insideBorderColor.setRgb(0, 0, 0);

        if (m_monitor->isPrimary()) {
            backgroundColor.setRgb(177, 177, 177);
        } else {
            backgroundColor.setRgb(85, 85, 85);
        }
    } else {
        outsideBorderColor.setRgb(46, 46, 46);
        insideBorderColor.setRgb(255, 255, 255);

        if (m_monitor->isPrimary()) {
            backgroundColor.setRgb(85, 85, 85);
        } else {
            backgroundColor.setRgb(177, 177, 177);
        }
    }

    QRectF r = this->boundingRect();
    auto radius = std::max(r.width(), r.height()) / 20;

    painter->save();
    // 绘制背景底色
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter->setBrush(backgroundColor);
    painter->drawRoundedRect(r, radius, radius);
    painter->setClipRect(r);

    QFont font ("Microsoft YaHei", 100, 20);
    painter->setFont(font);
    const QFontMetrics fm(painter->font());

    //修复在数字为0的时候，计算的字体宽度偏小导致显示遮挡的问题
    const int width = fm.boundingRect(name()).width() + 100;
    const int height = fm.boundingRect(name()).height();

    // 绘制屏幕名称
    painter->setPen(Qt::white);
    if (m_model->displayMode() != MERGE_MODE) {
        if (width > this->w()) {
            QString elidedText = fm.elidedText(name(), Qt::ElideRight, this->w() - height);
            painter->drawText(QRectF(r.x() + 100, r.y() + 100, width, height), Qt::AlignRight, elidedText);
        } else {
            painter->drawText(QRectF(r.x() + 100, r.y() + 100, width, height), Qt::AlignCenter, name());
        }
    }

    // draw dock pattern if it's primary screen
    if (m_model->displayMode() == EXTEND_MODE) {
        // 绘制主屏标识
        if (m_monitor->isPrimary()) {
            QIcon icon = QIcon::fromTheme(":/display/themes/common/dcc-primary.svg");
            if (!icon.isNull()) {
                const auto iconSize = std::min(r.width(), r.height()) / 10;
                const QRect iconRect(r.right() - iconSize * 1.7, r.bottom() - iconSize * 1.7, iconSize, iconSize);
                icon.paint(painter, iconRect, Qt::AlignCenter);
            }
        }
    }

    // 根据是否选中绘制不同颜色边框
    if (m_selected) {
        // 使用系统活动色绘制选中边框
        outsideBorderColor = DGuiApplicationHelper::instance()->systemTheme()->activeColor();
        // 绘制外边框
        QPen borderPen(outsideBorderColor);
        borderPen.setWidth(4);
        borderPen.setWidthF(radius/5.0);
        painter->setPen(borderPen);
        painter->setBrush(Qt::transparent);
        r = r.adjusted(borderPen.width() /2, borderPen.width() / 2, -borderPen.width() /2, -borderPen.width() / 2);
        painter->drawRoundedRect(r, radius, radius);

        // 绘制内边框
        borderPen.setColor(insideBorderColor);
        borderPen.setWidth(4);
        borderPen.setWidthF(radius/5.0);
        painter->setPen(borderPen);
        painter->setBrush(Qt::transparent);
        r = r.adjusted(borderPen.width(), borderPen.width(), -borderPen.width(), -borderPen.width());
        painter->drawRoundedRect(r, radius - 10, radius - 10);
    } else {
        // 绘制单边框
        QPen borderPen(outsideBorderColor);
        borderPen.setWidth(4);
        borderPen.setWidthF(radius/5.0);
        painter->setPen(borderPen);
        painter->setBrush(Qt::transparent);
        r.adjust(borderPen.width() /2, borderPen.width() / 2, -borderPen.width() /2, -borderPen.width() / 2);
        painter->drawRoundedRect(r, radius, radius);
    }

    painter->restore();
}

void MonitorProxyWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_model->displayMode() == EXTEND_MODE) {
        Q_EMIT requestMonitorPress(m_monitor);
    }
    QGraphicsItem::mousePressEvent(event);
}

void MonitorProxyWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseMoveEvent(event);

    if (m_model->displayMode() == EXTEND_MODE) {
        m_isMoving = true;
        Q_EMIT requestMouseMove(this);
    }
}

void MonitorProxyWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_model->displayMode() == EXTEND_MODE) {

        if(m_isMoving) {
            Q_EMIT requestMonitorRelease(m_monitor);
            m_isMoving = false;
        }
    }

    QGraphicsItem::mouseReleaseEvent(event);
}

void MonitorProxyWidget::focusInEvent(QFocusEvent *event)
{
    //增加选中的效果
    m_selected = true;

    QGraphicsItem::focusInEvent(event);
}
void MonitorProxyWidget::focusOutEvent(QFocusEvent *event)
{
    //增加选中的效果
    m_selected = false;

    QGraphicsItem::focusOutEvent(event);
}

void MonitorProxyWidget::keyPressEvent(QKeyEvent *event)
{
    QGraphicsItem::keyPressEvent(event);

   if (m_model->displayMode() == EXTEND_MODE) {
        int keyValue = event->key();
        if (keyValue != Qt::Key_Up && keyValue != Qt::Key_Down && keyValue != Qt::Key_Left && keyValue != Qt::Key_Right) {
            return;
        }
        Q_EMIT requestKeyPress(this, keyValue);
   }
}

QRectF MonitorProxyWidget::boundingRect() const
{
    return QRectF(0, 0, this->w(), this->h());
}

QRectF MonitorProxyWidget::bufferboundingRect() const
{
    return boundingRect().adjusted(-200, -200, 200, 200);
}

QRectF MonitorProxyWidget::justIntersectRect() const
{
    return boundingRect().adjusted(1, 1, -1, -1);
}

//外扩0.05个像素, 规避由于计算导致精度丢失或者坐标值完全一致的情况下不能判定为相交的情况
QRectF MonitorProxyWidget::boundingRectEx() const
{
    return boundingRect().adjusted(-0.05, -0.05, 0.05, 0.05);
}
