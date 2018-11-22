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

#ifndef FONTMODEL_H
#define FONTMODEL_H

#include <QObject>
#include <QList>
#include <QJsonObject>

namespace dcc
{
namespace personalization
{
class FontModel : public QObject
{
    Q_OBJECT
public:
    explicit FontModel(QObject *parent = 0);
    void setFontList(const QList<QJsonObject> &list);
    void setFontName(const QString &name);
    inline const QList<QJsonObject> getFontList() const {return m_list;}
    inline const QString getFontName() const {return m_fontName;}

Q_SIGNALS:
    void listChanged(const QList<QJsonObject> &list);
    void defaultFontChanged(const QString &name);

private:
    QList<QJsonObject> m_list;
    QString m_fontName;
};
}
}


#endif // FONTMODEL_H
