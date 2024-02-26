//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef LISTVIEW_H
#define LISTVIEW_H
#include "interface/namespace.h"
#include <qglobal.h>
#include <QAbstractItemView>

namespace DCC_NAMESPACE {

class ListViewPrivate;
class ListView : public QAbstractItemView
{
    Q_OBJECT
    Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
    Q_PROPERTY(QSize gridSize READ gridSize WRITE setGridSize)
    Q_PROPERTY(ViewMode viewMode READ viewMode WRITE setViewMode)
    Q_PROPERTY(Qt::Alignment alignment READ alignment WRITE setAlignment)

public:
    enum ViewMode { ListMode,
                    IconMode };
    Q_ENUM(ViewMode)

    explicit ListView(QWidget *parent = nullptr);
    virtual ~ListView() override;

    void setSpacing(int space);
    int spacing() const;

    void setGridSize(const QSize &size);
    QSize gridSize() const;

    void setViewMode(ViewMode mode);
    ViewMode viewMode() const;

    void setAlignment(Qt::Alignment alignment);
    Qt::Alignment alignment() const;

    void setContentsMargins(int left, int top, int right, int bottom);
    void setContentsMargins(const QMargins &margins);
    QMargins contentsMargins() const;

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

    void mouseMoveEvent(QMouseEvent *event) override;
    void verticalScrollbarValueChanged(int value) override;
    void horizontalScrollbarAction(int value) override;
    void setHoverIndexAt(const QPoint &p);

protected:
    void paintEvent(QPaintEvent *e) override;
    bool viewportEvent(QEvent *event) override;

    DCC_DECLARE_PRIVATE(ListView)
};

} // namespace DCC_NAMESPACE

#endif // LISTVIEW_H
