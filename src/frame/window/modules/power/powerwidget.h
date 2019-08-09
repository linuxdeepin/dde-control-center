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

#include <QWidget>
#include <QVBoxLayout>
#include <QListView>
#include <QStandardItemModel>

namespace dcc {
namespace power {
class PowerModel;
}
}

namespace DCC_NAMESPACE {
namespace power {

const unsigned int power_default_index = 0;

class PowerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PowerWidget(QWidget *parent = nullptr);
    virtual ~PowerWidget();

    void initialize();
    void setModel(const dcc::power::PowerModel *model);
    void requestDefaultWidget();

private:
    QVBoxLayout *layout;
    QListView *m_listview;
    const dcc::power::PowerModel *m_model;
    QModelIndex m_defaultIndex;
    QStandardItemModel *m_listmodel;

Q_SIGNALS:
    void requestPushWidget(int index);
    void requestRemoveBattery(bool state);

public Q_SLOTS:
    void onItemClieck(const QModelIndex &index);
};

}// namespace datetime
}// namespace DCC_NAMESPACE
