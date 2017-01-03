#include "bootwidget.h"

#include <QVBoxLayout>

#include "settingsgroup.h"
#include "grubbackgrounditem.h"
#include "translucentframe.h"
#include "systeminfomodel.h"
#include "labels/tipslabel.h"

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
    m_bootList->setDragDropMode(QListWidget::DragDrop);
    m_bootList->setDefaultDropAction(Qt::MoveAction);
    m_bootList->move(50, 50);
    m_bootList->setPalette(Qt::transparent);
    m_bootList->setWordWrap(true);

    m_boot = new SwitchWidget();
    m_boot->setTitle(tr("Startup Delay"));

    m_theme = new SwitchWidget();
    m_theme->setTitle(tr("Theme"));

    TipsLabel *label = new TipsLabel(tr("The boot image can be replaced by dragging one to it, "
                                        "and the boot order can be changed by dragging the highlight"));
    label->setWordWrap(true);
    label->setMargin(10);

    group->appendItem(background);
    group->appendItem(m_boot);
    group->appendItem(m_theme);

    layout->addWidget(group);
    layout->addWidget(label);
    layout->addStretch();

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
    m_boot->setChecked(model->bootDelay());
    m_theme->setChecked(model->themeEnabled());

    setEntryList(model->entryLists());
    setDefaultEntry(model->defaultEntry());
}

void BootWidget::setEntryList(const QStringList &list)
{
    for(int i = 0; i<list.count(); i++)
    {
        const QString entry = list.at(i);

        QListWidgetItem* item = new QListWidgetItem(entry);
        item->setBackground(Qt::transparent);
        item->setSizeHint(QSize(m_bootList->width(), 30));
        m_bootList->addItem(item);

        this->blockSignals(true);
        if (m_defaultEntry == entry) {
            m_bootList->setCurrentRow(i);
            onCurrentItem(item, nullptr);
        } else {
            onCurrentItem(nullptr, item);
        }
        this->blockSignals(false);
    }
}

void BootWidget::onItemActivated(QListWidgetItem *item)
{
    if (item) {
        emit defaultEntry(item->text());
    }
}

void BootWidget::onCurrentItem(QListWidgetItem *cur, QListWidgetItem *pre)
{
    if ( pre ) {
        QPixmap pix(16, 16);
        pix.fill(Qt::transparent);
        pre->setIcon(pix);
    }

    if ( cur ) {
        QPixmap pix(":/systeminfo/themes/common/icons/select.png");
        cur->setIcon(pix);

        emit defaultEntry(cur->text());
    }
}

}
}
