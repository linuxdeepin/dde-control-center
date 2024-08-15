//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <DListView>

#include <optional>

class CommonInfoListView : public DTK_WIDGET_NAMESPACE::DListView
{
    Q_OBJECT
public:
    explicit CommonInfoListView(QWidget *parent = nullptr);

    void setMaxShowHeight(int height);

protected slots:
    void updateGeometries() override;

private:
    std::optional<int> m_maxheight;
};
