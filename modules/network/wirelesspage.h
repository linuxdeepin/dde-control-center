#ifndef WIRELESSPAGE_H
#define WIRELESSPAGE_H

#include "contentwidget.h"

#include <QMap>
#include <QTimer>

namespace dcc {

namespace widgets {
class SettingsGroup;
class SwitchWidget;
}

namespace network {

class AccessPointWidget;
class NetworkModel;
class NetworkDevice;
class WirelessPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit WirelessPage(NetworkDevice *dev, QWidget *parent = 0);

    void setModel(NetworkModel *model);

public slots:
    void onAPAdded(const QJsonObject &apInfo);
    void onAPChanged(const QJsonObject &apInfo);
    void onAPRemoved(const QString &ssid);

signals:
    void requestDeviceAPList(const QString &devPath) const;
    void requestDeviceEnabled(const QString &devPath, const bool enabled) const;

private slots:
    void sortAPList();

private:
    NetworkDevice *m_device;
    NetworkModel *m_model;

    dcc::widgets::SettingsGroup *m_listGroup;
    dcc::widgets::SwitchWidget *m_switchBtn;

    QTimer m_sortDelayTimer;
    QMap<QString, AccessPointWidget *> m_apItems;
};

}   // namespace dcc

}   // namespace network

#endif // WIRELESSPAGE_H
