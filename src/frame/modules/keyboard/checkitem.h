// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef CHECKITEM_H
#define CHECKITEM_H

#include "widgets/settingsitem.h"
#include <DIconButton>
#include <QPushButton>
#include <QLabel>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace keyboard{
class CheckItem : public dcc::widgets::SettingsItem
{
    Q_OBJECT

public:
    explicit CheckItem(QFrame *parent = 0);
    void setTitle(const QString& title);
    QString title() const;

    void setChecked(bool checked);
    void setMultipleMode(bool multiple);
    bool checked() const;

Q_SIGNALS:
    void checkedChanged(const QString &title);
    void destroySelf(CheckItem* item);

public Q_SLOTS:
    void onEditMode(bool value);
    void onClick();

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    bool m_checked;
    bool m_bEdit;
    QLabel *m_title;
    DIconButton *m_checkBtn;
    int m_circle;
    bool m_multiple;
};
}
}
#endif // CHECKITEM_H
