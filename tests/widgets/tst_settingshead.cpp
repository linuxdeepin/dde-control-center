#include <gtest/gtest.h>

#include "widgets/settingshead.h"

#include <QStandardItemModel>

using namespace DCC_NAMESPACE;

class Tst_SettingsHead : public testing::Test
{
public:
    void SetUp() override { obj = new SettingsHead(); }

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
