/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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
