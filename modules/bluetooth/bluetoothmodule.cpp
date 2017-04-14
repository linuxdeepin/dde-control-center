#include "bluetoothmodule.h"
#include "bluetoothwidget.h"
#include "bluetoothmodel.h"
#include "bluetoothworker.h"
#include "adapterwidget.h"
#include "contentwidget.h"
#include "detailpage.h"

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

void BluetoothModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName)

    showBluetoothDetail(m_bluetoothModel->adapters().first());
}

void BluetoothModule::showDetail(const Adapter *adapter, const Device *device)
{
    DetailPage *page = new DetailPage(adapter, device);

    connect(page, &DetailPage::requestIgnoreDevice, m_bluetoothWorker, &BluetoothWorker::ignoreDevice);
    connect(page, &DetailPage::requestDisconnectDevice, m_bluetoothWorker, &BluetoothWorker::disconnectDevice);

    m_frameProxy->pushWidget(this, page);
}

void BluetoothModule::showBluetoothDetail(const Adapter *adapter)
{
    AdapterWidget *w = new AdapterWidget(adapter);

    connect(w, &AdapterWidget::requestSetToggleAdapter, m_bluetoothWorker, &BluetoothWorker::setAdapterPowered);
    connect(w, &AdapterWidget::requestConnectDevice, m_bluetoothWorker, &BluetoothWorker::connectDevice);
    connect(w, &AdapterWidget::requestShowDetail, this, &BluetoothModule::showDetail);
    connect(w, &AdapterWidget::requestSetAlias, m_bluetoothWorker, &BluetoothWorker::setAlias);

    m_frameProxy->pushWidget(this, w);
}

BluetoothModule::~BluetoothModule()
{
    m_bluetoothModel->deleteLater();
    m_bluetoothWorker->deleteLater();
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
    // refresh the view
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
        m_bluetoothView->setTitle(tr("Bluetooth"));
        connect(m_bluetoothView, &BluetoothWidget::showBluetoothDetail, this, &BluetoothModule::showBluetoothDetail);
        connect(m_bluetoothView, &BluetoothWidget::requestModuleVisible, [this](const bool visible) { m_frameProxy->setModuleVisible(this, visible); });
    }

    return m_bluetoothView;
}

}
}
