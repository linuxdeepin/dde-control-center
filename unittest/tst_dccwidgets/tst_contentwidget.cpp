#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/contentwidget.h"

using namespace dcc;

class Tst_ContentWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ContentWidget;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    ContentWidget *obj = nullptr;
};

TEST_F(Tst_ContentWidget, coverage)
{
    obj->setTitle("aaa");
    obj->sendBackSignal();
    obj->content();
    obj->setContent(new QWidget());
    obj->scrollTo(5);
}
