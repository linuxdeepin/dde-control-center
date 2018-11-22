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

#ifndef DATETIMEWIDGET_H
#define DATETIMEWIDGET_H

#include <QFrame>
#include <QLineEdit>
#include <dimagebutton.h>

#include "widgets/settingsitem.h"
#include "widgets/labels/normallabel.h"

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;

namespace dcc {
namespace datetime {

class DateWidget : public SettingsItem
{
    Q_OBJECT

public:
    enum Type{
        Year,
        Month,
        Day
    };

public:
    explicit DateWidget(Type type, int minimum, int maximum, QFrame *parent = 0);

    int value() const;
    void setValue(const int &value);

    int minimum() const;
    int maximum() const;
    void setRange(int minimum, int maximum);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

Q_SIGNALS:
    void editingFinished();

public Q_SLOTS:
    void slotAdd();
    void slotReduced();

    void fixup();

private:
    Type m_type;

    int m_minimum;
    int m_maximum;

    QLineEdit *m_lineEdit;
    NormalLabel *m_label;
    DImageButton* m_addBtn;
    DImageButton* m_reducedBtn;
};

class TimeWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit TimeWidget(QFrame* parent = 0);

    int hour() const;
    int minute() const;

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    QLineEdit *m_hourEdit;
    QLineEdit *m_minuteEdit;
};
}
}

#endif // DATETIMEWIDGET_H
