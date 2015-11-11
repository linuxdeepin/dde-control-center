#ifndef MODEMNETWORKLISTITEM_H
#define MODEMNETWORKLISTITEM_H

#include "abstractdevicewidget.h"

class ModemNetworkListItem : public AbstractDeviceWidget
{
    Q_OBJECT
public:
    explicit ModemNetworkListItem(DBusNetwork *dbus, QWidget *parent = 0);

private slots:
    void onConnectsChanged();
    void init();

private:
    NetworkGenericListItem* m_item = nullptr;
};

#endif // MODEMNETWORKLISTITEM_H
