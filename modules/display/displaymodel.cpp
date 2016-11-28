#include "displaymodel.h"

DisplayModel::DisplayModel(QObject *parent)
    : QObject(parent)
{

}

void DisplayModel::monitorAdded(MonitorInter *inter)
{
    m_monitors.append(inter);

    emit monitorListChanged();
}
