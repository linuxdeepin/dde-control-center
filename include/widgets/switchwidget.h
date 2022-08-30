// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SWITCHWIDGET_H
#define SWITCHWIDGET_H

#include "widgets/settingsitem.h"

#include <QHBoxLayout>
#include <QLabel>
#include <dswitchbutton.h>

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {

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
}

#endif // SWITCHWIDGET_H
