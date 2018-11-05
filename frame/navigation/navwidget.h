/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             listenerri <listenerri@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             listenerri <listenerri@gmail.com>
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

#ifndef NAVWIDGET_H
#define NAVWIDGET_H

#include "navmodel.h"

#include <QTableView>

class NavWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NavWidget(QWidget *parent = 0);

Q_SIGNALS:
    void requestModule(const QString &module) const;
    void requestModuleVisible(const QString &module, bool visible);

protected:
    void leaveEvent(QEvent *e) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onNavEnter(const QModelIndex &index);
    void onNavClicked(const QModelIndex &index);

private:
    QStringList getValidModuleList();

private:
    QTableView *m_navView;
    NavModel *m_navModel;
};

#endif // NAVWIDGET_H
