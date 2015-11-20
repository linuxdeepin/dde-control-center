#include "imagenamebutton.h"
#include "deviceitemwidget.h"
#include "constants.h"
#include "confrimwidget.h"
#include "adapterwidget.h"

#include <QHBoxLayout>
#include <QDBusObjectPath>

#include <libdui/dconstants.h>
#include <libdui/dthememanager.h>

DeviceItemWidget::DeviceItemWidget(BluetoothMainWidget::DeviceInfo *info, QWidget *parent)
    : QWidget(parent),
      m_info(info)
{
    m_deviceName = new QLabel;
    m_deviceName->setStyleSheet("color:#b4b4b4;");
    m_statTips = new QLabel;
    m_statTips->setStyleSheet("color:#666;");
    m_removeBtn = new DImageButton;
    m_removeBtn->setNormalPic(":/dark/images/disconnect.png");
    m_removeBtn->hide();
    m_loadingIndicator = new DLoadingIndicator;
    m_loadingIndicator->setFixedSize(16, 16);
    m_loadingIndicator->setImageSource(QPixmap(":/dark/images/waiting.png"));
    m_loadingIndicator->setLoading(true);
    m_loadingIndicator->hide();
//    m_loadingIndicator->show();

    QHBoxLayout *iconsLayout = new QHBoxLayout;
    iconsLayout->addWidget(m_removeBtn);
    iconsLayout->addWidget(m_loadingIndicator);
    iconsLayout->setMargin(0);
    iconsLayout->setSpacing(0);

    QWidget *iconsWidget = new QWidget;
    iconsWidget->setLayout(iconsLayout);
    iconsWidget->setFixedWidth(16);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_deviceName);
    mainLayout->addStretch(0);
    mainLayout->addWidget(m_statTips);
    mainLayout->addWidget(iconsWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 2, 10, 2);

    setLayout(mainLayout);
    updateUI();

    connect(m_removeBtn, &DImageButton::clicked, this, &DeviceItemWidget::disConnect);
}

DeviceItemWidget::~DeviceItemWidget()
{
    if(m_info)
        delete m_info;
}

void DeviceItemWidget::updateUI()
{
    m_deviceName->setText(m_info->name);

    m_statTips->setVisible(m_info->paired);
    if (m_info->state != BluetoothMainWidget::DeviceInfo::Connecting)
        m_loadingIndicator->hide();

    switch (m_info->state)
    {
    case BluetoothMainWidget::DeviceInfo::Connected:        m_statTips->setText(tr("Connected"));           break;
    case BluetoothMainWidget::DeviceInfo::Connecting:       m_statTips->clear();                            break;
    case BluetoothMainWidget::DeviceInfo::Disconnected:     m_statTips->setText(tr("Unconnect"));           break;
    }
}

void DeviceItemWidget::enterEvent(QEvent *)
{
    if (m_info->paired && m_info->state != BluetoothMainWidget::DeviceInfo::Connecting)
        m_removeBtn->show();
}

void DeviceItemWidget::leaveEvent(QEvent *)
{
    m_removeBtn->hide();
}

void DeviceItemWidget::mouseReleaseEvent(QMouseEvent *)
{
    if (m_info->state == BluetoothMainWidget::DeviceInfo::Disconnected) {
        m_loadingIndicator->show();
        m_removeBtn->hide();
        m_info->adapterInfo->bluetoothDbus->ConnectDevice(QDBusObjectPath(m_info->path));
    }
}

void DeviceItemWidget::disConnect()
{
    ConfrimWidget *confirm = new ConfrimWidget;
    m_info->adapterInfo->widget->addConfirm(confirm, m_info);
//    m_info->adapterInfo->bluetoothDbus->DisconnectDevice(QDBusObjectPath(m_info->path));

    connect(confirm, &ConfrimWidget::ignore, confirm, &ConfrimWidget::deleteLater, Qt::QueuedConnection);
    connect(confirm, &ConfrimWidget::disconnect, confirm, &ConfrimWidget::deleteLater, Qt::QueuedConnection);

    connect(confirm, &ConfrimWidget::ignore, [this] {
        m_info->adapterInfo->bluetoothDbus->RemoveDevice(QDBusObjectPath(m_info->adapterInfo->path), QDBusObjectPath(m_info->path));
//        m_info->adapterInfo->bluetoothDbus->
    });
    connect(confirm, &ConfrimWidget::disconnect, [this] {
        m_info->adapterInfo->bluetoothDbus->DisconnectDevice(QDBusObjectPath(m_info->path));
    });
}
