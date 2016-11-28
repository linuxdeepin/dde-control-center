#include "displaymodel.h"

DisplayModel::DisplayModel(QObject *parent)
    : QObject(parent)
{

}

void DisplayModel::monitorAdded(Monitor *mon)
{
    m_monitors.append(mon);

    emit monitorListChanged();
}
