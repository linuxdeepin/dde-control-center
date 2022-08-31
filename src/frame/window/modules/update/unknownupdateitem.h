// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef UNKNOWNUPDATEITEM_H
#define UNKNOWNUPDATEITEM_H

#include "updatesettingitem.h"

namespace dcc {
namespace update {


class UnknownUpdateItem: public UpdateSettingItem
{
    Q_OBJECT
public:
    explicit UnknownUpdateItem(QWidget *parent = nullptr);
    void init();
    void setData(UpdateItemInfo *updateItemInfo) override;

};
}
}

#endif // UNKNOWNUPDATEITEM_H
