#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/lineeditwidget.h"

using namespace dcc::widgets;

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
