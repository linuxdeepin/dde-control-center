/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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
#include "modules/authentication/charamangermodel.h"

#include <QWidget>

namespace dcc {
namespace authentication {

class CharaMangerModel;

class IrisInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IrisInfoWidget(QWidget *parent = nullptr);
    ~IrisInfoWidget();

public Q_SLOTS:
    void updateState(dcc::authentication::CharaMangerModel::AddInfoState state);

private:
    void initWidget();

protected:
    void paintEvent(QPaintEvent *event);

private:
    dcc::authentication::CharaMangerModel::AddInfoState m_state;
    QTimer *m_timer;
    int m_angle;
};

}
}
