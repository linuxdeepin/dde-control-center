//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ACCOUNTSLISTVIEW_H
#define ACCOUNTSLISTVIEW_H
#include "interface/namespace.h"

#include <QListView>
class AccountsListViewPrivate;
class AccountsListView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit AccountsListView(QWidget *parent = nullptr);
    virtual ~AccountsListView() override;


    void setModel(QAbstractItemModel *model) override;
    QRect visualRect(const QModelIndex &index) const override;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;
    QModelIndex indexAt(const QPoint &p) const override;

protected:
    QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    int horizontalOffset() const override;
    int verticalOffset() const override;

    bool isIndexHidden(const QModelIndex &index) const override;
    QRegion visualRegionForSelection(const QItemSelection &selection) const override;
    void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command) override;

    void updateGeometries() override;

    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>()) override;
    void rowsInserted(const QModelIndex &parent, int start, int end) override;
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end) override;

protected:
    void paintEvent(QPaintEvent *e) override;
    bool viewportEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *e) override;

    DCC_DECLARE_PRIVATE(AccountsListView)
};

#endif // ACCOUNTSLISTVIEW_H
