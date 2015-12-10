#ifndef VPNAPPLETITEM_H
#define VPNAPPLETITEM_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include "dbus/dbusnetwork.h"
#include "libdui/dloadingindicator.h"
#include "libdui/dimagebutton.h"
#include "../network-data/networkdata.h"

DUI_USE_NAMESPACE
using namespace NetworkPlugin;

class VpnAppletItem : public QFrame
{
    Q_OBJECT
public:    
    struct VpnData {
        QString path;
        QString uuid;
        QString id;
    };

    explicit VpnAppletItem(const VpnData &data, DBusNetwork *dbusNetwork, QWidget *parent = 0);

    QString getItemUuid() const;
    VpnData getItemData() const;

private:
    void updateConnectionState();

private:
    DBusNetwork *m_dbusNetwork;
    VpnData m_itemData;

    QPushButton *m_title;
    DImageButton *m_checkIcon;
    DLoadingIndicator *m_loadingIcon;
};

#endif // VPNAPPLETITEM_H
