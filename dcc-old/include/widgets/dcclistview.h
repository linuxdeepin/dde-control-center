//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCLISTVIEW_H
#define DCCLISTVIEW_H
#include "interface/namespace.h"

#include <DListView>

namespace DCC_NAMESPACE {
class DCCListView : public DTK_WIDGET_NAMESPACE::DListView
{
    Q_OBJECT
public:
    explicit DCCListView(QWidget *parent = nullptr);

protected slots:
    void updateGeometries() override;
};
}
#endif // DCCLISTVIEW_H
