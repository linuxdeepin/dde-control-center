#include "networkconst.h"

namespace dde {

namespace network {

Connection::Connection()
{
}

Connection::~Connection()
{
}

QString Connection::path()
{
    return m_data.value("Path").toString();
}

QString Connection::uuid()
{
    return m_data.value("Uuid").toString();
}

QString Connection::id()
{
    return m_data.value("Id").toString();
}

QString Connection::hwAddress()
{
    return m_data.value("HwAddress").toString();
}

QString Connection::clonedAddress()
{
    return m_data.value("ClonedAddress").toString();
}

QString Connection::ssid()
{
    return m_data.value("Ssid").toString();
}

void Connection::updateConnection(const QJsonObject &data)
{
    m_data = data;
}

Connection *ControllItems::connection() const
{
    return m_connection;
}

// 连接具体项的基类
ControllItems::ControllItems()
    : m_connection(new Connection)
{
}

ControllItems::~ControllItems()
{
    delete m_connection;
}

void ControllItems::setConnection(const QJsonObject &jsonObj)
{
    m_connection->updateConnection(jsonObj);
}

}

}
