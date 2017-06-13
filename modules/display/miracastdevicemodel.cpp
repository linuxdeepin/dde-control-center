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
    m_sinkList.clear();
}

const QList<SinkInfo> MiracastDeviceModel::sinkList() const
{
    return m_sinkList;
}

const LinkInfo MiracastDeviceModel::linkInfo() const
{
    return m_linkInfo;
}

bool MiracastDeviceModel::isConnected() const
{
    for (const SinkInfo &info : m_sinkList)
        if (info.m_connected)
            return true;

    return false;
}

void dcc::display::MiracastDeviceModel::onSinkAdded(const SinkInfo &sinkinfo)
{
    if (m_sinkList.contains(sinkinfo))
        return;

    m_sinkList.append(sinkinfo);

    emit addSink(sinkinfo);
}

void dcc::display::MiracastDeviceModel::onSinkRemoved(const SinkInfo &sinkinfo)
{
    m_sinkList.removeOne(sinkinfo);
    emit removeSink(sinkinfo);
}

void MiracastDeviceModel::onSinkConnect(const QDBusObjectPath &sinkPath, bool connected)
{
    for (SinkInfo &info : m_sinkList)
        if (info.m_sinkPath == sinkPath)
            info.m_connected = connected;

    emit connectStateChanged(sinkPath, connected);
}

void MiracastDeviceModel::onLinkManageChanged(const bool state)
{
    if (m_linkInfo.m_managed == state)
        return;

    m_linkInfo.m_managed = state;
    emit linkManageChanged(state);
}
