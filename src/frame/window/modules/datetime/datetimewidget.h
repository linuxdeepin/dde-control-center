// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "window/utils.h"
#include "window/insertplugin.h"

#include <types/zoneinfo.h>
#include <DListView>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {

class SwitchWidget;
class SettingsItem;
class MultiSelectListView;

namespace labels {
class NormalLabel;
} //labels
} //widgets

namespace datetime {
class DatetimeModel;
} //datetime
} //dcc

namespace DCC_NAMESPACE {
namespace datetime {

class ClockItem;

class DatetimeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DatetimeWidget(QWidget *parent = nullptr);

    virtual ~DatetimeWidget();
    void init();
    void setModel(const dcc::datetime::DatetimeModel *model);
    DTK_WIDGET_NAMESPACE::DListView *getListViewPointer();
    void setCurrentTimeZone(const ZoneInfo &info);
    void setTimeType(const QString &country);
    void setDefaultWidget();

Q_SIGNALS:
    void requestSetHourType(const bool &type);
    void requestPushWidget(const int &page);
    void requestTimezoneList();
    void requestFormatSetting();
    void requestTimeSetting();
    void requestCloseWidget();
    void requestUpdateSecondMenu(bool);

public Q_SLOTS:
    void onItemClicked(const QModelIndex &index);
    void onHourTypeChanged(const bool &type);
    bool configContent(const QString & configName);

private:
    dcc::widgets::MultiSelectListView *m_listview;
    ClockItem *m_clockItem;

    QList<ListSubItem> m_itemList;
    const dcc::datetime::DatetimeModel *m_model;
    dcc::widgets::SwitchWidget *m_hourTypeSwitch;
    QModelIndex m_lastIndex;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
