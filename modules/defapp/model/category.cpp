#include "category.h"
#include <QDebug>
using namespace dcc;
using namespace dcc::defapp;
Category::Category(QObject *parent)
    : QObject(parent)
{

}

void Category::setuserList(const QList<QJsonObject> &list)
{
    if (m_userlist != list && list.size() != 0) {
        m_userlist = list;
        emit userItemChanged(m_userlist);
    }
}

void Category::setappList(const QList<QJsonObject> &list)
{
    if (m_applist != list && list.size() != 0) {
        m_applist = list;
        emit itemsChanged(list);
    }
}

void Category::setDefault(const QString &id)
{
    if (m_id != id && id.size() != 0) {
        m_id = id;
    }
    emit defaultChanged(id);
}

void Category::addUserItem(const QJsonObject &value)
{
    if(!m_userlist.contains(value)) {
        m_userlist.insert(m_userlist.end(), value);
        emit AdduserItem(value);
    }
}

void Category::delUserItem(const QJsonObject &value)
{
    m_userlist.removeOne(value);
}

