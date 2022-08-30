// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
