/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "interface/namespace.h"

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
    void requestCloseWidget();

public Q_SLOTS:
    void onItemClicked(const QModelIndex &index);
    void onHourTypeChanged(const bool &type);

private:
    dcc::widgets::MultiSelectListView *m_listview;
    ClockItem *m_clockItem;
    const dcc::datetime::DatetimeModel *m_model;
    dcc::widgets::SwitchWidget *m_hourTypeSwitch;
    QModelIndex m_lastIndex;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
