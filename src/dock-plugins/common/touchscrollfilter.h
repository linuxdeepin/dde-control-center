// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TOUCHSCROLLFILTER_H
#define TOUCHSCROLLFILTER_H

#include <QObject>
#include <QPointF>
#include <QAbstractScrollArea>

class TouchScrollFilter : public QObject
{
public:
    explicit TouchScrollFilter(QAbstractScrollArea *area);
    ~TouchScrollFilter() override = default;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QAbstractScrollArea *m_area;
    QPointF m_lastPos;
    bool m_isDrag;
};

#endif // TOUCHSCROLLFILTER_H
