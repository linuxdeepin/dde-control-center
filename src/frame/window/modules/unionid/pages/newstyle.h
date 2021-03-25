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

#ifndef CUSTOMSWITCHBUTTON_H
#define CUSTOMSWITCHBUTTON_H

#include <QCommonStyle>

class newStyle : public QCommonStyle
{
    Q_OBJECT
public:
    enum buttonStyle{
        Text,       //只有文字
        Rim,        //只有边框
        background, //只有背景
        TextRim,   //文字加边框
        TextBackground //文字加背景
    };

    newStyle() {}
    newStyle(buttonStyle buttonStyle);
//    void polish(QPalette& palette);
    void polish(QWidget* widget);
    void unpolish(QWidget* widget);
//    int  styleHint(StyleHint hint, const QStyleOption *opt, const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const;
    int  pixelMetric(PixelMetric pm, const QStyleOption *option, const QWidget *widget = nullptr) const;
    void drawPrimitive(PrimitiveElement which,const QStyleOption* option,QPainter* painter,const QWidget* widget = nullptr)const;
    void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p,const QWidget *w = nullptr) const;
//    void drawComplexControl(ComplexControl which,const QStyleOptionComplex* option,QPainter* painter,const QWidget* widget = nullptr)const;
//    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc, const QWidget *w = nullptr) const;
    void drawCustomButton(const QStyleOption *opt, QPainter *p) const;
    void drawCustomCheckBox(const QStyleOption *opt, QPainter *p) const;
    void drawCustomFrame(const QStyleOption *opt, QPainter *p) const;
    void setTextColor(QColor normal, QColor hover = nullptr, QColor press = nullptr, QColor disable = nullptr);
    void setBorderColor(QColor normal, QColor hover = nullptr, QColor press = nullptr, QColor disable = nullptr);
    void setBackgroundColor(QColor normal, QColor hover = nullptr, QColor press = nullptr, QColor disable = nullptr);
    void setLineColor(QColor lineColor);
private:
    buttonStyle m_buttonStyle;
    QColor m_textNormalColor;
    QColor m_textHoverColor;
    QColor m_textPressColor;
    QColor m_textDisableColor;
    QColor m_borderNormalColor;
    QColor m_borderHoverColor;
    QColor m_borderPressColor;
    QColor m_borderDisableColor;
    QColor m_backgroundNormalColor;
    QColor m_backgroundHoverColor;
    QColor m_backgroundPressColor;
    QColor m_backgroundDisableColor;
    QColor m_lineColor;
};

#endif // CUSTOMSWITCHBUTTON_H
