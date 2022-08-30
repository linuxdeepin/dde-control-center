// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/dccslider.h"

using namespace dcc::widgets;

class Tst_DCCSlider : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new DCCSlider;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    DCCSlider *obj = nullptr;
};

TEST_F(Tst_DCCSlider, coverage)
{
    obj->slider();
    obj->qtSlider();
    obj->setType(DCCSlider::SliderType::Normal);
    obj->setRange(0, 100);
    obj->setTickPosition(QSlider::NoTicks);
    obj->setTickInterval(10);
    obj->setSliderPosition(10);
    obj->setAnnotations(QStringList() << "aaa" << "bbb");
    obj->setOrientation(Qt::Orientation::Vertical);
    obj->setSeparateValue(10);
}
