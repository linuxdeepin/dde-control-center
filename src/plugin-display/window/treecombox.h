#pragma once

#include "interface/namespace.h"
#include <DComboBox>
#include <DListView>
#include <DStandardItem>
#include <src/plugin-display/operation/machine.h>

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

class Machine;
class DisplayModel;
class TreeCombox : public DTK_WIDGET_NAMESPACE::DComboBox
{
    Q_OBJECT
public:
    explicit TreeCombox(QStandardItemModel *model, QWidget *parent = nullptr);
    ~TreeCombox();
//    inline DTK_WIDGET_NAMESPACE::DListView* diviceItemsListView() { return m_deviceItemsListView;}
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

