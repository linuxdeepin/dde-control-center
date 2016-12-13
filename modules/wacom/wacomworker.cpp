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
    m_dbusWacom->setSync(false);
    connect(m_dbusWacom, &Wacom::PressureSensitiveChanged, this, &WacomWorker::setPressureSensitive);
    connect(m_dbusWacom, &Wacom::ExistChanged, m_model, &WacomModel::setIsExist);
    active();
    m_model->setIsExist(m_dbusWacom->exist());
}

void WacomWorker::active()
{
    m_dbusWacom->blockSignals(false);
    WacomModelBase *ModelBase = m_model->getWacomModelBase();
    ModelBase->setPressureValue(converToModelPressureValue(m_dbusWacom->pressureSensitive()));
}

void WacomWorker::deactive()
{
    m_dbusWacom->blockSignals(true);
}

void WacomWorker::setPressureSensitive(const int value)
{
    WacomModelBase *ModelBase = m_model->getWacomModelBase();
    ModelBase->setPressureValue(converToModelPressureValue(value));
}

void WacomWorker::onPressureSensitiveChanged(const int value)
{
    m_dbusWacom->setPressureSensitive(converToPressureValue(value));
}

int WacomWorker::converToPressureValue(int value)
{
    return value + 2;
}

int WacomWorker::converToModelPressureValue(int value)
{
    return value - 2;
}
