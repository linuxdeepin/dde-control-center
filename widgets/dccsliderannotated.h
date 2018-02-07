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

#ifndef DCCSLIDERANNOTATED_H
#define DCCSLIDERANNOTATED_H

#include <QFrame>
#include <QSlider>

namespace dcc {
namespace widgets {

class DCCSlider;

class SliderAnnotation : public QWidget
{
    Q_OBJECT
public:
    explicit SliderAnnotation(QWidget *parent=0);
    virtual ~SliderAnnotation() {}

    void setContents(const QStringList &contents);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

class DCCSliderAnnotated : public QFrame
{
    Q_OBJECT
public:
    explicit DCCSliderAnnotated(QWidget *parent = 0);
    virtual ~DCCSliderAnnotated();

    void setAnnotations(const QStringList &annotations);

    DCCSlider *slider() const;
    void setSlider(DCCSlider *slider);

private:
    DCCSlider *m_slider;
    SliderAnnotation *m_annotation;
};

} // namespace widgets
} // namespace dcc

#endif // DCCSLIDERANNOTATED_H
