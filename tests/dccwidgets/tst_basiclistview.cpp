#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/basiclistview.h"

using namespace dcc::widgets;

class Tst_BasicListView : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new BasicListView;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    BasicListView *obj = nullptr;
};

TEST_F(Tst_BasicListView, coverage)
{
    obj->setModel(new QStandardItemModel());
    obj->onContentHeightChanged();
    obj->setAutoFitHeight(false);
    obj->sizeHint();
    obj->leaveEvent(nullptr);
}
