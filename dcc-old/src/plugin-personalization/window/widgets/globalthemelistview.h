//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef GLOBALTHEMELISTVIEW_H
#define GLOBALTHEMELISTVIEW_H
#include "interface/namespace.h"
#include <DListView>

class ThemeModel;
class GlobalThemeListViewPrivate;
class GlobalThemeListView : public QAbstractItemView
{
    Q_OBJECT
public:
    explicit GlobalThemeListView(QWidget *parent = nullptr);
    ~GlobalThemeListView();

    void setThemeModel(ThemeModel *model);
    void setSpacing(int space);
    int spacing() const;

    void setGridSize(const QSize &size);
    QSize gridSize() const;

    QRect visualRect(const QModelIndex &index) const override;
    void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible) override;
    QModelIndex indexAt(const QPoint &p) const override;

Q_SIGNALS:
    void applied(const QModelIndex &index);
    void previewed(const QModelIndex &index);

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
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    DCC_DECLARE_PRIVATE(GlobalThemeListView)
};

class GlobalThemeModel : public QAbstractItemModel
{
public:
    enum UserDataRole {
        IdRole = Qt::UserRole + 0x101,
    };
    explicit GlobalThemeModel(QObject *parent = nullptr);
    ~GlobalThemeModel() { }

    void setThemeModel(ThemeModel *model);
    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    void updateData();

private:
    ThemeModel *m_themeModel;
    QStringList m_keys;
};

class GlobalThemeDelegate : public Dtk::Widget::DStyledItemDelegate
{
public:
    explicit GlobalThemeDelegate(QAbstractItemView *parent = nullptr);

protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    virtual void drawDecoration(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
    void drawChecked(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
    void drawDisplay(const QStyle *style, QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect) const;
};

#endif // GLOBALTHEMELISTVIEW_H
