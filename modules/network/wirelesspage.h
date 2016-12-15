#ifndef WIRELESSPAGE_H
#define WIRELESSPAGE_H

#include "contentwidget.h"

#include <QMap>
#include <QTimer>
#include <QPointer>

namespace dcc {

namespace widgets {
class SettingsGroup;
class SwitchWidget;
}

namespace network {

class ConnectHiddenPage;
class AccessPointEditPage;
class AccessPointWidget;
class NetworkModel;
class WirelessDevice;
class WirelessPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit WirelessPage(WirelessDevice *dev, QWidget *parent = 0);
    ~WirelessPage();

    void setModel(NetworkModel *model);

public slots:
    void onAPAdded(const QJsonObject &apInfo);
    void onAPChanged(const QJsonObject &apInfo);
    void onAPRemoved(const QString &ssid);

signals:
    void requestNextPage(ContentWidget * const w) const;
    void requestDeviceStatus(const QString &devPath) const;
    void requestDeviceAPList(const QString &devPath) const;
    void requestDeviceEnabled(const QString &devPath, const bool enabled) const;

private slots:
    void onDeviceRemoved();
    void sortAPList();
    void showConnectHidePage();
    void showAPEditPage(const QString &path);

private:
    WirelessDevice *m_device;
    NetworkModel *m_model;

    dcc::widgets::SettingsGroup *m_listGroup;
    dcc::widgets::SwitchWidget *m_switchBtn;
    AccessPointWidget *m_connectHideSSID;

    QPointer<ConnectHiddenPage> m_connectHidePage;
    QPointer<AccessPointEditPage> m_apEditPage;

    QTimer m_sortDelayTimer;
    QMap<QString, AccessPointWidget *> m_apItems;
};

}   // namespace dcc

}   // namespace network

#endif // WIRELESSPAGE_H
