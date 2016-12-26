#ifndef VPNPAGE_H
#define VPNPAGE_H

#include "contentwidget.h"

#include <QList>
#include <QJsonObject>
#include <QMap>

namespace dcc {

namespace widgets {

class SettingsGroup;
class SwitchWidget;
class NextPageWidget;

}

namespace network {

class NetworkModel;
class VpnPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit VpnPage(QWidget *parent = 0);

signals:
    void requestVpnEnabled(const bool enabled) const;
    void requestEditVpn(const QString &devPath, const QString &connPath) const;

public slots:
    void setModel(NetworkModel *model);
    void refershVpnList(const QList<QJsonObject> &vpnList);

private slots:
    void onVpnClicked();

private:
    NetworkModel *m_model;

    widgets::SwitchWidget *m_vpnSwitch;
    widgets::SettingsGroup *m_vpnGroup;

    QMap<widgets::NextPageWidget *, QJsonObject> m_vpns;
};

}

}

#endif // VPNPAGE_H
