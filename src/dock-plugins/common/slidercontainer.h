// Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SLIDERCONTAINER_H
#define SLIDERCONTAINER_H

#include "commoniconbutton.h"
#include "settingsitem.h"

#include <DSlider>
#include <DTipLabel>

#include <QProxyStyle>
#include <QTimer>
#include <QHBoxLayout>
#include <QSpacerItem>

DWIDGET_USE_NAMESPACE

class QLabel;
class SliderProxyStyle;
class SliderIconWidget;
class RightIconButton;

/**
 * @brief 滚动条的类的封装，封装这个类的原因，是为了方便设置左右图标，dtk中的对应的DSlider类也有这个功能，
 * 但是只能简单的设置左右图标，对于右边图标有阴影的，需要外部提供一个带阴影图标，但是如果由外部来提供，
 * 通过QPixmap绘制的带阴影的图标无法消除锯齿（即使通过反走样也不行），因此在此处封装这个类
 */
class SliderContainer : public SettingsItem
{
    Q_OBJECT

public:
    enum IconPosition {
        LeftIcon = 0,
        RightIcon
    };

    enum TitlePosition {
        LeftTip = 0,
        RightTip
    };

public:
    explicit SliderContainer(QWidget *parent = nullptr);
    ~SliderContainer() override;

    void setTip(const QString &text, TitlePosition pos);
    void setSliderProxyStyle(QProxyStyle *proxyStyle);
    void setIcon(const IconPosition &iconPosition, const QString &icon, int space = 0);
    void setIcon(const SliderContainer::IconPosition &iconPosition, const QIcon &icon, int space = 0);
    void setSlider(QSlider *slider);
    void setSlider(Dtk::Widget::DSlider *slider);
    QSlider *slider() const { return m_slider; };

    void setPageStep(int step);
    void setRange(int min, int max);
    void setSliderEnabled(bool enable);
    void setButtonEnabled(IconPosition pos, bool enabled);
    void setButtonsEnabled(bool enabled);
    void setButtonSize(IconPosition pos, QSize size);
    void setButtonsSize(QSize size);
    void setSliderContentsMargin(QMargins margins);

    RightIconButton *rightIconWidget() const { return m_rightIconWidget; };

Q_SIGNALS:
    void iconClicked(IconPosition pos);
    void sliderValueChanged(int value);
    void panelClicked();

public slots:
    void updateSliderValue(int value);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    QSize getSuitableSize(const QSize &iconSize, const QSize &bgSize);

private:
    CommonIconButton *m_leftIconWidget;
    RightIconButton *m_rightIconWidget;
    QSlider *m_slider;
    Dtk::Widget::DTipLabel *m_leftTip;
    Dtk::Widget::DTipLabel *m_rightTip;
    QWidget *m_tipWidget;
    QSpacerItem *m_leftSpacing;
    QSpacerItem *m_rightSpacing;
    QHBoxLayout *m_sliderLayout;
};

/**
 * @brief 用来设置滚动条的样式
 * @param drawSpecial: true
 */
class SliderProxyStyle : public QProxyStyle
{
    Q_OBJECT

public:
    enum StyleType {
        RoundHandler = 0,    // 绘制那种黑色圆底滑动条
        Normal               // 绘制那种通用的滑动条
    };

public:
    explicit SliderProxyStyle(StyleType drawSpecial = RoundHandler, QStyle *style = nullptr);
    ~SliderProxyStyle() override;

protected:
    void drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget = nullptr) const override;

private:
    void drawNormalSlider(QPainter *painter, QRect rectGroove, QRect rectHandle, const QWidget *wigdet) const;
    void drawRoundSlider(QPainter *painter, QRect rectGroove, QRect rectHandle, const QWidget *wigdet) const;

private:
    StyleType m_drawSpecial;
};

class RightIconButton : public CommonIconButton
{
    Q_OBJECT
public:
    explicit RightIconButton(QWidget *parent = nullptr);

    enum StyleType {
        Default = 0,    // CommonIconButton样式
        Background      // 有背景色的CommonIconButton样式
    };
    void setStyleType(const StyleType &style);
    StyleType styleType() const { return m_style; }

protected:
    bool event(QEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    StyleType m_style;
    bool m_press;
    bool m_hover;
};

#endif // VOLUMESLIDER_H
