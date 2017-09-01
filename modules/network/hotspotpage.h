#ifndef HOTSPOTPAGE_H
#define HOTSPOTPAGE_H

#include "contentwidget.h"

#include <QJsonObject>

namespace dcc {

namespace widgets {

class NextPageWidget;
class SwitchWidget;

}

namespace network {

class NetworkModel;
class WirelessDevice;
class HotspotPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit HotspotPage(WirelessDevice *wdev, QWidget *parent = nullptr);

    void setModel(NetworkModel *model);

signals:
    void requestNewHotspot(const QString &debPath);
    void requestActivateConnection(const QString &devPath, const QString &uuid);
    void requestDisconnectConnection(const QString &uuid);

private:
    void onSwitchToggled(const bool checked);
    void onConnectionsChanged();
    void onActiveConnsChanged();

    void closeHotspot();
    void openHotspot();

    inline const QString hotspotUuid() const
    { return m_hotspotInfo.value("Uuid").toString(); }

private:
    WirelessDevice * const m_wdev;
    NetworkModel *m_model;
    widgets::SwitchWidget *m_hotspotSwitch;
    widgets::NextPageWidget *m_configureWidget;

    QJsonObject m_hotspotInfo;
};

}

}

#endif // HOTSPOTPAGE_H
