// Copyright (C) 2022 ~ 2026 Deepin Technology Co., Ltd.
// SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "slidercontainer.h"
#include "commoniconbutton.h"

#include <DStyle>
#include <DGuiApplicationHelper>
#include <DPaletteHelper>
#include <DFontSizeManager>

#include <QPainterPath>
#include <QMouseEvent>
#include <QGridLayout>
#include <QLabel>

DWIDGET_USE_NAMESPACE

SliderContainer::SliderContainer(QWidget *parent)
    : SettingsItem(parent)
    , m_leftIconWidget(new CommonIconButton(this))
    , m_rightIconWidget(new RightIconButton(this))
    , m_slider(new QSlider(Qt::Orientation::Horizontal, this))
    , m_leftTip(new DTipLabel("", this))
    , m_rightTip(new DTipLabel("", this))
    , m_tipWidget(new QWidget(this))
    , m_leftSpacing(new QSpacerItem(10, 0))
    , m_rightSpacing(new QSpacerItem(10, 0))
{
    m_leftIconWidget->setFixedSize(QSize(24, 24));
    m_rightIconWidget->setFixedSize(QSize(24, 24));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_sliderLayout = new QHBoxLayout;
    m_sliderLayout->setContentsMargins(10, 0, 10, 0);
    m_sliderLayout->setSpacing(0);
    m_sliderLayout->addWidget(m_leftIconWidget);
    m_sliderLayout->addSpacerItem(m_leftSpacing);
    m_sliderLayout->addWidget(m_slider);
    m_sliderLayout->addSpacerItem(m_rightSpacing);
    m_sliderLayout->addWidget(m_rightIconWidget);

    DFontSizeManager::instance()->bind(m_leftTip, DFontSizeManager::T9, QFont::Medium);
    DFontSizeManager::instance()->bind(m_rightTip, DFontSizeManager::T9, QFont::Medium);
    m_leftTip->setForegroundRole(DPalette::TextTips);
    auto titleLayout = new QHBoxLayout(m_tipWidget);
    titleLayout->setSpacing(0);
    titleLayout->setContentsMargins(10, 0, 10, 2);
    titleLayout->addWidget(m_leftTip);
    titleLayout->addStretch();
    titleLayout->addWidget(m_rightTip);

    mainLayout->addStretch();
    mainLayout->addWidget(m_tipWidget);
    mainLayout->addLayout(m_sliderLayout);
    mainLayout->addStretch();

    m_tipWidget->setVisible(false);

    m_leftIconWidget->installEventFilter(this);
    m_slider->setFocusPolicy(Qt::StrongFocus);
    m_slider->setObjectName("Slider");
    m_slider->installEventFilter(this);
    m_rightIconWidget->installEventFilter(this);
    installEventFilter(this);

    connect(m_slider, &QSlider::valueChanged, this, &SliderContainer::sliderValueChanged);

    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, [this]{
        QColor tipColor;
        if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
            tipColor = QColor(0, 0, 0, 255 * 0.60);
        } else {
            tipColor = QColor(255, 255, 255, 255 * 0.60);
        }
        DPalette pa = DPaletteHelper::instance()->palette(this);
        pa.setBrush(DPalette::TextTips, tipColor);
        DPaletteHelper::instance()->setPalette(m_leftTip, pa);
        DPaletteHelper::instance()->setPalette(m_rightTip, pa);
        update();
    });
}

SliderContainer::~SliderContainer()
{
}

void SliderContainer::setSlider(QSlider *slider)
{
    m_sliderLayout->replaceWidget(m_slider, slider);
    m_slider->deleteLater();
    m_slider = slider;
    m_slider->installEventFilter(this);

    connect(m_slider, &QSlider::valueChanged, this, &SliderContainer::sliderValueChanged);
}

void SliderContainer::setSlider(Dtk::Widget::DSlider *slider)
{
    m_sliderLayout->replaceWidget(m_slider, slider);
    m_slider->deleteLater();
    m_slider = slider->slider();
    slider->installEventFilter(this);

    connect(m_slider, &QSlider::valueChanged, this, &SliderContainer::sliderValueChanged);
}

void SliderContainer::setTip(const QString &text, TitlePosition pos)
{
    if (pos == LeftTip) {
        m_leftTip->setText(text);
        m_leftTip->setVisible(!text.isEmpty());
    } else {
        m_rightTip->setText(text);
        m_rightTip->setVisible(!text.isEmpty());
    }

    m_tipWidget->setVisible(!m_leftTip->text().isEmpty() || !m_rightTip->text().isEmpty());
}

QSize SliderContainer::getSuitableSize(const QSize &iconSize, const QSize &bgSize)
{
    if (bgSize.isValid() && !bgSize.isNull() && !bgSize.isEmpty())
        return bgSize;

    return iconSize;
}

void SliderContainer::setIcon(const SliderContainer::IconPosition &iconPosition, const QString &icon, int space)
{
    if (icon.isEmpty()) {
        return;
    }

    switch (iconPosition) {
        case IconPosition::LeftIcon: {
            m_leftIconWidget->setIcon(QIcon::fromTheme(icon));
            m_leftSpacing->changeSize(space, 0);
            break;
        }
        case IconPosition::RightIcon: {
            m_rightIconWidget->setIcon(QIcon::fromTheme(icon));
            m_rightSpacing->changeSize(space, 0);
            break;
        }
    }

    layout()->invalidate();
}

void SliderContainer::setIcon(const SliderContainer::IconPosition &iconPosition, const QIcon &icon, int space)
{
    if (icon.isNull()) {
        return;
    }

    switch (iconPosition) {
        case IconPosition::LeftIcon: {
            m_leftIconWidget->setIcon(icon);
            m_leftSpacing->changeSize(space, 0);
            break;
        }
        case IconPosition::RightIcon: {
            m_rightIconWidget->setIcon(icon);
            m_rightSpacing->changeSize(space, 0);
            break;
        }
    }

    layout()->invalidate();
}

void SliderContainer::setPageStep(int step)
{
    return m_slider->setPageStep(step);
}

void SliderContainer::setRange(int min, int max)
{
    return m_slider->setRange(min, max);
}

void SliderContainer::setButtonEnabled(IconPosition pos, bool enabled)
{
    (pos == IconPosition::LeftIcon ? m_leftIconWidget : m_rightIconWidget)->setEnabled(enabled);
}

void SliderContainer::setSliderEnabled(bool enable)
{
    m_slider->setEnabled(enable);
}

void SliderContainer::setButtonsEnabled( bool enabled)
{
    m_leftIconWidget->setAllEnabled(enabled);

    // 右边按键，在默认样式下（与左按键样式一样），使能状态和左按键保持一致；
    // 在带背景色的样式下（快捷面板内声音/亮度控件的右边按键样式），一直保持使能状态
    m_rightIconWidget->setAllEnabled(m_rightIconWidget->styleType() == RightIconButton::StyleType::Default ? enabled : true);

}

void SliderContainer::setButtonSize(IconPosition pos, QSize size)
{
    (pos == IconPosition::LeftIcon ? m_leftIconWidget : m_rightIconWidget)->setFixedSize(size);
}

void SliderContainer::setButtonsSize(QSize size)
{
    m_leftIconWidget->setFixedSize(size);
    m_rightIconWidget->setFixedSize(size);
}

void SliderContainer::setSliderContentsMargin(QMargins margins)
{
    m_sliderLayout->setContentsMargins(margins);
}

bool SliderContainer::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        auto mouseEvent = dynamic_cast<QMouseEvent*>(event);
        if (mouseEvent && mouseEvent->button() == Qt::LeftButton) {
            if (watched == m_leftIconWidget) {
                Q_EMIT iconClicked(IconPosition::LeftIcon);
                return true;
            }
            if (watched == m_rightIconWidget) {
                Q_EMIT iconClicked(IconPosition::RightIcon);
                return true;
            }
            if (watched == this) {
                Q_EMIT panelClicked();
            }
        }
    }

    if (event->type() == QEvent::Hide && watched == m_slider) {
        m_slider->clearFocus();
    }

    return QWidget::eventFilter(watched, event);
}

void SliderContainer::updateSliderValue(int value)
{
    m_slider->blockSignals(true);
    m_slider->setValue(value);
    m_slider->blockSignals(false);
}

void SliderContainer::setSliderProxyStyle(QProxyStyle *proxyStyle)
{
    proxyStyle->setParent(m_slider);
    m_slider->setStyle(proxyStyle);
}

SliderProxyStyle::SliderProxyStyle(StyleType drawSpecial, QStyle *style)
    : QProxyStyle(style)
    , m_drawSpecial(drawSpecial)
{
}

SliderProxyStyle::~SliderProxyStyle()
{
}

void SliderProxyStyle::drawComplexControl(QStyle::ComplexControl control, const QStyleOptionComplex *option, QPainter *painter, const QWidget *widget) const
{
    if (control != ComplexControl::CC_Slider)
        return;

    // 绘制之前先保存之前的画笔
    painter->save();
    painter->setRenderHint(QPainter::RenderHint::Antialiasing);
    // 获取滑动条和滑块的区域
    const QStyleOptionSlider *sliderOption = static_cast<const QStyleOptionSlider *>(option);
    QRect rectGroove = subControlRect(CC_Slider, sliderOption, SC_SliderGroove, widget);
    QRect rectHandle = subControlRect(CC_Slider, sliderOption, SC_SliderHandle, widget);
    // 设置滑动条高度为 4
    rectGroove.setHeight(4);
    if (m_drawSpecial == RoundHandler)
        drawRoundSlider(painter, rectGroove, rectHandle, widget);
    else
        drawNormalSlider(painter, rectGroove, rectHandle, widget);

    painter->restore();
}

// 绘制通用的滑动条
void SliderProxyStyle::drawNormalSlider(QPainter *painter, QRect rectGroove, QRect rectHandle, const QWidget *widget) const
{
    DPalette dpa = DPaletteHelper::instance()->palette(widget);
    QColor color = dpa.color(DPalette::Highlight);
    QColor rightColor(Qt::gray);
    if (!widget->isEnabled()) {
        color.setAlphaF(0.8);
        rightColor.setAlphaF(0.8);
    }

    QPen penLine = QPen(color, 2);
    // 绘制上下的竖线，一根竖线的宽度是2，+4个像素刚好保证中间也是间隔2个像素
    for (int i = rectGroove.x(); i < rectGroove.x() + rectGroove.width(); i = i + 4) {
        if (i < rectHandle.x())
            painter->setPen(penLine);
        else
            painter->setPen(QPen(rightColor, 2));

        painter->drawLine(i, rectGroove.y() + 2, i, rectGroove.y() + rectGroove.height() - 2);
    }
    // 绘制滚动区域
    painter->setBrush(color);
    painter->setPen(Qt::NoPen);
    QPainterPath path;
    path.addRoundedRect(rectHandle, 6, 6);
    painter->drawPath(path);
}

// 绘制设计师定义的那种圆形滑块，黑色的滑条
void SliderProxyStyle::drawRoundSlider(QPainter *painter, QRect rectGroove, QRect rectHandle, const QWidget *widget) const
{
    // 深色背景下，滑块和滑动条白色，浅色背景下，滑块和滑动条黑色
    QColor color = widget->isEnabled() ? (DGuiApplicationHelper::DarkType == DGuiApplicationHelper::instance()->themeType() ? Qt::white : Qt::black) : Qt::gray;

    // 此处中绘制圆形滑动条，需要绘制圆角，圆角大小为其高度的一半
    int radius = rectGroove.height() / 2;

    // 此处绘制滑条的全长
    QColor sliderColor = color;
    sliderColor.setAlpha(255 * 0.2); // 20% opacity
    QBrush allBrush(sliderColor);
    QPainterPath allPathGroove;
    allPathGroove.addRoundedRect(rectGroove, radius, radius);
    painter->fillPath(allPathGroove, allBrush);

    // 已经滑动过的区域
    QBrush brush(color);
    QPainterPath pathGroove;
    int handleSize = qMin(rectHandle.width(), rectHandle.height());
    rectGroove.setWidth(rectHandle.x() + (rectHandle.width() - handleSize) / 2);
    pathGroove.addRoundedRect(rectGroove, radius, radius);
    painter->fillPath(pathGroove, brush);

    // 绘制滑块,因为滑块是正圆形，而它本来的区域是一个长方形区域，因此，需要计算当前
    // 区域的正中心区域，将其作为一个正方形区域来绘制圆形滑块
    int x = rectHandle.x() + (rectHandle.width() - handleSize) / 2;
    int y = rectGroove.y() + (rectGroove.height() / 2) - handleSize / 2;
    rectHandle.setX(x);
    rectHandle.setY(y);
    rectHandle.setWidth(handleSize);
    rectHandle.setHeight(handleSize);

    QPainterPath pathHandle;
    pathHandle.addEllipse(rectHandle);
    painter->fillPath(pathHandle, brush);
}

RightIconButton::RightIconButton(QWidget *parent)
    : CommonIconButton(parent)
    , m_style(StyleType::Default)
    , m_press(false)
    , m_hover(false)
{
    setAccessibleName("RightIconButton");
    setStyleType(m_style);
}

void RightIconButton::setStyleType(const StyleType &style)
{
    m_style = style;
    if (m_style == StyleType::Background) {
        setFixedSize(36, 36);
        setIconSize(QSize(24, 24));
    } else {
        setIconSize(QSize());
    }
    update();
}

bool RightIconButton::event(QEvent *e)
{
    if (m_style != StyleType::Background) {
        return CommonIconButton::event(e);
    }

    switch (e->type()) {
    case QEvent::Leave:
    case QEvent::Enter:
        m_press = false;
        m_hover = e->type() == QEvent::Enter;
        update();
        break;
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
        m_hover = false;
        m_press = e->type() == QEvent::MouseButtonPress;
        update();
        break;
    default:
        break;
    }
    return QWidget::event(e);
}

void RightIconButton::paintEvent(QPaintEvent *e)
{
    CommonIconButton::paintEvent(e);

    if (m_style != StyleType::Background) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    // 背景
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::instance()->themeType();
    QColor bgColor = type == DGuiApplicationHelper::LightType ? Qt::black : Qt::white;
    if (m_hover) {
        bgColor.setAlphaF(type == DGuiApplicationHelper::LightType ? 0.15 : 0.20);
    } else if (m_press) {
        bgColor.setAlphaF(type == DGuiApplicationHelper::LightType ? 0.20 : 0.25);
    } else {
        bgColor.setAlphaF(type == DGuiApplicationHelper::LightType ? 0.10 : 0.10);
    }
    painter.setBrush(bgColor);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect());
}
