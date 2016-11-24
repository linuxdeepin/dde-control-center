#include "category.h"

Category::Category(QObject *parent)
{
    Q_UNUSED(parent);
}

void Category::setuserList(const QList<QJsonObject> &list)
{
    m_userlist = list;
    emit userItemChanged();
}

void Category::setappList(const QList<QJsonObject> &list)
{
    m_applist = list;
    emit itemsChanged();
}

void Category::setDefault(const QString &id)
{
    if (m_id != id) {
        m_id = id;
    }
    emit defaultChanged(id);
}
