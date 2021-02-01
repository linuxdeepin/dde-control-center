#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/themeitem.h"

using namespace dcc::widgets;

class Tst_ThemeItem : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ThemeItem();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    ThemeItem *obj = nullptr;
};

TEST_F(Tst_ThemeItem, coverage)
{
    obj->setSelected(1);
    obj->setTitle("bbb");
    obj->setPic("aaa");
    obj->setId(1);
    obj->id();
}
