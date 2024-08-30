//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SAFEUPDATEITEM_H
#define SAFEUPDATEITEM_H

#include "updatesettingitem.h"

class SafeUpdateItem: public UpdateSettingItem
{
    Q_OBJECT
public:
    explicit SafeUpdateItem(QWidget *parent = nullptr);

    void init();

    void setData(UpdateItemInfo *updateItemInfo) override;
};

#endif // SAFEUPDATEITEM_H
