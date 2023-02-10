//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "widgets/settingsheaderitem.h"

using namespace DCC_NAMESPACE;

class Tst_SettingsHeaderItem : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new SettingsHeaderItem();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    SettingsHeaderItem *obj = nullptr;
};

TEST_F(Tst_SettingsHeaderItem, coverage)
{
    obj->setTitle("aaa");
    obj->textLabel();
    obj->layout();
    obj->setRightWidget(new QWidget);
}
