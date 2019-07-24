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

#include "window/namespace.h"
#include "widgets/switchwidget.h"

#include <QObject>
#include <QVBoxLayout>
#include <QListView>

namespace DCC_NAMESPACE{
namespace datetime {

class DatetimeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DatetimeWidget(QWidget *parent = nullptr);

    virtual ~DatetimeWidget();
    void init();

private:
    QVBoxLayout *layout;
    dcc::widgets::SwitchWidget *m_ntpSwitch;
    QListView *m_listview;

Q_SIGNALS:
    void requestSetNtp(const bool &ntp);

public Q_SLOTS:
    void onItemClieck(const QModelIndex& index);
};

}// namespace datetime
}// namespace DCC_NAMESPACE

