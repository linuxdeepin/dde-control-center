// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "safeupdateitem.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::update;

SafeUpdateItem::SafeUpdateItem(QWidget *parent)
    : UpdateSettingItem(parent)
{
    init();
}

void SafeUpdateItem::init()
{
    setIcon(":/update/updatev20/dcc_safe_update.svg");
    setClassifyUpdateType(SecurityUpdate);
    m_controlWidget->setDetailEnable(false);
    m_controlWidget->setShowMoreButtonVisible(false);
    m_controlWidget->setVersionVisible(false);
}

void SafeUpdateItem::setData(UpdateItemInfo *updateItemInfo)
{
    UpdateSettingItem::setData(updateItemInfo);

    m_controlWidget->setDatetimeVisible(false);
}


