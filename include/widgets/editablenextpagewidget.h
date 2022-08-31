// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef EDITABLENEXTPAGEWIDGET_H
#define EDITABLENEXTPAGEWIDGET_H

#include "widgets/settingsitem.h"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>

#include <DIconButton>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace widgets {

class EditableNextPageWidget : public SettingsItem
{
    Q_OBJECT

public:
    explicit EditableNextPageWidget(QWidget *parent = 0);

    void setTitle(const QString &title);
    void setIcon(const QPixmap &icon);

Q_SIGNALS:
    void acceptNextPage() const;
    void selected() const;
    void textChanged(const QString &oldText, const QString &newText) const;

private:
    void mousePressEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private Q_SLOTS:
    void toEditMode();
    void toEchoMode();

private:
    QLabel *m_name;
    QLabel *m_selectIcon;
    QLineEdit *m_edit;
    DIconButton *m_editButton;
    DIconButton *m_nextButton;
};

}

}

#endif // EDITABLENEXTPAGEWIDGET_H
