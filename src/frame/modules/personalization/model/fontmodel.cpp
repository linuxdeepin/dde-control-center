// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
        Q_EMIT listChanged(list);
    }
}
void FontModel::setFontName(const QString &name)
{
    if (m_fontName != name) {
        m_fontName = name;
        Q_EMIT defaultFontChanged(name);
    }
}
