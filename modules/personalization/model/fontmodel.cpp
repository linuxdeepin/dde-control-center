#include "fontmodel.h"

using namespace dcc;
using namespace dcc::personalization;

FontModel::FontModel(QObject *parent) : QObject(parent)
{

}

void FontModel::setFontList(const QList<QJsonObject> &list)
{
    if (m_list != list) {
        m_list = list;
        emit listChanged(list);
    }
}
void FontModel::setFontName(const QString &name)
{
    if (m_fontName != name) {
        m_fontName = name;
        emit defaultFontChanged(name);
    }
}
