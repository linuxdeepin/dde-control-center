// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"
#include "timesetedit.h"

#include <DIconButton>

#include <QObject>
#include <QLineEdit>
#include <QEvent>

namespace dcc {
namespace widgets {
class NormalLabel;
}
}

namespace DCC_NAMESPACE {
namespace datetime {
class DateWidget : public dcc::widgets::SettingsItem
{
    Q_OBJECT
public:
    enum Type {
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
    const QString getCurrentText() const;
    void setCurrentText(QString text);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

Q_SIGNALS:
    void editingFinished();
    void notifyClickedState(bool);

public Q_SLOTS:
    void slotAdd();
    void slotReduced();
    void fixup();

private:
    Type m_type;
    int m_minimum;
    int m_maximum;
    TimeSetLineEdit *m_lineEdit;
    TimeSetEdit *m_label;
    DTK_WIDGET_NAMESPACE::DIconButton *m_addBtn;
    DTK_WIDGET_NAMESPACE::DIconButton *m_reducedBtn;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
