//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "widgets/settingsitem.h"
#include <dtkwidget_global.h>
#include <QLabel>

DWIDGET_BEGIN_NAMESPACE
class DSwitchButton;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QHBoxLayout;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {

class SwitchLabel : public QLabel
{
    Q_OBJECT

public:
    explicit SwitchLabel(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    inline QSize actualSize() { return m_actualSize; }

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    QSize m_actualSize;
    QString m_sourceText;
};

class SwitchWidget : public SettingsItem
{
    Q_OBJECT

public:
    // explicit SwitchWidget(QWidget *parent = nullptr);
    explicit SwitchWidget(const QString &title, QWidget *parent = nullptr);
    explicit SwitchWidget(QWidget *parent = nullptr, QWidget *widget = nullptr);

    void setChecked(const bool checked = true);
    QString title() const;
    void setTitle(const QString &title);
    bool checked() const;

    void setLeftWidget(QWidget *widget);
    QWidget *leftWidget() const { return m_leftWidget; }
    QHBoxLayout* getMainLayout() { return m_mainLayout; }

public:
    inline DTK_WIDGET_NAMESPACE::DSwitchButton *switchButton() const { return m_switchBtn; }

Q_SIGNALS:
    void checkedChanged(const bool checked) const;
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    void init();

    QWidget *m_leftWidget;
    Dtk::Widget::DSwitchButton *m_switchBtn;
    QHBoxLayout *m_mainLayout;
};

}
