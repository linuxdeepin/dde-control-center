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

#ifndef CATEGORY_H
#define CATEGORY_H
#include <QObject>
#include <QList>
#include <QJsonObject>
namespace dcc
{
namespace defapp
{
struct App {
    QString Id;
    QString Name;
    QString DisplayName;
    QString Description;
    QString Icon;
    QString Exec;
    bool isUser;

    bool operator ==(const App &app) const {
        return app.Id == Id;
    }

    bool operator !=(const App &app) const {
        return app.Id != Id;
    }
};

class Category : public QObject
{
    Q_OBJECT

public:
    explicit Category(QObject *parent = 0);

    void setappList(const QList<App> &list);   //设置list到暂存区
    void setuserList(const QList<App> &list);
    void setDefault(const App &def);

    const QString getName() const { return m_category;}
    void setCategory(const QString &category);
    inline const QList<App> getappItem() const { return m_applist;}
    inline const App getDefault() { return m_default;}
    void addUserItem(const App &value);
    void delUserItem(const App &value);

signals:
    void itemsChanged(const QList<App> &list);
    void defaultChanged(const App &id);
    void AdduserItem(const App &app);
    void categoryNameChanged(const QString &name);

private:
    QList<App> m_applist;
    QString m_category;
    App m_default;
};
}
}

#endif // CATEGORY_H
