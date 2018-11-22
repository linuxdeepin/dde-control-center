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

#ifndef DCC_SOUND_PORTITEM_H
#define DCC_SOUND_PORTITEM_H

#include "widgets/optionitem.h"
#include "soundmodel.h"

using namespace dcc::widgets;

namespace dcc {
namespace sound {

class Port;
class PortItem : public OptionItem
{
public:
    explicit PortItem(const Port *port);
    ~PortItem() {}

    void setPort(const Port *port);
    inline const Port *port() const { return m_port; }

    Port::Direction direction();

private Q_SLOTS:
    void setPortItemSelected(bool state);

private:
    const Port *m_port;
    QWidget *m_frame;
    NormalLabel *m_portLabel;
    NormalLabel *m_cardLabel;
};

} // namespace sound
} // namespace dcc

#endif // DCC_SOUND_PORTITEM_H
