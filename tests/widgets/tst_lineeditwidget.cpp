//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "widgets/lineeditwidget.h"

using namespace DCC_NAMESPACE;

class Tst_LineEditWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new LineEditWidget;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    LineEditWidget *obj = nullptr;
};

TEST_F(Tst_LineEditWidget, coverage)
{
    obj->textEdit();
    obj->dTextEdit();
    obj->text();
    obj->setTitleVisible(false);
    obj->addRightWidget(new QWidget());
    obj->setReadOnly(true);
    obj->setIsErr(false);
    obj->isShowAlert();
    obj->showAlertMessage("aaa");
    obj->hideAlertMessage();
    obj->label();
    obj->setTitle("aaa");
    obj->setText("bbb");
    obj->setPlaceholderText("ccc");
}
