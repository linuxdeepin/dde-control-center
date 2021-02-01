#include <gtest/gtest.h>

#include "../../include/widgets/basiclistmodel.h"

using namespace dcc::widgets;

class Tst_BasicListModel : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new BasicListModel;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    BasicListModel *obj = nullptr;
};

TEST_F(Tst_BasicListModel, coverage)
{
    obj->appendOption("aaa", "bbb");
    obj->rowCount(obj->index(0));
    obj->data(obj->index(0), BasicListModel::ItemSizeRole);
    obj->setSelectedIndex(obj->index(0));
    obj->setHoveredIndex(obj->index(0));
    obj->clear();
}
