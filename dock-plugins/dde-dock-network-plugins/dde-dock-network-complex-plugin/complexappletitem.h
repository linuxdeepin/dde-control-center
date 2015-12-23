#ifndef COMPLEXAPPLETITEM_H
#define COMPLEXAPPLETITEM_H

#include <QFrame>
#include <QLabel>
#include "dbus/dbusnetwork.h"
#include "libdui/dimagebutton.h"
#include "../network-data/networkdata.h"

DUI_USE_NAMESPACE
using namespace NetworkPlugin;

class ComplexAppletItem final : public DImageButton
{
    Q_OBJECT
public:
    enum DeviceType {
        DeviceUnknow,
        DeviceVPN,
        DeviceBluetooth,
        DeviceWired,
        DeviceWireless,
        DeviceMobile
    };

    explicit ComplexAppletItem(QWidget *parent = 0);

    void setIcon(const QString &icon);
    QString index() const;
    void setIndex(const QString &value);

    DeviceType deviceType() const;
    void setDeviceType(const DeviceType &deviceType);

private:
    QLabel *m_indexLabel;
    ComplexAppletItem::DeviceType m_deviceType = DeviceUnknow;
};

#endif // COMPLEXAPPLETITEM_H
