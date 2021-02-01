#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/loadingnextpagewidget.h"

using namespace dcc::widgets;

class Tst_LoadingNextPageWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new LoadingNextPageWidget;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    LoadingNextPageWidget *obj = nullptr;
};

TEST_F(Tst_LoadingNextPageWidget, coverage)
{
    obj->setLoading(1);
}
