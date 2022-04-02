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

#ifndef INDEXFRAME_H
#define INDEXFRAME_H

#include "interface/namespace.h"
#include <QRect>
#include <QObject>
#include <QFrame>

namespace DCC_NAMESPACE {

class IndexFrame : public QFrame
{
    Q_OBJECT
public:
    explicit IndexFrame(QFrame* parent = nullptr);
    void setLetters(const QStringList& lists);

protected:
    void paintEvent(QPaintEvent* );
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void mouseMoveEvent(QMouseEvent* e);
    void resizeEvent(QResizeEvent *);
    void leaveEvent(QEvent *event);

Q_SIGNALS:
    void click(const QString& ch);

private:
    QVector<QString> m_letters;
    int m_w;
    int m_index;
    QRect m_rect;
    bool m_pressed;
};

}
#endif // INDEXFRAME_H
