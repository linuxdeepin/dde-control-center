#include "updatectrlwidget.h"

UpdateCtrlWidget::UpdateCtrlWidget(QWidget *parent)
    : ContentWidget(parent)
{
    QLabel *testLbl = new QLabel;
    testLbl->setText("Update");

    setContent(testLbl);
    setTitle(tr("Update"));
}
