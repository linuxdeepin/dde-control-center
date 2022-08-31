// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "window/insertplugin.h"
#include "window/utils.h"

#include <DListView>

#include <QWidget>

namespace dcc {
namespace power {
class PowerModel;
}
namespace widgets {
class MultiSelectListView;
}
} // namespace dcc

namespace DCC_NAMESPACE {
namespace power {

class PowerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PowerWidget(QWidget *parent = nullptr);
    virtual ~PowerWidget();

    void initialize(bool hasBattery);
    void setModel(const dcc::power::PowerModel *model);
    DTK_WIDGET_NAMESPACE::DListView *getListViewPointer();
    bool getIsUseBattety();
    void showDefaultWidget();

Q_SIGNALS:
    void requestPushWidget(int index);
    void requestShowGeneral() const;
    void requestShowUseElectric() const;
    void requestShowUseBattery() const;
    void requestUpdateSecondMenu(bool);

public Q_SLOTS:
    void onItemClicked(const QModelIndex &index);
    void removeBattery(bool state);

private:
    void initUi();
    void initMembers();
    void initConnections();
    bool configContent(const QString & configName);

private:
    dcc::widgets::MultiSelectListView *m_listView;
    QStandardItemModel *m_itemModel;
    const dcc::power::PowerModel *m_model;
    bool m_bhasBattery;
    QList<ListSubItem> m_menuIconText;
    QModelIndex m_lastIndex;
    int m_batteryIndex;
};

} // namespace power
} // namespace DCC_NAMESPACE
