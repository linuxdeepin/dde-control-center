#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/titledslideritem.h"

using namespace dcc::widgets;

class Tst_TitledSliderItem : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new TitledSliderItem("aaa");
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    TitledSliderItem *obj = nullptr;
};

TEST_F(Tst_TitledSliderItem, coverage)
{
    obj->slider();
    obj->setAnnotations(QStringList() << "aaa" << "bbb");
    obj->valueLiteral();
    obj->setValueLiteral("ccc");
    obj->setLeftIcon(QIcon::fromTheme("ddd"));
    obj->setRightIcon(QIcon::fromTheme("eee"));
    obj->setIconSize(QSize(0, 0));
    obj->getbottomlayout();
}
