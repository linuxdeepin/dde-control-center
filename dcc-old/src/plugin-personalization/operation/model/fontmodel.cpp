//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "fontmodel.h"

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
