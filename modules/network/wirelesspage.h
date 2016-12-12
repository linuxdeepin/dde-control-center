#ifndef WIRELESSPAGE_H
#define WIRELESSPAGE_H

#include "contentwidget.h"

namespace dcc {

namespace widgets {
class SettingsGroup;
class SwitchWidget;
}

namespace network {

class NetworkModel;
class NetworkDevice;
class WirelessPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit WirelessPage(NetworkDevice *dev, QWidget *parent = 0);

    void setModel(NetworkModel *model);

signals:
    void requestDeviceEnabled(const QString &devPath, const bool enabled) const;

private:
    NetworkDevice *m_device;
    NetworkModel *m_model;

    dcc::widgets::SettingsGroup *m_listGroup;
    dcc::widgets::SwitchWidget *m_switchBtn;
};

}   // namespace dcc

}   // namespace network

#endif // WIRELESSPAGE_H
