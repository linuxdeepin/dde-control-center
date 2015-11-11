#include <QDebug>
#include <QPushButton>

#include <libdui/libdui_global.h>
#include <libdui/dthememanager.h>

#include "constants.h"
#include "abstractdevicewidget.h"
#include "dbus/dbusnetwork.h"
#include "networkglobal.h"

AbstractDeviceWidget::AbstractDeviceWidget(const QString &title, DBusNetwork *dbus, QWidget *parent) :
    DVBoxWidget(parent),
    m_dbusNetwork(dbus)
{
    D_THEME_INIT_WIDGET(AbstractDeviceWidget);

    m_managed = false;
    m_usbDevice = false;
    m_state = 0;

    m_headerLine = new DHeaderLine;
    m_switchButton = new DSwitchButton;

    m_headerLine->setTitle(title);
    m_headerLine->setFixedWidth(DCC::ModuleContentWidth);
    m_headerLine->setFixedHeight(DUI::CONTENT_HEADER_HEIGHT);
    m_headerLine->setContent(m_switchButton);

    m_listWidget = new DListWidget;
    m_listWidget->setStyleSheet(this->styleSheet());
    m_listWidget->setItemSize(DCC::ModuleContentWidth, DUI::RADIO_ITEM_HEIGHT);
    //m_listWidget->hide();

    m_bottomSeparator = new DSeparatorHorizontal;
    m_bottomSeparator->hide();

    connect(m_switchButton, &DSwitchButton::checkedChanged, this, [this](bool enabled) {
        m_bottomSeparator->setVisible(m_listWidget->count() > 0 && enabled);
        emit enabledChanged(enabled);
    });
    connect(m_listWidget, &DListWidget::countChanged, this, [this](int count) {
        //m_bottomSeparator->setVisible(count > 1 && enabled());
        setVisible(count > 0);
    });
    connect(this, &AbstractDeviceWidget::enabledChanged, this, [this](bool enabled){
        if(!path().isEmpty())
            m_dbusNetwork->EnableDevice(QDBusObjectPath(path()), enabled);
    });
    connect(m_dbusNetwork, &DBusNetwork::DeviceEnabled, this, [this](const QString &str, bool enabled){
        if(str == path())
            setEnabled(enabled);
    });

    addWidget(m_headerLine);
    addWidget(new DSeparatorHorizontal);
    addWidget(m_listWidget);
    addWidget(m_bottomSeparator);

    setVisible(listWidget()->count() > 0);
}

DListWidget *AbstractDeviceWidget::listWidget() const
{
    return m_listWidget;
}

void AbstractDeviceWidget::setTitle(const QString &title)
{
    m_headerLine->setTitle(title);
}

bool AbstractDeviceWidget::enabled() const
{
    return m_switchButton->checked();
}

bool AbstractDeviceWidget::managed() const
{
    return m_managed;
}

bool AbstractDeviceWidget::usbDevice() const
{
    return m_usbDevice;
}

int AbstractDeviceWidget::state() const
{
    return m_state;
}

QString AbstractDeviceWidget::path() const
{
    return m_path;
}

QString AbstractDeviceWidget::hwAddress() const
{
    return m_hwAddress;
}

QString AbstractDeviceWidget::vendor() const
{
    return m_vendor;
}

QString AbstractDeviceWidget::uniqueUuid() const
{
    return m_uniqueUnid;
}

QString AbstractDeviceWidget::activeAp() const
{
    return m_activeAp;
}

QString AbstractDeviceWidget::mobileNetworkType() const
{
    return m_mobileNetworkType;
}

QString AbstractDeviceWidget::mobileSignalQuality() const
{
    return m_mobileSignalQuality;
}

void AbstractDeviceWidget::updateInfoByMap(const QVariantMap &map)
{
    for(QString str : map.keys()) {
        const QVariant &value = map.value(str);
        str[0] = str[0].toLower();
        setProperty(qPrintable(str), value);
    }
}

void AbstractDeviceWidget::setEnabled(bool enabled)
{
    m_switchButton->setChecked(enabled);
    listWidget()->setVisible(enabled);
}

void AbstractDeviceWidget::setManaged(bool managed)
{
    if (m_managed == managed)
        return;

    m_managed = managed;
    emit managedChanged(managed);
}

void AbstractDeviceWidget::setUsbDevice(bool usbDevice)
{
    if (m_usbDevice == usbDevice)
        return;

    m_usbDevice = usbDevice;

    if(usbDevice && !vendor().isEmpty())
        setTitle(vendor());

    emit usbDeviceChanged(usbDevice);
}

void AbstractDeviceWidget::setState(int state)
{
    if (m_state == state)
        return;

    m_state = state;
    emit stateChanged(state);
}

void AbstractDeviceWidget::setPath(QString path)
{
    if (m_path == path)
        return;

    m_path = path;

    ASYN_CALL(m_dbusNetwork->IsDeviceEnabled(QDBusObjectPath(path)), {
                  setEnabled(args[0].toBool());
              }, this)

    emit pathChanged(path);
}

void AbstractDeviceWidget::setHwAddress(QString hwAddress)
{
    if (m_hwAddress == hwAddress)
        return;

    m_hwAddress = hwAddress;
    emit hwAddressChanged(hwAddress);
}

void AbstractDeviceWidget::setVendor(QString vendor)
{
    if (m_vendor == vendor)
        return;

    m_vendor = vendor;

    if(usbDevice() && !vendor.isEmpty())
        setTitle(vendor);

    emit vendorChanged(vendor);
}

void AbstractDeviceWidget::setUniqueUuid(QString uniqueUnid)
{
    if (m_uniqueUnid == uniqueUnid)
        return;

    m_uniqueUnid = uniqueUnid;
    emit uniqueUuidChanged(uniqueUnid);
}

void AbstractDeviceWidget::setActiveAp(QString activeAp)
{
    if (m_activeAp == activeAp)
        return;

    m_activeAp = activeAp;
    emit activeApChanged(activeAp);
}

void AbstractDeviceWidget::setMobileNetworkType(QString mobileNetworkType)
{
    if (m_mobileNetworkType == mobileNetworkType)
        return;

    m_mobileNetworkType = mobileNetworkType;
    emit mobileNetworkTypeChanged(mobileNetworkType);
}

void AbstractDeviceWidget::setMobileSignalQuality(QString mobileSignalQuality)
{
    if (m_mobileSignalQuality == mobileSignalQuality)
        return;

    m_mobileSignalQuality = mobileSignalQuality;
    emit mobileSignalQualityChanged(mobileSignalQuality);
}

