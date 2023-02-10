//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"

using namespace DCC_NAMESPACE;

class Tst_SettingsGroup : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new SettingsGroup();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    SettingsGroup *obj = nullptr;
};

TEST_F(Tst_SettingsGroup, coverage)
{
    obj->headerItem();
    obj->setHeaderVisible(1);
    obj->insertWidget(new QWidget);
    obj->insertItem(0, new SettingsItem);
    obj->appendItem(new SettingsItem);
    obj->appendItem(new SettingsItem, SettingsGroup::BackgroundStyle::ItemBackground);
    obj->removeItem(obj->getItem(0));
    obj->moveItem(obj->getItem(0), 1);
    obj->setSpacing(10);
    obj->itemCount();
    obj->clear();
    obj->getLayout();
}
