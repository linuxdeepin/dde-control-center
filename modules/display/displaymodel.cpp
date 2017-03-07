#include "displaymodel.h"

using namespace dcc;
using namespace dcc::display;

bool contains(const QList<Resolution> &container, const Resolution &item)
{
    for (auto r : container)
        if (r.width() == item.width() && r.height() == item.height())
            return true;

    return false;
}

DisplayModel::DisplayModel(QObject *parent)
    : QObject(parent)
{

}

const QList<Resolution> DisplayModel::monitorsSameModeList() const
{
    Q_ASSERT(m_monitors.size() > 1);

    QList<Resolution> resultList = m_monitors.first()->modeList();
    for (int i(1); i != m_monitors.size(); ++i)
    {
        const QList<Resolution> originList = m_monitors[i]->modeList();
        QList<Resolution> filteredList;

        for (auto r : resultList)
            if (contains(originList, r))
                filteredList.append(r);
        resultList = filteredList;
    }

    return resultList;
}

Monitor *DisplayModel::primaryMonitor() const
{
    for (auto mon : m_monitors)
        if (mon->name() == m_primary)
            return mon;

    return nullptr;
}

bool DisplayModel::monitorsIsIntersect() const
{
    if (m_monitors.size() < 2)
        return false;

    // only support 2 screens
    Q_ASSERT(m_monitors.size() == 2);

    return m_monitors.first()->rect().intersects(m_monitors.last()->rect());
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

void DisplayModel::setDisplayMode(const int mode)
{
    if (m_mode != mode)
    {
        m_mode = mode;
        emit displayModeChanged(m_mode);
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

void DisplayModel::setCurrentConfig(const QString &config)
{
    if (m_currentConfig != config)
    {
        m_currentConfig = config;
        emit currentConfigChanged(m_currentConfig);
    }
}

void DisplayModel::setConfigList(const QStringList &configList)
{
    if (m_configList != configList)
    {
        m_configList = configList;
        emit configListChanged(m_configList);
    }
}

void DisplayModel::monitorAdded(Monitor *mon)
{
    m_monitors.append(mon);

    emit monitorListChanged();
}

void DisplayModel::monitorRemoved(Monitor *mon)
{
    m_monitors.removeOne(mon);

    emit monitorListChanged();
}
