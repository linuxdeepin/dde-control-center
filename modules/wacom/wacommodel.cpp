#include "wacommodel.h"
#include "wacom/model/wacommodelbase.h"
#include <QObject>
#include <QDebug>
using namespace dcc;
using namespace dcc::wacom;

WacomModel::WacomModel(QObject *parent):QObject(parent)
{
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

void WacomModel::setIsExist(bool state)
{
    if (m_exist != state) {
        m_exist = state;
    }
    emit existChanged(state);
}
