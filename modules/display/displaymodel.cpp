#include "displaymodel.h"

DisplayModel::DisplayModel(QObject *parent)
    : QObject(parent)
{

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

void DisplayModel::monitorAdded(Monitor *mon)
{
    m_monitors.append(mon);

    emit monitorListChanged();
}
