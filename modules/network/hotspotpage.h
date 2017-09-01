#ifndef HOTSPOTPAGE_H
#define HOTSPOTPAGE_H

#include "contentwidget.h"
#include "connectioneditpage.h"

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
    void requestNextPage(ContentWidget * const w) const;
    void requestNewHotspot(const QString &debPath) const;
    void requestDisconnectConnection(const QString &uuid) const;
    void requestDeleteConnection(const QString &uuid);
    void requestActivateConnection(const QString &devPath, const QString &uuid) const;
    void requestEditConnection(const QString &devPath, const QString &uuid) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;

private:
    void onSwitchToggled(const bool checked);
    void onConfigWidgetClicked();
    void onConnectionsChanged();
    void onActiveConnsChanged();
    void onConnectionSessionCreated(const QString &sessionPath);

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
    QPointer<ConnectionEditPage> m_editPage;
};

}

}

#endif // HOTSPOTPAGE_H
