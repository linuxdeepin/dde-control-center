#include "shortcutwidget.h"
#include "shortcutitem.h"
#include <QVBoxLayout>
using namespace dcc;

ShortcutWidget::ShortcutWidget(QWidget *parent)
    :ContentWidget(parent)
{
    QWidget* widget = new QWidget();

    m_systemGroup = new SettingsGroup();
    m_windowGroup = new SettingsGroup();
    m_workspaceGroup = new SettingsGroup();
    m_custonGroup = new SettingsGroup();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_systemGroup);
    layout->addWidget(m_windowGroup);
    layout->addWidget(m_workspaceGroup);
    layout->addWidget(m_custonGroup);

    widget->setLayout(layout);
    setContent(widget);
}

void ShortcutWidget::addShortcut(QList<ShortcutInfo *> list, ShortcutWidget::InfoType type)
{
    for(int i = 0; i<list.count(); i++)
    {
        ShortcutItem* item = new ShortcutItem();
        item->setTitle(list.at(i)->name);
        item->setShortcutString(list.at(i)->accels);

        if(type == ShortcutWidget::System)
            m_systemGroup->appendItem(item);
        else if(type == ShortcutWidget::Window)
            m_windowGroup->appendItem(item);
        else if(type == ShortcutWidget::Workspace)
            m_workspaceGroup->appendItem(item);
        else if(type == ShortcutWidget::Custom)
            m_custonGroup->appendItem(item);
    }
}
