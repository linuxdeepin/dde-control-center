#include <QVBoxLayout>

#include "buttongroup.h"
#include "dheaderline.h"
#include "dseparatorhorizontal.h"

DUI_USE_NAMESPACE

ButtonGroup::ButtonGroup(const QString& title)
{
    setLayout(new QVBoxLayout);
    layout()->setSpacing(0);
    layout()->setMargin(0);

    DHeaderLine* header = new DHeaderLine();
    header->setTitle(title);
    layout()->addWidget(header);
}

ButtonGroup* ButtonGroup::addItem(QWidget* w)
{
    layout()->addWidget(w);
    return this;
}


ButtonGroup* ButtonGroup::addSeparator()
{
    layout()->addWidget(new DSeparatorHorizontal);
    return this;
}
