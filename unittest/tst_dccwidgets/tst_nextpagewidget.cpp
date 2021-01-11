#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/nextpagewidget.h"

using namespace dcc::widgets;

class Tst_NextPageWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new NextPageWidget;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    NextPageWidget *obj = nullptr;
};

TEST_F(Tst_NextPageWidget, coverage)
{
    obj->title();
    obj->setTitle("aaa");
    obj->setValue("bbb");
    obj->setIcon(QPixmap());
    obj->clearValue();
    obj->value();
    obj->setRightTxtWordWrap(false);
}
