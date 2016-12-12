#ifndef WIRELESSPAGE_H
#define WIRELESSPAGE_H

#include "contentwidget.h"

namespace dcc {

namespace network {

class WirelessPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit WirelessPage(QWidget *parent = 0);
};

}   // namespace dcc

}   // namespace network

#endif // WIRELESSPAGE_H
