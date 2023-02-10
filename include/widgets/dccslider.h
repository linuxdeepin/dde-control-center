//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include <DSlider>
namespace DCC_NAMESPACE {

class DCCSlider : public DTK_WIDGET_NAMESPACE::DSlider
{
    Q_OBJECT
public:
    enum SliderType {
        Normal,
        Vernier,
        Progress
    };

public:
    explicit DCCSlider(SliderType type = Normal, QWidget *parent = nullptr);
    explicit DCCSlider(Qt::Orientation orientation, QWidget *parent = nullptr);

    inline DCCSlider *slider() const { return const_cast<DCCSlider *>(this); }
    QSlider *qtSlider();

    void setType(SliderType type);
    void setRange(int min, int max);
    void setTickPosition(QSlider::TickPosition tick);
    void setTickInterval(int ti);
    void setSliderPosition(int Position);
    void setAnnotations(const QStringList &annotations);
    void setOrientation(Qt::Orientation orientation);
    //当value大于0时，在slider中插入一条分隔线
    void setSeparateValue(int value = 0);

protected:
    void wheelEvent(QWheelEvent *e);
    void paintEvent(QPaintEvent *e);
private:
    QSlider::TickPosition tickPosition = QSlider::TicksBelow;
    int m_separateValue;
};

}
