#include "wireddevice.h"

#include <QDebug>

using namespace dcc::network;

WiredDevice::WiredDevice(const QJsonObject &info, QObject *parent)
    : NetworkDevice(NetworkDevice::Wired, info, parent)
{

}

void WiredDevice::onConnectionInfoChanged(const QJsonObject &connInfo)
{
    m_wiredConnection = connInfo;
}

//void WiredDevice::onPPPInfoChanged(QList<QJsonObject> &pppInfoList)
//{
//    m_pppConnections.clear();

//    for (const auto &info : pppInfoList)
//    {
//        const QString hwAddr = info.value("HwAddress").toString();
//        if (hwAddr.isEmpty() || hwAddr == this->hwAddr())
//            m_pppConnections.append(info);
//    }

//    emit pppInfoListChanged(m_pppConnections);
//}
