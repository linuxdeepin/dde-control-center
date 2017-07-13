#ifndef WIREDPAGE_H
#define WIREDPAGE_H

#include "contentwidget.h"
#include "connectioneditpage.h"
#include "wireddevice.h"

class QPushButton;

namespace dcc {

namespace widgets {
class SettingsGroup;
class NextPageWidget;
}

namespace network {

class WiredDevice;
class NetworkModel;
class ConnectionEditPage;
class WiredPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit WiredPage(WiredDevice *dev, QWidget *parent = 0);

    void setModel(NetworkModel *model);

signals:
    void requestConnectionsList(const QString &devPath);
    void requestEditConnection(const QString &devPath, const QString &uuid) const;
    void requestCreateConnection(const QString &type, const QString &devPath) const;
    void requestActiveConnection(const QString &devPath, const QString &uuid) const;
    void requestDeleteConnection(const QString &uuid) const;
    void requestDisconnectConnection(const QString &uuid) const;
    void requestNextPage(ContentWidget * const w) const;

private slots:
    void initUI();

    void refreshConnectionList();
    void editConnection();
    void createNewConnection();
    void activeConnection();
    void checkActivatedConnection();
    void onDeviceStatusChanged(const WiredDevice::DeviceStatus stat);
    void onSessionCreated(const QString &sessionPath);
    void onDeviceRemoved();

private:
    WiredDevice *m_device;
    NetworkModel *m_model;

    QPointer<ConnectionEditPage> m_editPage;

    QPushButton *m_createBtn;
    widgets::SettingsGroup *m_settingsGrp;
    widgets::SettingsGroup *m_tipsGrp;

    QMap<widgets::NextPageWidget *, QString> m_connectionPath;
};

}

}

#endif // WIREDPAGE_H
