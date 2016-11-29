#include "bootwidget.h"
#include "settingsgroup.h"
#include "grubbackgrounditem.h"

#include <QVBoxLayout>

BootWidget::BootWidget(QWidget *parent)
    :ContentWidget(parent)
{
    QWidget* widget = new QWidget();

    QVBoxLayout* layout = new QVBoxLayout();
    SettingsGroup *group = new SettingsGroup();

    GrubBackgroundItem* background = new GrubBackgroundItem();

    m_bootList = new QListWidget(background);
    m_bootList->setDragDropMode(QListWidget::DragDrop);
//    m_bootList->setDefaultDropAction(Qt::MoveAction);
    m_bootList->move(50,50);

    m_boot = new SwitchWidget();
    m_boot->setTitle(tr("Startup Delay"));

    m_theme = new SwitchWidget();
    m_theme->setTitle(tr("Theme"));

    group->appendItem(background);
    group->appendItem(m_boot);
    group->appendItem(m_theme);

    layout->addWidget(group);

    QLabel* label = new QLabel(tr("The boot image can be replaced by dragging one to it,\n"
                                  "and the boot order can be changed by dragging \n the highlight"));
    layout->addWidget(label);

    widget->setLayout(layout);

    setContent(widget);

    connect(m_theme, SIGNAL(checkedChanegd(bool)), this, SIGNAL(enableTheme(bool)));
    connect(m_boot, SIGNAL(checkedChanegd(bool)), this, SIGNAL(bootdelay(bool)));
    connect(m_bootList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(onItemActivated(QListWidgetItem*)));
}

void BootWidget::setDefaultEntry(const QString &value)
{
    m_defaultEntry = value;
}

void BootWidget::setEntryList(const QStringList &list)
{
    m_bootList->addItems(list);
    for(int i = 0; i<list.count(); i++)
    {
        if(m_defaultEntry == list.at(i))
        {
            m_bootList->setCurrentRow(i);
            break;
        }
    }
}

void BootWidget::onItemActivated(QListWidgetItem *item)
{
    if(item)
    {
        emit defaultEntry(item->text());
    }
}

void BootWidget::setChecked(bool value)
{
    m_theme->setChecked(value);
}
