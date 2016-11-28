#include "thememodel.h"

ThemeModel::ThemeModel(QObject * parent) : QObject(parent)
{
}

void ThemeModel::setJson(const QList<QJsonObject> &json)
{
    if (m_list != json) {
        m_list = json;
        emit listChanged(json);
    }
}

void ThemeModel::setDefault(const QString &value)
{
    m_default = value;
    emit defaultChanged(value);
}
