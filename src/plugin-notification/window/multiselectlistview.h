//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MULTISELECTLISTVIEW_H
#define MULTISELECTLISTVIEW_H

#include "interface/namespace.h"
#include <DListView>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

class MultiSelectListView : public DListView
{
    Q_OBJECT
public:
    explicit MultiSelectListView(QWidget *parent = nullptr);
    void resetStatus(const QModelIndex &index);

protected:
    void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    int m_currentIndex;
};

}

#endif // MULTISELECTLISTVIEW_H
