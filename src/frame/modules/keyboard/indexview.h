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

#ifndef INDEXVIEW_H
#define INDEXVIEW_H

#include <QListView>
#include <QLabel>

namespace dcc {
namespace keyboard{
class IndexView : public QListView
{
    Q_OBJECT

public:
    explicit IndexView(QWidget* parent = 0);

public Q_SLOTS:
    void onClick(const QString& ch);

protected:
//    void paintEvent(QPaintEvent* e);
//    void wheelEvent(QWheelEvent *e);
    void showEvent(QShowEvent* e);
    void scrollContentsBy(int dx, int dy);

private:
    QString m_section;
    QLabel* m_label;
};
}
}
#endif // INDEXVIEW_H
