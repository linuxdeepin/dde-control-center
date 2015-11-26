#include <libdui/dthememanager.h>
#include <libdui/libdui_global.h>
#include <libdui/dlineedit.h>
#include <libdui/dtextbutton.h>
#include <libdui/dheaderline.h>

#include "adapterwidget.h"
#include "deviceitemwidget.h"
#include "constants.h"

DUI_USE_NAMESPACE

AdapterWidget::AdapterWidget(BluetoothObject::AdapterInfo *info,
                             QWidget *parent) :
    DVBoxWidget(parent),
    m_info(info)
{
    initUI();
}

AdapterWidget::~AdapterWidget()
{
    if(m_info)
        delete m_info;
}

void AdapterWidget::addDevice(BluetoothObject::DeviceInfo *info)
{
    info->adapterInfo = m_info;

    m_deviceItemList->addWidget(info->item);
}

void AdapterWidget::removeDevice(BluetoothObject::DeviceInfo *info, bool isDelete)
{
    int index = m_deviceItemList->indexOf(info->item);

    if(index >= 0){
        info->adapterInfo = nullptr;
        m_deviceItemList->removeWidget(index, isDelete);
    }
}

void AdapterWidget::updateUI()
{
    m_headerLine->setTitle(m_info->name);
    m_bluetoothSwitch->setChecked(m_info->powered);
    m_deviceItemList->setVisible(m_info->powered);
    m_listWidgetSeparator->setVisible(m_deviceItemList->count() > 0 && m_info->powered);
}

void AdapterWidget::initUI()
{
    setStyleSheet("background: transparent");
    setFixedWidth(BLUETOOTH_PLUGIN_WIDTH);

    m_headerLine = new DHeaderLine;
    m_bluetoothSwitch = new DSwitchButton;

    m_bluetoothSwitch->setChecked(m_info->powered);
    m_headerLine->setFixedWidth(BLUETOOTH_PLUGIN_WIDTH);
    m_headerLine->setTitle(m_info->name);
    m_headerLine->setContent(m_bluetoothSwitch);

    connect(m_bluetoothSwitch, &DSwitchButton::checkedChanged, this, [this](bool checked){
        if(m_info->powered != checked)
            m_info->bluetoothDbus->SetAdapterPowered(QDBusObjectPath(m_info->path), checked);
    });

    m_deviceItemList = new DListWidget;
    m_listWidgetSeparator = new DSeparatorHorizontal;

    m_deviceItemList->setVisible(m_info->powered);
    m_deviceItemList->setMaximumHeight(DEVICE_LIST_MAX_HEIGHT);
    m_deviceItemList->setEnableVerticalScroll(true);
    m_listWidgetSeparator->hide();
    m_info->widget = this;

    addWidget(m_headerLine);
    addWidget(new DSeparatorHorizontal);
    addWidget(m_deviceItemList);
    addWidget(m_listWidgetSeparator);
}
