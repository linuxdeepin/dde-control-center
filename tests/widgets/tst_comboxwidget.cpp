#include <gtest/gtest.h>

#include "widgets/comboxwidget.h"

#include <QStandardItemModel>

using namespace DCC_NAMESPACE;

class Tst_ComboxWidget : public testing::Test
{
public:
    void SetUp() override { obj = new ComboxWidget; }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    ComboxWidget *obj = nullptr;
};

TEST_F(Tst_ComboxWidget, coverage)
{
    obj->setComboxOption(QStringList() << "aaa"
                                       << "bbb"
                                       << "ccc");
    obj->setCurrentText("ccc");
    obj->setTitle("ddd");
    obj->comboBox();
}
