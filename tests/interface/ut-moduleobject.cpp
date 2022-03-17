#include "gtest/gtest.h"

#include "interface/moduleobject.h"

#include <QDebug>

DCC_USE_NAMESPACE

class Tst_ModuleObject : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ModuleObject;
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
    ModuleData *dataRoot = new ModuleData("Name", "DisplayName", "Description", QIcon::fromTheme("preferences-system"), obj);
    obj->setModuleData(dataRoot);
    obj->setChildType(ModuleObject::ChildType::HList);

    for (int i = 1; i < 4; i++) {
        ModuleObject *module = new ModuleObject(QString("menu%1").arg(i), QString("菜单%1").arg(i), QString("我是菜单%1").arg(i));
        module->setChildType(ModuleObject::ChildType::Page);

        obj->appendChild(module);
    }

    ModuleObject *module = new ModuleObject;
    ModuleData *data = new ModuleData;
    data->Name = QString("action1");
    data->DisplayName = QString("选项1");
    data->Description = QString("我是选项1");
    module->setModuleData(data);

    obj->childrens().last()->insertChild(1, module);

    ASSERT_TRUE(obj->findChild(module) == 2);

    obj->removeChild(0);

    ASSERT_TRUE(obj->childrens().size() == 2);

}