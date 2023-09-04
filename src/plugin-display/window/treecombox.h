//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <DComboBox>
#include <DListView>
#include <DStandardItem>

QT_BEGIN_NAMESPACE
class QStandardItem;
class QComboBox;
class QPushButton;
QT_END_NAMESPACE

enum DevViewItemType {
    DeviceViewItem = 0,
    MoreSettingsItem
};

Q_DECLARE_METATYPE(DevViewItemType)

class TreeComboxDelegate : public DTK_WIDGET_NAMESPACE::DStyledItemDelegate
{
public:
    explicit TreeComboxDelegate(QAbstractItemView *parent = Q_NULLPTR);
    ~TreeComboxDelegate() ;
protected:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    QAbstractItemView *m_parentWidget;
};

class Machine;
class DisplayModel;
class TreeCombox : public DTK_WIDGET_NAMESPACE::DComboBox
{
    Q_OBJECT
public:
    explicit TreeCombox(QStandardItemModel *model, QWidget *parent = nullptr);
    ~TreeCombox();
    void addDivicesTitel(const QString& devTitel);
    void addDevicesSettingsItem();

    void addDeviceCheckedIcon(DTK_WIDGET_NAMESPACE::DStandardItem *pi, bool initChecked);
    void updateItemCheckStatus(const QString &name, bool visible);

private:
    void initUI();
    void initConnect();

signals:
    void viewItemPressed(const QModelIndex &index);

private:
    DTK_WIDGET_NAMESPACE::DListView *m_deviceItemsListView;
    QStandardItemModel *m_itemsmodel;

    QList<QStandardItem *> m_historyDeviceItems;
    QList<QStandardItem *> m_MoreDeviceItems;
};

