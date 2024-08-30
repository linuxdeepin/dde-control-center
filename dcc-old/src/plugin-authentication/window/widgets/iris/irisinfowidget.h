//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "charamangermodel.h"

#include <QWidget>

class CharaMangerModel;

class IrisInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit IrisInfoWidget(QWidget *parent = nullptr);
    ~IrisInfoWidget();

public Q_SLOTS:
    void updateState(CharaMangerModel::AddInfoState state);

private:
    void initWidget();

protected:
    void paintEvent(QPaintEvent *event);

private:
    CharaMangerModel::AddInfoState m_state;
    QTimer *m_timer;
    int m_angle;
};
