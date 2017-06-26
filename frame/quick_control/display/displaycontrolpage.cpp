#include "displaycontrolpage.h"
#include "displaycontrolmodel.h"
#include "displayitemdelegate.h"
#include "basiclistview.h"

#include <QVBoxLayout>
#include <QEvent>

using dcc::widgets::BasicListView;
using namespace dcc::display;

DisplayControlPage::DisplayControlPage(DisplayModel *model, QWidget *parent)
    : QWidget(parent)
{

    DisplayControlModel *listModel = new DisplayControlModel(model);
    DisplayItemDelegate *delegate = new DisplayItemDelegate;
    BasicListView *listView = new BasicListView;
    listView->setModel(listModel);
    listView->setItemDelegate(delegate);
    listView->installEventFilter(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch();
    mainLayout->addWidget(listView);
    mainLayout->setMargin(0);

    setLayout(mainLayout);

    connect(listView, &BasicListView::clicked, this, &DisplayControlPage::onItemClicked);
}

bool DisplayControlPage::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);

    if (event->type() == QEvent::Leave)
        emit mouseLeaveView();

    return false;
}

void DisplayControlPage::onItemClicked(const QModelIndex &index) const
{
    const DisplayControlModel::ItemType type = index.data(DisplayControlModel::ItemTypeRole).value<DisplayControlModel::ItemType>();

    switch (type)
    {
    case DisplayControlModel::Duplicate:
        emit requestDuplicateMode();
        return;
    case DisplayControlModel::Extend:
        emit requestExtendMode();
        return;
    case DisplayControlModel::Custom:
        emit requestConfig(index.data(DisplayControlModel::ItemConfigNameRole).toString());
        return;
    case DisplayControlModel::NewConfig:
        emit requestCustom();
        return;
    default:;
    }

    Q_ASSERT(type == DisplayControlModel::Specificed);

    emit requestOnlyMonitor(index.data(DisplayControlModel::ItemNameRole).toString());
}
