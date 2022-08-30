// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
