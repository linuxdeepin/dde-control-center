#include <libdui/dconstants.h>
#include <libdui/dthememanager.h>

#include "imagenamebutton.h"

#include "bluetoothlistitem.h"

DeviceListItem::DeviceListItem(BluetoothMainWidget::DeviceInfo *info, QWidget *parent) :
    GenericListItem(parent),
    m_indicator(new DLoadingIndicator(this)),
    m_info(info)
{
    D_THEME_INIT_WIDGET(DeviceListItem, checked);

    initUI();
}

DeviceListItem::~DeviceListItem()
{
    if(m_info)
        delete m_info;
}

bool DeviceListItem::loading() const
{
    return m_loading;
}

void DeviceListItem::setLoading(bool loading)
{
    if(m_loading == loading)
        return;

    m_loading = loading;

    m_indicator->setVisible(loading);
    setShowBgColor(!loading);
    m_indicator->setLoading(loading);
}

void DeviceListItem::updateUI()
{
    setTitle(m_info->name);
    setChecked(m_info->state == BluetoothMainWidget::DeviceInfo::Connected);
    setLoading(m_info->state == BluetoothMainWidget::DeviceInfo::Connecting);
}

void DeviceListItem::initUI()
{
    m_indicator->setFixedSize(16, 16);
    m_indicator->setImageSource(QPixmap(":/dark/images/waiting.png"));
    m_indicator->move(10, DUI::EXPAND_HEADER_HEIGHT / 2 - m_indicator->height() / 2);
    m_indicator->hide();

    ImageNameButton *clear_button = new ImageNameButton("clear", this);
    clear_button->setFixedSize(m_indicator->size());
    clear_button->move(m_indicator->pos());
    clear_button->setHoverPic(clear_button->getNormalPic());
    clear_button->setPressPic(clear_button->getNormalPic());
    clear_button->hide();

    setTitle(m_info->name);
    setChecked(m_info->state == BluetoothMainWidget::DeviceInfo::Connected);
    setLoading(m_info->state == BluetoothMainWidget::DeviceInfo::Connecting);

    connect(this, &DeviceListItem::clicked, this, [this, clear_button]{
        if(m_info->state == BluetoothMainWidget::DeviceInfo::Disconnected){
            m_info->adapterInfo->bluetoothDbus->ConnectDevice(QDBusObjectPath(m_info->path));
            clear_button->hide();
        }
    });

    connect(this, &DeviceListItem::mouseEnter, clear_button, [this, clear_button]{
        m_indicator->hide();
        setShowBgColor(false);
        clear_button->show();
    });
    connect(this, &DeviceListItem::mouseLeave, clear_button, [this, clear_button]{
        clear_button->hide();
        setShowBgColor(!loading());
        m_indicator->setVisible(loading());
    });
    connect(clear_button, &ImageNameButton::clicked, this, [this]{
        m_info->adapterInfo->bluetoothDbus->DisconnectDevice(QDBusObjectPath(m_info->path));
        if(m_info->state == BluetoothMainWidget::DeviceInfo::Disconnected)
            m_info->adapterInfo->bluetoothDbus->RemoveDevice(QDBusObjectPath(m_info->adapterInfo->path),
                                                             QDBusObjectPath(m_info->path));
    });
}

