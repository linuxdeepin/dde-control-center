// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <QListView>

#include "monitoritemdelegate.h"

class MonitorListView : public QListView
{
    Q_OBJECT

public:
    explicit MonitorListView(QWidget *parent = 0);

    void setModel(QAbstractItemModel *model) override;

    int getStandardItemHeight() const;
    int getExpandItemHeight() const;
    int getItemSpacing() const;

protected:
    void rowsInserted(const QModelIndex &parent, int start, int end) override;
    bool viewportEvent(QEvent *event) override;

private:
    MonitorItemDelegate *m_delegate;
};
