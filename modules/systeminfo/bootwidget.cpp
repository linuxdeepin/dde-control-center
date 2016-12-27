#include "bootwidget.h"
#include "settingsgroup.h"
#include "grubbackgrounditem.h"
#include "translucentframe.h"
#include "systeminfomodel.h"

#include <QVBoxLayout>

namespace dcc{
namespace systeminfo{

BootWidget::BootWidget(QWidget *parent)
    :ContentWidget(parent)
{
    TranslucentFrame* widget = new TranslucentFrame();

    QVBoxLayout* layout = new QVBoxLayout();
    SettingsGroup *group = new SettingsGroup();

    GrubBackgroundItem* background = new GrubBackgroundItem();

    m_bootList = new QListWidget(background);
    m_bootList->setStyleSheet("background-color: transparent");
    m_bootList->setDragDropMode(QListWidget::DragDrop);
    m_bootList->setDefaultDropAction(Qt::MoveAction);
    m_bootList->move(50,50);
    m_bootList->setPalette(Qt::transparent);

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
    connect(m_bootList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this, SLOT(onCurrentItem(QListWidgetItem*,QListWidgetItem*)));
}

void BootWidget::setDefaultEntry(const QString &value)
{
    m_defaultEntry = value;
}

void BootWidget::setModel(SystemInfoModel *model)
{
    m_boot->setChecked(model->bootTimeout());
    m_theme->setChecked(model->themeEnabled());
}

void BootWidget::setEntryList(const QStringList &list)
{
    for(int i = 0; i<list.count(); i++)
    {
        QPixmap pix(32,32);
        if(i == 0)
            pix.fill(Qt::red);
        else
            pix.fill(Qt::transparent);

        QListWidgetItem* item = new QListWidgetItem(QIcon(pix), list.at(i));
        item->setBackground(Qt::transparent);
        m_bootList->addItem(item);

        if(m_defaultEntry == list.at(i))
        {
            m_bootList->setCurrentRow(i);
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

void BootWidget::onCurrentItem(QListWidgetItem *cur, QListWidgetItem *pre)
{
    if(pre)
    {
        QPixmap pix(16, 16);
        pix.fill(Qt::transparent);
        pre->setIcon(pix);
    }

    if(cur)
    {
        QPixmap pix(":/gpl/themes/common/icons/select.png");
        cur->setIcon(pix);
    }
}
}
}
