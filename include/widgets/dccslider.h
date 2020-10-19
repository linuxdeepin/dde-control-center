/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef DCCSLIDER_H
#define DCCSLIDER_H

#include <DSlider>

namespace dcc {
namespace widgets {

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
}

#endif // DCCSLIDER_H
