//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "gtest/gtest.h"

#include "interface/pagemodule.h"
#include "interface/hlistmodule.h"

#include <QDebug>

using namespace DCC_NAMESPACE;

class Tst_ModuleObject : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new HListModule;
    }
    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }
public:
    ModuleObject *obj = nullptr;
};


TEST_F(Tst_ModuleObject, coverage)
{
    obj->setName("Name");
    obj->setDisplayName("DisplayName");
    obj->setDescription("Description");
    obj->setIcon(QIcon::fromTheme("preferences-system"));

    for (int i = 1; i < 4; i++) {
        ModuleObject *module = new PageModule(QString("menu%1").arg(i), QString("菜单%1").arg(i), QString("我是菜单%1").arg(i));

        obj->appendChild(module);
    }

    ModuleObject *module = new ModuleObject;
    module->setName("action1");
    module->setDisplayName("选项1");
    module->setDescription("我是选项1");

    obj->childrens().last()->insertChild(1, module);

    ASSERT_TRUE(obj->findChild(module) == 2);

    obj->removeChild(0);

    ASSERT_TRUE(obj->childrens().size() == 2);

}
