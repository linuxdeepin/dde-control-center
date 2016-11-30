#include "displaymodel.h"

DisplayModel::DisplayModel(QObject *parent)
    : QObject(parent)
{

}

Monitor *DisplayModel::primaryMonitor() const
{
    for (auto mon : m_monitors)
        if (mon->name() == m_primary)
            return mon;

    return nullptr;
}

void DisplayModel::setScreenHeight(const int h)
{
    if (m_screenHeight != h)
    {
        m_screenHeight = h;
        emit screenHeightChanged(m_screenHeight);
    }
}

void DisplayModel::setScreenWidth(const int w)
{
    if (m_screenWidth != w)
    {
        m_screenWidth = w;
        emit screenWidthChanged(m_screenWidth);
    }
}

void DisplayModel::setPrimary(const QString &primary)
{
    if (m_primary != primary)
    {
        m_primary = primary;
        emit primaryScreenChanged(m_primary);
    }
}

void DisplayModel::monitorAdded(Monitor *mon)
{
    m_monitors.append(mon);

    emit monitorListChanged();
}
