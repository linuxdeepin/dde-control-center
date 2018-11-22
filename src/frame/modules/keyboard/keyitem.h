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

#ifndef KEYITEM_H
#define KEYITEM_H

#include <QObject>
#include <QRect>
#include <QPainter>

namespace dcc {
namespace keyboard{

class KeyItem
{
public:
    explicit KeyItem(int row, int col, const QString& main, const QString& vice = QString());
    explicit KeyItem(int row, int col, int key, const QString& main, const QString& vice = QString());
    virtual ~KeyItem();
    void setMainKey(const QString& key);
    QString mainKey() const;

    void setViceKey(const QString& key);
    QString viceKey() const;

    void setPress(bool press);
    bool press() const { return m_press; }

    void setConflict(bool conflict);

    QRect rect() const { return m_rect; }
    quint32 keycode() const { return m_keycode;}
    bool modifies() const { return m_modifies;}

    static QList<KeyItem*> keyboards();
    static void deleteItems();
public:
    virtual void paint(QPainter* painter, const QRect& rect);

private:
    QString m_mainKey;
    QString m_viceKey;
    QPixmap m_mainPix;
    int m_row;
    int m_col;
    quint32 m_keycode;
    QRect m_rect;
    bool m_modifies;
    bool m_press;
    static QList<KeyItem*> m_keys;
    bool m_conflict;
};
}
}

#endif // KEYITEM_H
