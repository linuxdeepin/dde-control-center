#ifndef VPNPAGE_H
#define VPNPAGE_H

#include "contentwidget.h"

#include <QList>
#include <QJsonObject>

namespace dcc {

namespace widgets {

class SettingsGroup;
class SwitchWidget;

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

public slots:
    void setModel(NetworkModel *model);
    void refershVpnList(const QList<QJsonObject> &vpnList);

private:
    NetworkModel *m_model;

    widgets::SwitchWidget *m_vpnSwitch;
    widgets::SettingsGroup *m_vpnGroup;
};

}

}

#endif // VPNPAGE_H
