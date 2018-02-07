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

#ifndef MOUSEMODELBASE_H
#define MOUSEMODELBASE_H

#include <QObject>
#include <QDebug>

namespace dcc
{
namespace mouse
{
class MouseModelBase : public QObject
{
    Q_OBJECT
public:
    explicit MouseModelBase(QObject *parent = 0);
    void setSliderValue(const int &value);
    inline bool getSliderValue() {return m_sliderValue;}
    void setExist(bool state);
    inline bool getExist(){return m_isExist;}

    bool getNaturalScroll() const;
    void setNaturalScroll(bool naturalScroll);

signals:
    void sliderValueChanged(const int &value);
    void existChanged(bool state);
    void naturalScrollChanged(bool state);

private:
    int m_sliderValue;
    bool m_isExist;
    bool m_naturalScroll;
};
}
}

#endif // MOUSEMODELBASE_H
