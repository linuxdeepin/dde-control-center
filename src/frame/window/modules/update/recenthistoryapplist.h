// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once
#include "interface/namespace.h"
#include "widgets/contentwidget.h"

namespace DCC_NAMESPACE {
namespace update {

class RecentHistoryApplist : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit RecentHistoryApplist(QWidget *parent = 0);

    void setContentWidget(QWidget *widget);
};

} // namespace update
} // namespace DCC_NAMESPACE
