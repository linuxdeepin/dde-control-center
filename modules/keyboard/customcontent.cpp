#include "customcontent.h"
#include "keyboardcontrol.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

CustomContent::CustomContent(QWidget *parent)
    :ContentWidget(parent)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);

    m_commandGroup = new SettingsGroup();
    m_name = new InputItem();
    m_name->setTitle(tr("Name"));

    m_command = new InputItem();
    m_command->setTitle(tr("Command"));

    m_command->setChooseVisible(true);

    TitleValueItem* item = new TitleValueItem();
    item->setTitle(tr("Shortcut"));
    item->setValue(tr("Please Grab Shortcut Again"));

    m_commandGroup->appendItem(m_name);
    m_commandGroup->appendItem(m_command);
    m_commandGroup->appendItem(item);

    layout->addWidget(m_commandGroup);
    KeyboardControl* control = new KeyboardControl();
    layout->addWidget(control);

    m_cancel = new QPushButton(tr("Cancel"));
    m_ok = new QPushButton(tr("Add"));

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(m_cancel);
    hlayout->addWidget(m_ok);

    layout->addLayout(hlayout);
    widget->setLayout(layout);
    setContent(widget);
}
