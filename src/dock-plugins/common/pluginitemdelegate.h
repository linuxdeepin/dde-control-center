// SPDX-FileCopyrightText: 2016 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "commoniconbutton.h"
#include "commontextbutton.h"

#include <DLabel>
#include <DSpinner>

#include <QObject>
#include <QStyledItemDelegate>
#include <QStandardItem>
#include <QBoxLayout>
#include <QSpacerItem>

DWIDGET_USE_NAMESPACE

enum PluginItemState
{
    NoState,            // 不显示状态提示
    Connecting,         // 显示spinner旋片，提示用户正在连接中
    Connected,          // 显示“对勾”状态，提示用户结果，可以hover和click
    ConnectedOnlyPrompt // 显示“对勾”状态，仅提示用户结果
};

struct ItemSpacing
{
    int top;
    int height;
    int bottom;
    QStyleOptionViewItem::ViewItemPosition viewItemPosition;
};

class PluginStandardItem;
class PluginItemWidget;
class PluginItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit PluginItemDelegate(QAbstractItemView* parent = nullptr);

    inline int itemHeight() const { return m_widgetHeight; }
    inline int itemSpacing() const { return m_bottomSpacing; }

    inline void setItemHeight(int height) { m_widgetHeight = height; }
    inline void setItemSpacing(int spacing) { m_bottomSpacing = spacing; }

    inline void setBeginningItemStyle(QStyleOptionViewItem::ViewItemPosition viewItemPosition) { m_beginningItemStyle = viewItemPosition; }
    inline void setMiddleItemStyle(QStyleOptionViewItem::ViewItemPosition viewItemPosition) { m_middleItemStyle = viewItemPosition; }
    inline void setEndItemStyle(QStyleOptionViewItem::ViewItemPosition viewItemPosition) { m_endItemStyle = viewItemPosition; }

    ItemSpacing getItemSpacing(const QModelIndex &index) const;

    // painting
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // editing
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void destroyEditor(QWidget *editor, const QModelIndex &index) const override;

private:
    const QAbstractItemView *m_view;
    int m_widgetHeight;
    int m_bottomSpacing;
    QStyleOptionViewItem::ViewItemPosition m_beginningItemStyle;
    QStyleOptionViewItem::ViewItemPosition m_middleItemStyle;
    QStyleOptionViewItem::ViewItemPosition m_endItemStyle;
};

class PluginStandardItem : public QObject, public QStandardItem
{
    Q_OBJECT

public:
    explicit PluginStandardItem(const QIcon &icon, const QString &name, const PluginItemState state = PluginItemState::NoState);
    explicit PluginStandardItem();
    ~PluginStandardItem() override;

    void updateIcon(const QIcon &icon);
    QIcon icon() const { return m_icon; }

    void updateName(const QString &name);
    QString name() const { return m_name; }

    void updateState(const PluginItemState state);
    void updateBattery(const int battery);
    PluginItemState state() const { return m_state; }
    int battery() const { return m_battery; }

Q_SIGNALS:
    void iconChanged(const QIcon &icon);
    void nameChanged(const QString &name);
    void stateChanged(const PluginItemState state);
    void batteryChanged(const int battery);
    void connectBtnClicked();

private:
    QIcon m_icon;
    QString m_name;
    PluginItemState m_state;
    int m_battery = -1;
};

class PluginItemWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PluginItemWidget(PluginStandardItem *item, QWidget *parent = nullptr);
    ~PluginItemWidget() override;

public Q_SLOTS:
    void updateIcon(const QIcon &icon);
    void updateName(const QString &name);
    void updateState(const PluginItemState state);
    void updateBatteryDisplay(int battery);

protected:
    bool event(QEvent *e) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    PluginStandardItem *m_item;

    QHBoxLayout *m_mainLayout;
    CommonIconButton *m_iconBtn;
    CommonTextButton *m_disConnectBtn;
    DLabel *m_nameLabel;
    CommonIconButton *m_connBtn;
    DSpinner *m_spinner;
    QSpacerItem *m_rightIconSpacerItem;
    PluginItemState m_state;
    DLabel *m_batteryIcon;
    DLabel *m_batteryPercent;
};
