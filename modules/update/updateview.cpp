#include "updateview.h"

#include <QDebug>

using namespace dcc;

UpdateView::UpdateView()
    : ModuleWidget()
{
    m_updateItem = new NextPageWidget;
    m_updateItem->setTitle(tr("Update"));
    m_updateGroup = new SettingsGroup;
    m_updateGroup->appendItem(m_updateItem);

    m_settingsItem = new NextPageWidget;
    m_settingsItem->setTitle(tr("Update Settings"));
    m_settingsGroup = new SettingsGroup;
    m_settingsGroup->appendItem(m_settingsItem);

    m_centeralLayout->addWidget(m_updateGroup);
    m_centeralLayout->addWidget(m_settingsGroup);

    setTitle(tr("Update"));

    for (int i(0); i != 10; ++i)
    {
        NextPageWidget *w = new NextPageWidget;
        w->setTitle(QString::number(i));
        m_settingsGroup->appendItem(w);

        connect(m_updateItem, &NextPageWidget::clicked, [=] {
            m_settingsGroup->removeItem(w);
            w->deleteLater();

            qDebug() << "item deleted";
        });
    }

    connect(m_updateItem, &NextPageWidget::clicked, this, &UpdateView::showUpdateWidget);
    connect(m_settingsItem, &NextPageWidget::clicked, this, &UpdateView::showSettingsWidget);
}
