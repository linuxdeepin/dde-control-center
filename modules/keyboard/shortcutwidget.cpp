#include "shortcutwidget.h"
#include "shortcutitem.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>

using namespace dcc;

ShortcutWidget::ShortcutWidget(QWidget *parent)
    :ContentWidget(parent)
{
    QWidget* widget = new QWidget();

    m_systemGroup = new SettingsGroup();
    m_windowGroup = new SettingsGroup();
    m_workspaceGroup = new SettingsGroup();
    m_custonGroup = new SettingsGroup();
    m_search = new SearchInput();

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(10);
    layout->addWidget(m_search);
    layout->addWidget(m_systemGroup);
    layout->addWidget(m_windowGroup);
    layout->addWidget(m_workspaceGroup);
    layout->addWidget(m_custonGroup);

    QPushButton* btn = new QPushButton(tr("Add Custon Shortcut"));
    layout->addWidget(btn);
    widget->setLayout(layout);
    setContent(widget);

    connect(btn, SIGNAL(clicked()), this, SIGNAL(customShortcut()));
}

void ShortcutWidget::addShortcut(QList<ShortcutInfo *> list, ShortcutWidget::InfoType type)
{
    QList<ShortcutInfo*>::iterator it = list.begin();
    for(; it != list.end(); ++it)
    {
        ShortcutItem* item = new ShortcutItem();
        connect(item, SIGNAL(shortcutChangd(bool, ShortcutInfo*, QString)), this, SIGNAL(shortcutChanged(bool, ShortcutInfo*, QString)));
        item->setShortcutInfo((*it));

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

void ShortcutWidget::onSearch(const QString &text)
{
    Q_UNUSED(text);
}
