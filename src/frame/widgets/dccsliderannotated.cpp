/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "widgets/dccslider.h"
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
