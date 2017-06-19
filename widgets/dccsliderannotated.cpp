/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "dccslider.h"
#include "dccsliderannotated.h"

#include <QStylePainter>
#include <QStyleOption>
#include <QVBoxLayout>

namespace dcc {
namespace widgets {

DCCSliderAnnotated::DCCSliderAnnotated(QWidget *parent) :
    QFrame(parent),
    m_slider(new DCCSlider),
    m_annotation(new SliderAnnotation)
{
    m_annotation->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_slider);
    layout->addWidget(m_annotation);
}

DCCSliderAnnotated::~DCCSliderAnnotated()
{

}

void DCCSliderAnnotated::setAnnotations(const QStringList &annotations)
{
    m_annotation->setContents(annotations);
}

DCCSlider *DCCSliderAnnotated::slider() const
{
    return m_slider;
}

void DCCSliderAnnotated::setSlider(DCCSlider *slider)
{
    if (slider == m_slider) return;

    if (!m_slider) {
        QVBoxLayout *la = qobject_cast<QVBoxLayout*>(layout());
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
