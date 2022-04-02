/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#ifndef DATEWIDGET_H
#define DATEWIDGET_H

#include "widgets/settingsitem.h"

#include <DIconButton>

class QLineEdit;
class QLabel;

class DateWidget : public DCC_NAMESPACE::SettingsItem
{
    Q_OBJECT
public:
    enum Type {
        Year,
        Month,
        Day
    };

public:
    explicit DateWidget(Type type, int minimum, int maximum, QWidget *parent = 0);

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
    QLineEdit *m_lineEdit;
    QLabel *m_label;
    DTK_WIDGET_NAMESPACE::DIconButton *m_addBtn;
    DTK_WIDGET_NAMESPACE::DIconButton *m_reducedBtn;
};
#endif // DATEWIDGET_H
