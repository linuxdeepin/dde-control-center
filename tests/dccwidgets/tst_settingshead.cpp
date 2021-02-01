#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/settingshead.h"

using namespace dcc::widgets;

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
