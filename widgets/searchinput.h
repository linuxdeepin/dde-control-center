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

#ifndef SEARCHINPUT_H
#define SEARCHINPUT_H

#include <QLineEdit>

namespace dcc {
namespace widgets {

class SearchInput : public QLineEdit
{
    Q_OBJECT
    Q_PROPERTY(QString icon READ iconPath WRITE setIcon)
public:
    explicit SearchInput(QWidget* parent = 0);
    void setSearchText(const QString& text);
    void setIconVisible(bool visible);
    QString iconPath() const;
    void setIcon(const QString &filepath);

protected:
    void paintEvent(QPaintEvent *);

private:
    bool m_iconVisible;
    QString m_search;
    QPixmap m_icon;
    QString m_iconPath;
};

}
}

#endif // SEARCHINPUT_H
