// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "widgets/dccslider.h"
#include "widgets/dccsliderannotated.h"

#include <QStylePainter>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QLabel>
#include <QEvent>
#include <QDebug>

namespace dcc {
namespace widgets {

DCCSliderAnnotated::DCCSliderAnnotated(QWidget *parent)
    : QFrame(parent)
    , m_slider(new DCCSlider)
    , m_annotation(new SliderAnnotation)
    , m_leftIconLabel(new QLabel(this))
    , m_rightIconLabel(new QLabel(this))
{
    m_annotation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);


    m_leftIconLabel->setObjectName("SliderLeftIcon");
    m_rightIconLabel->setObjectName("SliderRightIcon");
    m_leftIconLabel->setVisible(false);
    m_rightIconLabel->setVisible(false);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 0, 24, 0);

    layout->addWidget(m_slider);

    m_annotation->setVisible(false);
    layout->addWidget(m_annotation);
}

DCCSliderAnnotated::~DCCSliderAnnotated()
{

}

void DCCSliderAnnotated::setAnnotations(const QStringList &annotations)
{
    m_annotation->setVisible(!annotations.size());
    m_annotation->setContents(annotations);
}

DCCSlider *DCCSliderAnnotated::slider() const
{
    return m_slider;
}

void DCCSliderAnnotated::setSlider(DCCSlider *slider)
{
    if (slider == m_slider)
        return;

    if (!m_slider) {
        QVBoxLayout *la = qobject_cast<QVBoxLayout *>(layout());
        if (la) {
            la->removeWidget(m_slider);
        }
        m_slider->deleteLater();
    }

    m_slider = slider;
}

SliderAnnotation::SliderAnnotation(QWidget *parent) :
    QWidget(parent)
{
    QFont font;
    QFontMetrics fm(font);
    setFixedHeight(fm.height());
}

void SliderAnnotation::setContents(const QStringList &contents)
{
    setProperty("tickmarkLabels", contents);

    update();
}

QString DCCSliderAnnotated::leftIcon() const
{
    return m_leftIcon;
}

void DCCSliderAnnotated::setLeftIcon(const QString &leftIcon)
{
#ifdef WINDOW_MODE
    Q_UNUSED(leftIcon)
#else
    if (leftIcon != m_leftIcon) {
        m_leftIcon = leftIcon;

        QPixmap pix = loadPixmap(m_leftIcon);
        m_leftIconLabel->setPixmap(pix);
        m_leftIconLabel->setVisible(!pix.isNull());

        Q_EMIT leftIconChanged();
    }
#endif
}

void DCCSliderAnnotated::setLeftIcon(const QIcon &leftIcon, const QSize &iconSize)
{
    auto pix = leftIcon.pixmap(iconSize);
    m_leftIconLabel->setPixmap(pix);
    m_leftIconLabel->setVisible(!pix.isNull());

    Q_EMIT leftIconChanged();
}

QString DCCSliderAnnotated::rightIcon() const
{
    return m_rightIcon;
}

void DCCSliderAnnotated::setRightIcon(const QString &rightIcon)
{
#ifdef WINDOW_MODE
    Q_UNUSED(rightIcon)
#else
    if (rightIcon != m_rightIcon) {
        m_rightIcon = rightIcon;

        QPixmap pix = loadPixmap(m_rightIcon);
        m_rightIconLabel->setPixmap(pix);
        m_rightIconLabel->setVisible(!pix.isNull());

        Q_EMIT rightIconChanged();
    }
#endif
}

void DCCSliderAnnotated::setRightIcon(const QIcon &rightIcon, const QSize &iconSize)
{
    auto pix = rightIcon.pixmap(iconSize);
    m_rightIconLabel->setPixmap(pix);
    m_rightIconLabel->setVisible(!pix.isNull());

    Q_EMIT rightIconChanged();
}

bool DCCSliderAnnotated::event(QEvent *evnet)
{
    if (evnet->type() == QEvent::Resize && (m_leftIconLabel || m_rightIconLabel)) {
        QStyleOptionSlider styleoption;
//        slider()->initStyleOption(&styleoption);

        auto ts = slider()->style();
        auto rect = ts->subControlRect(QStyle::CC_Slider, &styleoption, QStyle::SC_SliderGroove, slider());

        if (m_leftIconLabel) {
            m_leftIconLabel->adjustSize();
            auto leftRect = m_leftIconLabel->rect();
            leftRect.moveCenter(QPoint(leftRect.width() / 2, rect.center().y()));
            m_leftIconLabel->setGeometry(leftRect);
        }

        if (m_rightIconLabel) {
            m_rightIconLabel->adjustSize();
            auto rightRect = m_rightIconLabel->rect();
            rightRect.moveCenter(QPoint(this->rect().right() - rightRect.width() / 2, rect.center().y()));
            m_rightIconLabel->setGeometry(rightRect);
        }
    }

    return QWidget::event(evnet);
}

void SliderAnnotation::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);

    QStyleOption opt;
    opt.initFrom(this);

    painter.drawControl(QStyle::ControlElement(QStyle::CE_CustomBase + 1024), opt);

    painter.end();
}

} // namespace widgets
} // namespace dcc
