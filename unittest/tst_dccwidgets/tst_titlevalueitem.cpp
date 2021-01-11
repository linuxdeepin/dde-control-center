#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/titlevalueitem.h"

using namespace dcc::widgets;

class Tst_TitleValueItem : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new TitleValueItem();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    TitleValueItem *obj = nullptr;
};

TEST_F(Tst_TitleValueItem, coverage)
{
    obj->setValue("aaa");
    obj->setTitle("ccc");
    obj->setWordWrap(0);
    obj->value();
}
