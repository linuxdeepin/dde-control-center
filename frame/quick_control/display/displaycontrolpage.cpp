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

    connect(listView, &BasicListView::clicked, this, &DisplayControlPage::onItemClicked);
}

void DisplayControlPage::onItemClicked(const QModelIndex &index) const
{
    const DisplayControlModel::ItemType type = index.data(DisplayControlModel::ItemTypeRole).value<DisplayControlModel::ItemType>();

    switch (type)
    {
    case DisplayControlModel::Duplicate:    emit requestDuplicateMode();    return;
    case DisplayControlModel::Extend:       emit requestExtendMode();       return;
    case DisplayControlModel::Custom:       emit requestCustom();           return;
    default:;
    }

    Q_ASSERT(type == DisplayControlModel::Specificed);

    emit requestOnlyMonitor(index.data(DisplayControlModel::ItemNameRole).toString());
}
