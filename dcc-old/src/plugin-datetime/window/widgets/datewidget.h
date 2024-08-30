//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

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
    void chenged();

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
