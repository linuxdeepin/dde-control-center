#include "displaycontrolpage.h"
#include "displaycontrolmodel.h"
#include "../basiclistview.h"

#include <QVBoxLayout>

using namespace dcc::display;

DisplayControlPage::DisplayControlPage(DisplayModel *model, QWidget *parent)
    : QWidget(parent)
{

    DisplayControlModel *listModel = new DisplayControlModel(model);
    BasicListView *listView = new BasicListView;
    listView->setModel(listModel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(listView);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
}
