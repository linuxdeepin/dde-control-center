#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/dccsliderannotated.h"
#include "../../include/widgets/dccslider.h"

using namespace dcc::widgets;

class Tst_DCCSliderAnnotated : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new DCCSliderAnnotated;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    DCCSliderAnnotated *obj = nullptr;
};

TEST_F(Tst_DCCSliderAnnotated, coverage)
{
    obj->setAnnotations(QStringList() << "aaa" << "bbb");
    obj->slider();
    obj->setSlider(new DCCSlider());
    obj->leftIcon();
    obj->setLeftIcon("0, 100");
    obj->setLeftIcon(QIcon::fromTheme("aaa"), QSize(0, 0));
    obj->rightIcon();
    obj->setRightIcon("0, 100");
    obj->setRightIcon(QIcon::fromTheme("aaa"), QSize(0, 0));
}
