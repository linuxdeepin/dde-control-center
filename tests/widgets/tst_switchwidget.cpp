#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "widgets/switchwidget.h"

DCC_USE_NAMESPACE

class Tst_SwitchWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new SwitchWidget();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    SwitchWidget *obj = nullptr;
};

TEST_F(Tst_SwitchWidget, coverage)
{
    obj->setChecked(1);
    obj->setTitle("bbb");
    obj->title();
    obj->checked();
    obj->leftWidget();
    obj->getMainLayout();
    obj->switchButton();
}
