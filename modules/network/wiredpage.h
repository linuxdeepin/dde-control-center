#ifndef WIREDPAGE_H
#define WIREDPAGE_H

#include "contentwidget.h"
#include "connectioneditpage.h"

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
    void requestEditConnection(const QString &devPath, const QString &uuid) const;
    void requestCreateConnection(const QString &type, const QString &devPath) const;
    void requestActiveConnection(const QString &devPath, const QString &uuid) const;
    void requestNextPage(ContentWidget * const w) const;
//    void requestConnectAp(const QString &devPath, const QString &apPath, const QString &uuid) const;
//    void requestCreateApConfig(const QString &devPath, const QString &apPath) const;
//    void requestCreateAp(const QString &type, const QString &devPath) const;
//    void requestDeleteConnection(const QString &uuid);
//    void requestDisconnectConnection(const QString &uuid);
//    void requestNextPage(ContentWidget * const w) const;
//    void requestDeviceAPList(const QString &devPath) const;
//    void requestDeviceEnabled(const QString &devPath, const bool enabled) const;
//    void requestFrameKeepAutoHide(const bool autoHide) const;

private slots:
    void initUI();

    void editConnection();
    void createNewConnection();
    void activeConnection();
    void checkActivatedConnection();
    void onSessionCreated(const QString &sessionPath);

private:
    WiredDevice *m_device;
    NetworkModel *m_model;

    QPointer<ConnectionEditPage> m_editPage;

    QPushButton *m_createBtn;
    widgets::SettingsGroup *m_settingsGrp;

    QMap<widgets::NextPageWidget *, QString> m_connectionUuid;
};

}

}

#endif // WIREDPAGE_H
