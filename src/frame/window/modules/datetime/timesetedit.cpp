// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "timesetedit.h"
#include <QMouseEvent>

using namespace DCC_NAMESPACE::datetime;

TimeSetEdit::TimeSetEdit(QWidget *parent) : QLabel(parent)
{

}

void TimeSetEdit::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        Q_EMIT notifyClicked();
    }
}

TimeSetLineEdit::TimeSetLineEdit(QWidget *parent)
    : QLineEdit (parent)
{

}

void TimeSetLineEdit::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        Q_EMIT notifyClicked();
    }
}
