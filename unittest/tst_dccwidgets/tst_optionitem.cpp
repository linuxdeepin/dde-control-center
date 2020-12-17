#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/optionitem.h"

using namespace dcc::widgets;

class Tst_OptionItem : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new OptionItem;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    OptionItem *obj = nullptr;
};

TEST_F(Tst_OptionItem, coverage)
{
    obj->title();
    obj->setTitle("aaa");
    obj->setTitleFixedHeight(10);
    obj->setTitleWidget(new QWidget());
    obj->setContentWidget(new QWidget());
    obj->setContentsMargins(0, 0, 0, 0);
    obj->selected();
    obj->setSelected(0);
}
