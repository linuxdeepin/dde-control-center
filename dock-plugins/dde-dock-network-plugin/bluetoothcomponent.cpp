#include <QIcon>
#include <QLabel>
#include <QFrame>
#include <QHBoxLayout>

#include <dock/dockconstants.h>

#include "bluetoothcomponent.h"
#include "widgets/switchbutton.h"

BluetoothComponent::BluetoothComponent(QObject *parent) :
    QObject(parent)
{
    QIcon::setThemeName("Deepin");

    m_item = new QLabel;
    m_item->setFixedSize(Dock::APPLET_CLASSIC_ICON_SIZE, Dock::APPLET_CLASSIC_ICON_SIZE);
    m_item->setPixmap(QIcon::fromTheme("bluetooth-active-symbolic").pixmap(m_item->size()));

    m_label = new QLabel("hualet-pc");
    m_switchButton = new SwitchButton;

    m_appletFrame = new QFrame;

    QHBoxLayout * layout = new QHBoxLayout(m_appletFrame);

    layout->addWidget(m_label);
    layout->addStretch();
    layout->addWidget(m_switchButton);

    m_appletFrame->adjustSize();
}

BluetoothComponent::~BluetoothComponent()
{
    m_item->deleteLater();
    m_appletFrame->deleteLater();
}

QString BluetoothComponent::getUUID()
{
    return "uuid_bluetooth";
}

QString BluetoothComponent::getTitle()
{
    return "Bluetooth";
}

QWidget * BluetoothComponent::getItem()
{
    return m_item;
}

QWidget * BluetoothComponent::getApplet()
{
    return m_appletFrame;
}

QString BluetoothComponent::getMenuContent()
{
    return "";
}

void BluetoothComponent::invokeMenuItem(QString itemId, bool checked)
{

}
