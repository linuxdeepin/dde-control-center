#include "navwidget.h"
#include "navitemwidget.h"

NavWidget::NavWidget(QWidget *parent)
    : QWidget(parent)
{
    m_gridLayout = new QGridLayout;
    m_moduleList << "account" << "display" << "defapp" << "personalization" << "network" << "sound"
                 << "datetime" << "power" << "mouse" << "keyboard" << "update" << "systeminfo";

    setLayout(m_gridLayout);

    refershGridLayout();
}

void NavWidget::refershGridLayout()
{
    // clear
    while (QLayoutItem *item = m_gridLayout->takeAt(0))
    {
        item->widget()->deleteLater();
        delete item;
    }

    for (int i = 0; i != m_moduleList.size(); ++i)
    {
        NavItemWidget *item = new NavItemWidget(m_moduleList[i]);

        connect(item, &NavItemWidget::itemClicked, this, &NavWidget::requestModule);

        m_gridLayout->addWidget(item, i / 3, i % 3);
    }
}
