/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     fanpengcheng <fanpengcheng_cm@deepin.com>
 *
 * Maintainer: fanpengcheng <fanpengcheng_cm@deepin.com>
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
#include "button.h"
#include "constants.h"

#include <QAction>
#include <QDebug>
#include <QFont>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include <QSvgRenderer>
#include <QStyleOption>
#include <QVBoxLayout>
#include <DFontSizeManager>

ButtonContent::ButtonContent(QWidget *parent)
    : DWidget(parent)
    , m_text("")
    , m_align(Qt::AlignCenter)
{

}

void ButtonContent::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;

    update();
}

void ButtonContent::setText(const QString &text)
{
    m_text = text;

    update();
}

void ButtonContent::setId(const QString &id)
{
    m_id = id;
}

void ButtonContent::setTextAlignment(Qt::Alignment align)
{
    m_align = align;

    update();
}

QSize ButtonContent::sizeHint() const
{
    return QSize(60, 50);
}

QSize ButtonContent::minimumSizeHint() const
{
    return QSize(30, 25);
}

void ButtonContent::drawPixmap(QPainter *painter)
{
    painter->save();

    painter->drawPixmap(geometry(), m_pixmap, QRectF());

    painter->restore();
}

void ButtonContent::drawContent(QPainter *painter)
{
    painter->save();

    QTextOption option;
    option.setAlignment(textAlignment());
    QPen pen;

    pen.setColor(palette().color(QPalette::BrightText));
    painter->setPen(pen);

    QFontMetrics metrics = this->fontMetrics();
    QRect rect = geometry().marginsRemoved(QMargins(3, 3, 3, 3));
    painter->drawText(rect, metrics.elidedText(text(), Qt::ElideRight, rect.width(), 0), option);

    painter->restore();
}

void ButtonContent::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (geometry().contains(mapFromGlobal(QCursor::pos()))) {
            Q_EMIT clicked();
            Q_EMIT toggled(m_id);
        }
    }
}

void ButtonContent::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void ButtonContent::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawPixmap(&painter);
    drawContent(&painter);

    QWidget::paintEvent(event);
}

ButtonMenu::ButtonMenu(QWidget *parent)
    : DWidget(parent)
{

}

QSize ButtonMenu::sizeHint() const
{
    return QSize(MenuWidth + 2 * MenuPadding, MenuHeight + 2 * MenuPadding);
}

QSize ButtonMenu::minimumSizeHint() const
{
    return QSize(MenuWidth + 2 * MenuPadding, MenuHeight + 2 * MenuPadding);
}

void ButtonMenu::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    Q_EMIT clicked();
}

void ButtonMenu::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void ButtonMenu::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QStyleOption opt;
    opt.init(this);

    opt.rect.setX(MenuPadding);
    opt.rect.setY((height() - MenuHeight) / 2);
    opt.rect.setWidth(MenuWidth);
    opt.rect.setHeight(MenuHeight);

    QPen pen;
    painter.setPen(QPen(palette().color(QPalette::BrightText), 1.2));

    QPointF p0(opt.rect.topLeft());
    QPointF p1(opt.rect.topLeft().x() + opt.rect.width() / 2 * 1.0, opt.rect.bottomRight().y());
    QPointF p2(opt.rect.topRight());

    painter.drawLine(p0, p1);
    painter.drawLine(p1, p2);
}

Button::Button(QWidget *parent)
    : DWidget(parent)
    , m_button(new ButtonContent(this))
    , m_menuArea(new ButtonMenu(this))
    , m_menu(new DMenu(this))
    , m_radius(0)
    , m_hover(false)
{
    m_button->setAccessibleName("ButtonContent");
    m_menuArea->setAccessibleName("ButtonMenu");
    m_menu->setAccessibleName("Menu");

    m_menuArea->setFixedWidth(MIN(MenuWidth + 2 * MenuPadding, width() / 3));
    m_menuArea->hide();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addWidget(m_button);
    layout->addWidget(m_menuArea);
    setLayout(layout);

    connect(m_button, &ButtonContent::clicked, this, &Button::clicked);
    connect(m_button, &ButtonContent::toggled, this, &Button::toggled);
    connect(m_menuArea, &ButtonMenu::clicked, this, &Button::onMenuClicked);
}

void Button::setPixmap(const QPixmap &pixmap)
{
    m_button->setPixmap(pixmap);
}

void Button::setText(const QString &text)
{
    m_button->setFont(DFontSizeManager::instance()->t7());
    m_button->setText(text);
}

void Button::setId(const QString &id)
{
    m_button->setId(id);
}

void Button::setTextAlignment(Qt::Alignment align)
{
    m_button->setTextAlignment(align);
}

void Button::setRadius(int radius)
{
    m_radius = radius;

    update();
}

void Button::addAction(QAction *action)
{
    m_menu->addAction(action);

    m_menuArea->show();
}

void Button::clear()
{
    m_menu->clear();

    m_menuArea->hide();
}

void Button::setHoverState(bool state)
{
    m_hover = state;

    update();
}

QSize Button::sizeHint() const
{
    return QSize(60, 50);
}

QSize Button::minimumSizeHint() const
{
    return QSize(30, 25);
}

void Button::onMenuClicked()
{
    //unable to determine QMenu's size before it is displayed.
    m_menu->show();
    QPoint p;
    p.setX(geometry().x() - m_menu->width());
    p.setY(geometry().y() + geometry().height());
    m_menu->exec(mapToGlobal(p));
}

void Button::drawBackground(QPainter *painter)
{
    painter->save();

    QPalette pe = this->palette();
    QColor brushColor(pe.color(QPalette::Base));
    brushColor.setAlpha(m_hover ? 120 : 60);
    painter->setBrush(brushColor);

    QPen borderPen;
    borderPen.setColor(Qt::transparent);
    painter->setPen(borderPen);
    painter->drawRoundedRect(QRectF(0, 0, width(), height()), m_radius, m_radius);

    painter->restore();
}

void Button::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(&painter);

    QWidget::paintEvent(event);
}

void Button::enterEvent(QEvent *event)
{
    setHoverState(true);

    DWidget::enterEvent(event);
}

void Button::leaveEvent(QEvent *event)
{
    setHoverState(false);

    DWidget::leaveEvent(event);
}

void Button::focusInEvent(QFocusEvent *event)
{
    setHoverState(true);

    DWidget::focusInEvent(event);
}

void Button::focusOutEvent(QFocusEvent *event)
{
    setHoverState(false);

    DWidget::focusOutEvent(event);
}
