// SPDX-FileCopyrightText: 2016 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include <DListView>

#include "pluginitemdelegate.h"

class PluginListView : public DListView
{
    Q_OBJECT

public:
    explicit PluginListView(QWidget *parent = 0);

    void setModel(QAbstractItemModel *model) override;

    int getItemHeight() const;
    int getItemSpacing() const;

    inline void setItemHeight(int height) { m_delegate->setItemHeight(height); }
    inline void setItemSpacing(int spacing) { m_delegate->setItemSpacing(spacing); }

    inline void setBeginningItemStyle(QStyleOptionViewItem::ViewItemPosition viewItemPosition) { m_delegate->setBeginningItemStyle(viewItemPosition); }
    inline void setMiddleItemStyle(QStyleOptionViewItem::ViewItemPosition viewItemPosition) { m_delegate->setMiddleItemStyle(viewItemPosition); }
    inline void setEndItemStyle(QStyleOptionViewItem::ViewItemPosition viewItemPosition) { m_delegate->setEndItemStyle(viewItemPosition); }

protected:
    void rowsInserted(const QModelIndex &parent, int start, int end) override;
    bool viewportEvent(QEvent *event) override;

private:
    PluginItemDelegate *m_delegate;
};


