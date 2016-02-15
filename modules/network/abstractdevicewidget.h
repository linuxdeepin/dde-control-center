/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ABSTRACTDEVICEWIDGET_H
#define ABSTRACTDEVICEWIDGET_H

#include <dboxwidget.h>
#include <dswitchbutton.h>
#include <dheaderline.h>
#include <dseparatorhorizontal.h>
#include <dlistwidget.h>

DWIDGET_USE_NAMESPACE

class DBusNetwork;
class NetworkGenericListItem;
class ScrollFrame;
class AbstractDeviceWidget : public DVBoxWidget
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool managed READ managed WRITE setManaged NOTIFY managedChanged)
    Q_PROPERTY(bool usbDevice READ usbDevice WRITE setUsbDevice NOTIFY usbDeviceChanged)
    Q_PROPERTY(int state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(QString hwAddress READ hwAddress WRITE setHwAddress NOTIFY hwAddressChanged)
    Q_PROPERTY(QString vendor READ vendor WRITE setVendor NOTIFY vendorChanged)
    Q_PROPERTY(QString uniqueUuid READ uniqueUuid WRITE setUniqueUuid NOTIFY uniqueUuidChanged)
    Q_PROPERTY(QString activeAp READ activeAp WRITE setActiveAp NOTIFY activeApChanged)
    Q_PROPERTY(QString mobileNetworkType READ mobileNetworkType WRITE setMobileNetworkType NOTIFY mobileNetworkTypeChanged)
    Q_PROPERTY(QString mobileSignalQuality READ mobileSignalQuality WRITE setMobileSignalQuality NOTIFY mobileSignalQualityChanged)

public:
    explicit AbstractDeviceWidget(const QString &title, DBusNetwork *dbus, QWidget *parent = 0);

    DListWidget *listWidget() const;

    void setTitle(const QString &title);
    bool enabled() const;
    bool managed() const;
    bool usbDevice() const;
    int state() const;
    QString path() const;
    QString hwAddress() const;
    QString vendor() const;
    QString uniqueUuid() const;
    QString activeAp() const;
    QString mobileNetworkType() const;
    QString mobileSignalQuality() const;

    void updateInfoByMap(const QVariantMap &map);

public slots:
    void setEnabled(bool enabled);
    void setManaged(bool managed);
    void setUsbDevice(bool usbDevice);
    void setState(int state);
    void setPath(QString path);
    void setHwAddress(QString hwAddress);
    void setVendor(QString vendor);
    void setUniqueUuid(QString uniqueUuid);
    void setActiveAp(QString activeAp);
    void setMobileNetworkType(QString mobileNetworkType);
    void setMobileSignalQuality(QString mobileSignalQuality);

signals:
    void enabledChanged(bool enabled);
    void managedChanged(bool managed);
    void usbDeviceChanged(bool usbDevice);
    void stateChanged(int state);
    void pathChanged(QString path);
    void hwAddressChanged(QString hwAddress);
    void vendorChanged(QString vendor);
    void uniqueUuidChanged(QString uniqueUuid);
    void activeApChanged(QString activeAp);
    void mobileNetworkTypeChanged(QString mobileNetworkType);
    void mobileSignalQualityChanged(QString mobileSignalQuality);

protected:
    DBusNetwork *m_dbusNetwork;

private:
    bool m_managed;
    bool m_usbDevice;
    int m_state;
    QString m_path;
    QString m_hwAddress;
    QString m_vendor;
    QString m_uniqueUnid;
    QString m_activeAp;
    QString m_mobileNetworkType;
    QString m_mobileSignalQuality;
    DHeaderLine *m_headerLine;
    DSwitchButton *m_switchButton;
    DListWidget *m_listWidget;
    DSeparatorHorizontal *m_bottomSeparator;
};

#endif // ABSTRACTDEVICEWIDGET_H
