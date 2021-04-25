/*
 * Copyright (C) 2017 ~ 2020 Deepin Technology Co., Ltd.
 *
 * Author:     liumaochuan <liumaochuan@uniontech.com>
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

#include "newstyle.h"
#include <QPainter>
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOption>
#include <DApplicationHelper>
#include <DFontSizeManager>

DTK_USE_NAMESPACE
DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

void newStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{

    switch(element){
    case CE_PushButton:
        drawCustomButton(option,painter);
        break;
    default:
        QCommonStyle::drawControl(element,option,painter,widget);
        break;
    }
}

void newStyle::drawCustomButton(const QStyleOption *opt, QPainter *p) const
{
    QPen pen(m_borderNormalColor);

    //把opt转换为QStyleOptionButton类型。
    const QStyleOptionButton *pb = qstyleoption_cast<const QStyleOptionButton*>(opt);
    //获取设置的按钮的大小
    QRect r = pb->rect;
    QRect roundRect = opt->rect.adjusted(+1,+1,-1,-1);
    p->setRenderHint(QPainter::Antialiasing,true);
    int diameter=12;
    int cx = 100*diameter/roundRect.width();
    int cy = 100*diameter/roundRect.height();

    if(!roundRect.isValid()) {
        return;
    }

    if (m_buttonStyle == TextRim) {
        //悬浮状态
        if(pb->state&QStyle::State_MouseOver) {
            pen.setColor(m_borderHoverColor);
        }
        //常态
        else if(pb->state&QStyle::State_Raised) {
            pen.setColor(m_borderNormalColor);
        }
        //按下
        if(pb->state&QStyle::State_Sunken) {
            pen.setColor(m_borderPressColor);
        }
        //当按钮具有焦点时，绘制按钮的焦点边框
        if (pb->state&QStyle::State_HasFocus) {
            DPalette pa = pb->palette;
            pen.setColor(DGuiApplicationHelper::instance()->applicationPalette().color(QPalette::Highlight));
            pen.setWidth(2);
        }

        p->save();
    //        pen.setWidth(1);
    //        pen.setStyle(Qt::SolidLine);
        p->setPen(pen);
        p->drawRoundRect(roundRect,cx,cy);
        p->restore();
        pen.setColor(m_textNormalColor);

    } else if (m_buttonStyle == Text) {
        //悬浮状态
        if(pb->state&QStyle::State_MouseOver) {
            pen.setColor(m_textHoverColor);
        }
        //常态
        else if(pb->state&QStyle::State_Raised) {
            pen.setColor(m_textNormalColor);
        }
        //按下
        if(pb->state&QStyle::State_Sunken) {
            pen.setColor(m_textPressColor);
        }
        //当按钮具有焦点时，绘制按钮的焦点边框
        if (pb->state&QStyle::State_HasFocus) {
            DPalette pa = pb->palette;
            pen.setColor(DGuiApplicationHelper::instance()->applicationPalette().color(QPalette::Highlight));
            pen.setWidth(2);
            p->save();
        //        pen.setWidth(1);
        //        pen.setStyle(Qt::SolidLine);
            p->setPen(pen);
            p->drawRoundRect(roundRect,cx,cy);
            p->restore();
            pen.setColor(m_textNormalColor);
        }

        p->setPen(pen);
    } else if (m_buttonStyle == TextBackground) {
        QPainterPath path;
        path.addRoundRect(roundRect,cx,cy);
        //悬浮状态
        if(pb->state&QStyle::State_MouseOver) {
            pen.setColor(m_textHoverColor);
            p->fillPath(path,m_backgroundHoverColor);
        }
        //常态
        else if(pb->state&QStyle::State_Raised) {
            pen.setColor(m_textNormalColor);
            p->fillPath(path,m_backgroundNormalColor);
        }
        //按下
        if(pb->state&QStyle::State_Sunken) {
            pen.setColor(m_textPressColor);
            p->fillPath(path,m_backgroundPressColor);
        }
        //checked
        if(pb->state&QStyle::State_On) {
            pen.setColor(m_textPressColor);
            p->fillPath(path,m_backgroundPressColor);
        }
        //当按钮具有焦点时，绘制按钮的焦点边框
        if (pb->state&QStyle::State_HasFocus) {
            DPalette pa = pb->palette;
            pen.setColor(DGuiApplicationHelper::instance()->applicationPalette().color(QPalette::Highlight));
            pen.setWidth(2);
            p->save();
        //        pen.setWidth(1);
        //        pen.setStyle(Qt::SolidLine);
            p->setPen(pen);
            p->drawRoundRect(roundRect,cx,cy);
            p->restore();
            pen.setColor(m_textNormalColor);
        }

        p->setPen(pen);
    }

    //设置字体大小12
    quint16 stata = DFontSizeManager::instance()->fontPixelSize(DFontSizeManager::T8);
    QFont font;
    font.setPixelSize(stata);
    p->setFont(font);
    //绘制按钮显示的文本。
    p->drawText(r,Qt::AlignCenter,pb->text);
}

void newStyle::drawCustomCheckBox(const QStyleOption *option, QPainter *painter) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);

    QString themeType;

    DApplicationHelper::ColorType type = DApplicationHelper::instance()->themeType();
    if (type == DApplicationHelper::ColorType::LightType) {
        themeType = "light";
    } else if (type == DApplicationHelper::ColorType::DarkType) {
        themeType = "dark";
    }

    QColor color(0,0,0);
    color.setAlphaF(0.15);

    QPen pen(color);
    painter->setPen(pen);
    painter->drawRoundRect(option->rect.adjusted(+1,+1,-1,-1));

    QString checkedPath = QString(":/themes/%1/icons/checkbox_checked.svg").arg(themeType);
    QString normalPath = QString(":/themes/%1/icons/checkbox_normal.svg").arg(themeType);
    QString hoverPath = QString(":/themes/%1/icons/checkbox_hover.svg").arg(themeType);
    QPixmap pixmap;

    if(option->state&State_MouseOver){
        pixmap.load(hoverPath);
        painter->drawPixmap(option->rect,pixmap);
    }else{
        pixmap.load(normalPath);
        painter->drawPixmap(option->rect,pixmap);
    }

    if(option->state&(State_On|State_NoChange)){
        if(!(option->state&State_Enabled)){
            pixmap.load(checkedPath);
        }else if(option->state&State_NoChange){
            pixmap.load(checkedPath);
        }else{
            pixmap.load(checkedPath);
        }

        //QRect pixmapRect=pixmap.rect().translated(option->rect.topLeft()).translated(+2,-6);
        //QRect painterRect=visualRect(option->direction,option->rect,pixmapRect);

//        if(option->direction==Qt::RightToLeft){
//            painter->scale(-1.0,+1.0);
//            painterRect.moveLeft(-painterRect.right()-1);
//        }

        painter->drawPixmap(option->rect,pixmap);
    } else {
        pixmap.load(normalPath);
        painter->drawPixmap(option->rect,pixmap);
    }

    painter->restore();
}

void newStyle::drawCustomFrame(const QStyleOption *option, QPainter *painter) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing,true);
    QPen pen(m_lineColor);
    painter->setPen(pen);
    painter->drawLine(option->rect.topLeft(),option->rect.bottomRight());
    painter->restore();
}

void newStyle::setTextColor(QColor normal, QColor hover, QColor press, QColor disable)
{
    m_textNormalColor = normal;
    m_textHoverColor = hover;
    m_textPressColor = press;
    m_textDisableColor = disable;
}

void newStyle::setBorderColor(QColor normal, QColor hover, QColor press, QColor disable)
{
    m_borderNormalColor = normal;
    m_borderHoverColor = hover;
    m_borderPressColor = press;
    m_borderDisableColor = disable;
}

void newStyle::setBackgroundColor(QColor normal, QColor hover, QColor press, QColor disable)
{
    m_backgroundNormalColor = normal;
    m_backgroundHoverColor = hover;
    m_backgroundPressColor = press;
    m_backgroundDisableColor = disable;
}

void newStyle::setLineColor(QColor lineColor) {
    m_lineColor = lineColor;
}

int newStyle::pixelMetric(PixelMetric which, const QStyleOption *option, const QWidget *widget) const
{
    switch(which){
    case PM_ButtonDefaultIndicator:
        return 0;
    case PM_IndicatorHeight:
    case PM_IndicatorWidth:
        return 16;
    case PM_CheckBoxLabelSpacing:
        return 8;
    case PM_DefaultFrameWidth:
        return 2;
    default:
        return QCommonStyle::pixelMetric(which,option,widget);
    }
}

newStyle::newStyle(buttonStyle style)
{
    m_buttonStyle = style;
}

void newStyle::polish(QWidget *w) {
    //设置Qt::WA_Hover属性后，将使鼠标在进入或离开部件时产生绘制事件，若不设置此属性，则鼠标进入或离开部件时，部件不会更新。
    w->setAttribute(Qt::WA_Hover,true);
}

void newStyle::unpolish(QWidget *w){
    w->setAttribute(Qt::WA_Hover,false);
}

void newStyle::drawPrimitive(QStyle::PrimitiveElement which, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    switch(which){
        case PE_IndicatorCheckBox:
            drawCustomCheckBox(option,painter);
            break;
        case PE_Widget:
        qInfo() << " PE_Frame";
            drawCustomFrame(option,painter);
            break;
        default:
            QCommonStyle::drawPrimitive(which,option,painter,widget);
        }
}
