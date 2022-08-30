// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "recenthistoryapplist.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::update;

RecentHistoryApplist::RecentHistoryApplist(QWidget *parent)
    : ContentWidget(parent)
{
    setAccessibleName("RecentHistoryApplist");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void RecentHistoryApplist::setContentWidget(QWidget *widget)
{
    setContent(widget);
}
