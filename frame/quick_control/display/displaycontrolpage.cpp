#include "displaycontrolpage.h"
#include "displaycontrolmodel.h"
#include "../basiclistview.h"

#include <QVBoxLayout>

using namespace dcc::display;

DisplayControlPage::DisplayControlPage(QWidget *parent)
    : QWidget(parent),

      m_displayModel(new DisplayModel(this)),
      m_displayWorker(new DisplayWorker(m_displayModel, this))
{

    DisplayControlModel *listModel = new DisplayControlModel;
    BasicListView *listView = new BasicListView;
    listView->setModel(listModel);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(listView);
    mainLayout->setMargin(0);

    setLayout(mainLayout);
}
