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

#ifndef THEMEMODEL_H
#define THEMEMODEL_H

#include <QObject>
#include <QMap>
#include <QJsonObject>
#include <QDebug>

namespace dcc
{
namespace personalization
{
class ThemeModel : public QObject
{
    Q_OBJECT
public:
    explicit ThemeModel(QObject *parent = 0);

    void addItem(const QString &id, const QJsonObject &json);
    QMap<QString, QJsonObject> getList() {return m_list;}

    void setDefault(const QString &value);
    inline QString getDefault() {return m_default;}

    QMap<QString, QString> getPicList() const;
    void addPic(const QString &id, const QString &picPath);

    void removeItem(const QString &id);

Q_SIGNALS:
    void itemAdded(const QJsonObject &json);
    void defaultChanged(const QString &value);
    void picAdded(const QString &id, const QString &picPath);
    void itemRemoved(const QString &id);

private:
    QMap<QString, QJsonObject> m_list;
    QString m_default;
    QMap<QString, QString> m_picList;
};
}
}


#endif // THEMEMODEL_H
