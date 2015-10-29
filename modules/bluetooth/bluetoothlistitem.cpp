#include <libdui/dconstants.h>
#include <libdui/dthememanager.h>

#include "imagenamebutton.h"

#include "bluetoothlistitem.h"

DeviceListItem::DeviceListItem(BluetoothMainWidget::DeviceInfo *info, QWidget *parent) :
    GenericListItem(parent),
    m_info(info)
{
    initUI();
}

DeviceListItem::~DeviceListItem()
{
    if(m_info)
        delete m_info;
}

void DeviceListItem::updateUI()
{
    setTitle(m_info->name);
    setChecked(m_info->state == BluetoothMainWidget::DeviceInfo::Connected);
    setLoading(m_info->state == BluetoothMainWidget::DeviceInfo::Connecting);
}

void DeviceListItem::initUI()
{
    setTitle(m_info->name);
    setChecked(m_info->state == BluetoothMainWidget::DeviceInfo::Connected);
    setLoading(m_info->state == BluetoothMainWidget::DeviceInfo::Connecting);
    setShowBgColor(false);

    connect(this, &DeviceListItem::clicked, this, [this]{
        if(m_info->state == BluetoothMainWidget::DeviceInfo::Disconnected){
            m_info->adapterInfo->bluetoothDbus->ConnectDevice(QDBusObjectPath(m_info->path));
        }
    });

    connect(this, &DeviceListItem::mouseEnter, this, [this]{
        setShowClearButton(true);
    });
    connect(this, &DeviceListItem::mouseLeave, this, [this]{
        setShowClearButton(false);
    });
    connect(this, &DeviceListItem::clearButtonClicked, this, [this]{
        m_info->adapterInfo->bluetoothDbus->DisconnectDevice(QDBusObjectPath(m_info->path));
        if(m_info->state == BluetoothMainWidget::DeviceInfo::Disconnected)
            m_info->adapterInfo->bluetoothDbus->RemoveDevice(QDBusObjectPath(m_info->adapterInfo->path),
                                                             QDBusObjectPath(m_info->path));
    });
}

