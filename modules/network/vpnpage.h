#ifndef VPNPAGE_H
#define VPNPAGE_H

#include "contentwidget.h"

#include <QList>
#include <QJsonObject>
#include <QMap>
#include <QPointer>

namespace dcc {

namespace widgets {

class SettingsGroup;
class SwitchWidget;
class NextPageWidget;

}

namespace network {

class NetworkModel;
class ConnectionEditPage;
class VpnPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit VpnPage(QWidget *parent = 0);
    ~VpnPage();

signals:
    void requestNextPage(ContentWidget * const w) const;
    void requestVpnEnabled(const bool enabled) const;
    void requestEditVpn(const QString &devPath, const QString &connPath) const;

public slots:
    void setModel(NetworkModel *model);
    void refershVpnList(const QList<QJsonObject> &vpnList);

private slots:
    void onVpnClicked();
    void onVpnSessionCreated(const QString &device, const QString &sessionPath);

private:
    NetworkModel *m_model;

    widgets::SwitchWidget *m_vpnSwitch;
    widgets::SettingsGroup *m_vpnGroup;

    QPointer<ConnectionEditPage> m_editPage;

    QMap<widgets::NextPageWidget *, QJsonObject> m_vpns;
};

}

}

#endif // VPNPAGE_H
