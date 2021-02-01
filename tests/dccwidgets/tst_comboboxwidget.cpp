#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/comboboxwidget.h"

using namespace dcc::widgets;

class Tst_ComboBoxWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ComboBoxWidget;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    ComboBoxWidget *obj = nullptr;
};

TEST_F(Tst_ComboBoxWidget, coverage)
{
    obj->appendOption("aaa", "bbb");
    obj->removeOption("ccc");
    obj->setCurrent("bbb");
    obj->setEditable(false);
}
