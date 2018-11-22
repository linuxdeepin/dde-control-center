/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
