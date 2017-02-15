/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_SOUND_PORTITEM_H
#define DCC_SOUND_PORTITEM_H

#include "optionitem.h"
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

private slots:
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
