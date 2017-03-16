#include "thememodel.h"

using namespace dcc;
using namespace dcc::personalization;

ThemeModel::ThemeModel(QObject * parent) : QObject(parent)
{
}

void ThemeModel::addItem(const QString &id, const QJsonObject &json)
{
    if (m_list.contains(id))
        return;

    m_list.insert(id, json);
    emit itemAdded(json);
}

void ThemeModel::setDefault(const QString &value)
{
    m_default = value;
    emit defaultChanged(value);
}

QMap<QString, QString> ThemeModel::getPicList() const
{
    return m_picList;
}

void ThemeModel::addPic(const QString &id, const QString &picPath)
{
    m_picList.insert(id, picPath);
    emit picAdded(id, picPath);
}

void ThemeModel::removeItem(const QString &id)
{
    m_list.remove(id);
    emit itemRemoved(id);
}
