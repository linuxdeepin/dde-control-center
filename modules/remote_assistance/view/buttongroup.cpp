#include <QVBoxLayout>

#include "buttongroup.h"
#include "dheaderline.h"
#include "dseparatorhorizontal.h"

DUI_USE_NAMESPACE

ButtonGroup::ButtonGroup()
    : m_header(new DHeaderLine)
{
    setLayout(new QVBoxLayout);
    layout()->setSpacing(0);
    layout()->setMargin(0);
    layout()->addWidget(m_header);
    m_header->hide();
}

ButtonGroup::ButtonGroup(const QString& title): ButtonGroup()
{
    setGroupTitle(title);
}

ButtonGroup* ButtonGroup::setGroupTitle(const QString& title)
{
    m_header->setTitle(title);
    m_header->show();
    return this;
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
