#include <libdui/dconstants.h>

#include "bluetoothlistitem.h"

BluetoothListItem::BluetoothListItem(QWidget *parent) :
    GenericListItem(parent),
    m_indicator(new DLoadingIndicator(this))
{
    m_indicator->setFixedSize(16, 16);
    m_indicator->setImageSource(QPixmap(":/dark/images/waiting.png"));
    m_indicator->move(10, DUI::EXPAND_HEADER_HEIGHT / 2 - m_indicator->height() / 2);
    m_indicator->hide();

    setShowBgColor(true);
    setStyleSheet(styleSheet()
                  + "GenericListItem{background-color: transparent;}");
}

bool BluetoothListItem::loading() const
{
    return m_loading;
}

void BluetoothListItem::setLoading(bool loading)
{
    if(m_loading == loading)
        return;

    m_loading = loading;

    m_indicator->setVisible(loading);
    setShowBgColor(!loading);
    m_indicator->setLoading(loading);
}

