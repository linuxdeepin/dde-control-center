#include "wacomworker.h"
#include "wacommodel.h"
#include "model/wacommodelbase.h"

using namespace dcc;
using namespace dcc::wacom;

const QString Service = "com.deepin.daemon.InputDevices";

WacomWorker::WacomWorker(WacomModel *model, QObject *parent) :
    QObject(parent),
    m_dbusWacom(new Wacom(Service, "/com/deepin/daemon/InputDevice/Wacom", QDBusConnection::sessionBus(), this)),
    m_model(model)
{
    connect(m_dbusWacom, &Wacom::StylusPressureSensitiveChanged, this, &WacomWorker::setPressureSensitive);
    connect(m_dbusWacom, &Wacom::ExistChanged, m_model, &WacomModel::setExist);

    //get real data
    m_model->setExist(m_dbusWacom->exist());
    m_dbusWacom->setSync(false);
}

void WacomWorker::active()
{
    m_dbusWacom->blockSignals(false);

    WacomModelBase *ModelBase = m_model->getWacomModelBase();
    ModelBase->setPressureValue(m_dbusWacom->stylusPressureSensitive());
    m_model->setExist(m_dbusWacom->exist());
}

void WacomWorker::deactive()
{
    m_dbusWacom->blockSignals(true);
}

void WacomWorker::setPressureSensitive(const int value)
{
    WacomModelBase *ModelBase = m_model->getWacomModelBase();
    ModelBase->setPressureValue(value);
}

void WacomWorker::onPressureSensitiveChanged(const int value)
{
    m_dbusWacom->setStylusPressureSensitive(value);
    m_dbusWacom->setEraserPressureSensitive(value);
}

bool WacomWorker::exist()
{
    return m_dbusWacom->exist();
}
