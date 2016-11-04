#include "bluetoothmodule.h"
#include "modulewidget.h"
#include "contentwidget.h"

BluetoothModule::BluetoothModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
    ModuleInterface(frame),

    m_bluetoothView(nullptr)
{

}

BluetoothModule::~BluetoothModule()
{
    if (m_bluetoothView)
        m_bluetoothView->deleteLater();
}

void BluetoothModule::initialize()
{

}

void BluetoothModule::moduleActive()
{

}

void BluetoothModule::moduleDeactive()
{

}

void BluetoothModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}

const QString BluetoothModule::name() const
{
    return QStringLiteral("bluetooth");
}

ModuleWidget *BluetoothModule::moduleWidget()
{
    if (!m_bluetoothView)
    {
        m_bluetoothView = new ModuleWidget;
        m_bluetoothView->setTitle("Bluetooth");
    }

    m_bluetoothView->setVisible(false);

    return m_bluetoothView;
}
