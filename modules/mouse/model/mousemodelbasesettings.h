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

#ifndef MOUSEMODELBASESETTINGS_H
#define MOUSEMODELBASESETTINGS_H
#include "mousemodelbase.h"
#include <QObject>

namespace dcc
{
namespace mouse
{
class MouseModelBaseSettings : public MouseModelBase
{
    Q_OBJECT
public:
    explicit MouseModelBaseSettings(QObject *parent = 0);

    void setLeftHandState(const bool state);
    void setDisIfTyping(const bool state);


    inline  bool getLeftHandState() const {return m_leftHandState;}
    inline  bool getDisIfTyping()   const {return m_disIfTyping;}

signals:
    void leftHandStateChanged(const bool state);
    void disIfTypingStateChanged(const bool state);

private:
    bool m_leftHandState;
    bool m_disIfTyping;
};
}
}

#endif // MOUSEMODELBASESETTINGS_H
