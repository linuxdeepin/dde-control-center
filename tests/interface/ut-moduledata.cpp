#include "gtest/gtest.h"

#include "interface/moduledata.h"

DCC_USE_NAMESPACE

class Tst_ModuleData : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ModuleData;
    }
    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }
public:
    ModuleData *obj = nullptr;
};

TEST_F(Tst_ModuleData, coverage)
{
    ModuleData data("name", "display");
    data.Description = "description";

    obj->Name = "name";
    obj->DisplayName = "display";
    obj->Description = "description";

    qDebug() << data;

    ASSERT_TRUE(data == *obj);
}