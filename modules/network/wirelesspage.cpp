#include "wirelesspage.h"

using namespace dcc::network;

WirelessPage::WirelessPage(QWidget *parent)
    : ContentWidget(parent)
{
    setTitle(tr("WLAN"));
}
