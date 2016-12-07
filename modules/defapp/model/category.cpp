#include "category.h"

Category::Category(QObject *parent)
    : QObject(parent)
{

}

void Category::setuserList(const QList<QJsonObject> &list)
{
    if (m_userlist != list && list.size() != 0) {
        m_userlist = list;
        emit userItemChanged(list);
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
