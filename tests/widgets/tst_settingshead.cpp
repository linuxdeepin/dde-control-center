//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "widgets/settingshead.h"

using namespace DCC_NAMESPACE;

class Tst_SettingsHead : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new SettingsHead();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    SettingsHead *obj = nullptr;
};

TEST_F(Tst_SettingsHead, coverage)
{
    obj->setTitle("aaa");
    obj->setEditEnable(1);
    obj->toEdit();
    obj->toCancel();
}
