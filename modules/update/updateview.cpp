#include "updateview.h"

#include <QDebug>

using namespace dcc::widgets;

namespace dcc{
namespace update{

UpdateView::UpdateView()
    : ModuleWidget()
{
    setObjectName("Update");
    m_updateItem = new NextPageWidget;
    m_updateItem->setTitle(tr("Update"));
    m_updateGroup = new SettingsGroup;
    m_updateGroup->appendItem(m_updateItem);

    m_settingsItem = new NextPageWidget;
    m_settingsItem->setTitle(tr("Update Settings"));
    m_settingsGroup = new SettingsGroup;
    m_settingsGroup->appendItem(m_settingsItem);

    m_centralLayout->addWidget(m_updateGroup);
    m_centralLayout->addWidget(m_settingsGroup);

    setTitle(tr("Update"));

    connect(m_updateItem, &NextPageWidget::clicked, this, &UpdateView::pushUpdate);
    connect(m_settingsItem, &NextPageWidget::clicked, this, &UpdateView::pushMirrors);
}

}
}
