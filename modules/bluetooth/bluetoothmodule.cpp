#include "bluetoothmodule.h"
#include "bluetoothwidget.h"
#include "bluetoothmodel.h"
#include "bluetoothworker.h"

#include "contentwidget.h"

namespace dcc {
namespace bluetooth {

BluetoothModule::BluetoothModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
    ModuleInterface(frame),

    m_bluetoothView(nullptr),
    m_bluetoothModel(nullptr),
    m_bluetoothWorker(nullptr)
{

}

BluetoothModule::~BluetoothModule()
{
    if (m_bluetoothView)
        m_bluetoothView->deleteLater();
}

void BluetoothModule::initialize()
{
    m_bluetoothModel = new BluetoothModel;
    m_bluetoothWorker = new BluetoothWorker(m_bluetoothModel);

    m_bluetoothModel->moveToThread(qApp->thread());
    m_bluetoothWorker->moveToThread(qApp->thread());
}

void BluetoothModule::moduleActive()
{
    m_bluetoothWorker->activate();
}

void BluetoothModule::moduleDeactive()
{
    m_bluetoothWorker->deactivate();
}

void BluetoothModule::reset()
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
        m_bluetoothView = new BluetoothWidget(m_bluetoothModel);
        m_bluetoothView->setTitle("Bluetooth");

        connect(m_bluetoothView, &BluetoothWidget::requestToggleAdapter, m_bluetoothWorker, &BluetoothWorker::setAdapterPowered);
        connect(m_bluetoothView, &BluetoothWidget::requestConnectDevice, m_bluetoothWorker, &BluetoothWorker::connectAdapterDevice);
    }

    return m_bluetoothView;
}

}
}
