#include "miracastdevicemodel.h"
#include <QDebug>

using namespace dcc;
using namespace dcc::display;

MiracastDeviceModel::MiracastDeviceModel(const LinkInfo &linkInfo, QObject *parent) :
    QObject(parent),
    m_linkInfo(linkInfo)
{

}

MiracastDeviceModel::~MiracastDeviceModel()
{
    qDeleteAll(m_sinkList);
}

const QList<MiracastItem*> MiracastDeviceModel::sinkList() const
{
    return m_sinkList;
}

MiracastItem *MiracastDeviceModel::itemByPath(const QString &path)
{
    for (MiracastItem *item : m_sinkList) {
        if (item->info().m_sinkPath.path() == path)
            return item;
    }

    return nullptr;
}

const LinkInfo MiracastDeviceModel::linkInfo() const
{
    return m_linkInfo;
}

bool MiracastDeviceModel::isConnected() const
{
    for (MiracastItem *item : m_sinkList) {
        if (item->info().m_connected)
            return true;
    }

    return false;
}

void dcc::display::MiracastDeviceModel::onSinkAdded(const SinkInfo &sinkinfo)
{
    MiracastItem *item = new MiracastItem(sinkinfo);
    m_sinkList.append(item);

    emit addItem(item);
}

void dcc::display::MiracastDeviceModel::onSinkRemoved(const SinkInfo &sinkinfo)
{
    MiracastItem *item = itemByPath(sinkinfo.m_linkPath.path());
    if (item) {
        m_sinkList.removeOne(item);
        emit removeItem(item);
    }
}

void MiracastDeviceModel::onSinkConnect(const QDBusObjectPath &sinkPath, bool connected)
{
    MiracastItem *item = itemByPath(sinkPath.path());
    if (item) {
        item->onConnectState(connected);
        emit connectStateChanged(connected);
    }
}

void MiracastDeviceModel::onLinkManageChanged(const bool state)
{
    if (m_linkInfo.m_managed == state)
        return;

    m_linkInfo.m_managed = state;
    emit linkManageChanged(state);
}

void MiracastDeviceModel::clear()
{
    for (MiracastItem *item : m_sinkList)
        emit removeItem(item);

    m_sinkList.clear();
}
