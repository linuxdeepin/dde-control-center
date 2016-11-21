#include "wacommodel.h"

WacomModel::WacomModel(QObject *parent)
{
    Q_UNUSED(parent);
    m_wacomModelBase = new WacomModelBase;
}

WacomModel::~WacomModel()
{
    m_wacomModelBase->deleteLater();
}

WacomModelBase *WacomModel::getWacomModelBase()
{
    return m_wacomModelBase;
}
