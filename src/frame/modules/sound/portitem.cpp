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

#include "portitem.h"
#include "soundmodel.h"

#include "widgets/translucentframe.h"

namespace dcc {
namespace sound {

PortItem::PortItem(const Port *port) :
    OptionItem(),
    m_port(port),
    m_frame(new dcc::widgets::TranslucentFrame(this)),
    m_portLabel(new NormalLabel),
    m_cardLabel(new NormalLabel)
{
    m_portLabel->setObjectName("PortItemPortName");
    m_cardLabel->setObjectName("PortItemCardName");

    QVBoxLayout *layout = new QVBoxLayout(m_frame);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addStretch();
    layout->addWidget(m_portLabel);
    layout->addWidget(m_cardLabel);
    layout->addStretch();

    setTitleWidget(m_frame);
    setTitleFixedHeight(64);

    setPort(port);
}

void PortItem::setPort(const Port *port)
{
    m_port = port;

    connect(m_port, &Port::nameChanged, m_portLabel, &NormalLabel::setText);
    connect(m_port, &Port::cardNameChanged, m_portLabel, &NormalLabel::setText);
    connect(m_port, &Port::isActiveChanged, this, &PortItem::setPortItemSelected);

    m_portLabel->setText(m_port->name());
    m_cardLabel->setText(m_port->cardName());
    setPortItemSelected(m_port->isActive());
}

void PortItem::setPortItemSelected(bool state)
{
    blockSignals(true);
    setSelected(state);
    blockSignals(false);
}

Port::Direction PortItem::direction()
{
    return m_port->direction();
}

} // namespace sound
} // namespace dcc
