#include "bootwidget.h"

#include <QVBoxLayout>

#include "settingsgroup.h"
#include "grubbackgrounditem.h"
#include "translucentframe.h"
#include "systeminfomodel.h"

namespace dcc{
namespace systeminfo{

BootWidget::BootWidget(QWidget *parent)
    : ContentWidget(parent)
{
    TranslucentFrame* widget = new TranslucentFrame;

    QVBoxLayout* layout = new QVBoxLayout;
    SettingsGroup *group = new SettingsGroup;

    m_background = new GrubBackgroundItem;

    QVBoxLayout *listLayout = new QVBoxLayout;
    listLayout->setSpacing(0);
    listLayout->setMargin(0);

    m_bootList = new QListWidget;
    m_bootList->setDragDropMode(QListWidget::DragDrop);
    m_bootList->setDefaultDropAction(Qt::MoveAction);
    m_bootList->setPalette(Qt::transparent);
    m_bootList->setWordWrap(true);
    m_bootList->setDragEnabled(false);

    m_updatingLabel = new TipsLabel(tr("Updating..."));
    m_updatingLabel->setVisible(false);

    listLayout->addSpacing(10);
    listLayout->addWidget(m_bootList, 0, Qt::AlignHCenter);
    listLayout->addStretch();
    listLayout->addWidget(m_updatingLabel, 0, Qt::AlignHCenter);
    listLayout->addSpacing(10);
    m_background->setLayout(listLayout);

    m_boot = new SwitchWidget();
    m_boot->setTitle(tr("Startup Delay"));

    m_theme = new SwitchWidget();
    m_theme->setTitle(tr("Theme"));

    TipsLabel *label = new TipsLabel(tr("The boot image can be replaced by dragging one to it, "
                                        "and the boot order can be changed by dragging the highlight"));
    label->setWordWrap(true);
    label->setContentsMargins(20, 0, 20, 0);

    group->appendItem(m_background);
    group->appendItem(m_boot);
    group->appendItem(m_theme);

    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addSpacing(10);
    layout->addWidget(group);
    layout->addSpacing(8);
    layout->addWidget(label);
    layout->addStretch();

    widget->setLayout(layout);

    setContent(widget);

    connect(m_theme, SIGNAL(checkedChanged(bool)), this, SIGNAL(enableTheme(bool)));
    connect(m_boot, SIGNAL(checkedChanged(bool)), this, SIGNAL(bootdelay(bool)));
    connect(m_bootList, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this, SLOT(onCurrentItem(QListWidgetItem*,QListWidgetItem*)));
    connect(m_background, &GrubBackgroundItem::requestEnableTheme, this, &BootWidget::enableTheme);
}

void BootWidget::setDefaultEntry(const QString &value)
{
    m_defaultEntry = value;

    blockSignals(true);
    for (int i = 0; i < m_bootList->count(); i++) {
        QListWidgetItem * item = m_bootList->item(i);
        if (item->text() == value) {
            m_bootList->setCurrentRow(i);
        }
    }
    blockSignals(false);
}

#ifndef DCC_DISABLE_GRUB
void BootWidget::setModel(SystemInfoModel *model)
{
    connect(model, &SystemInfoModel::bootDelayChanged, m_boot, &SwitchWidget::setChecked);
    connect(model, &SystemInfoModel::themeEnabledChanged, m_theme, &SwitchWidget::setChecked);
    connect(model, &SystemInfoModel::defaultEntryChanged, this, &BootWidget::setDefaultEntry);
    connect(model, &SystemInfoModel::updatingChanged, m_updatingLabel, &SmallLabel::setVisible);
    connect(model, &SystemInfoModel::entryListsChanged, this, &BootWidget::setEntryList);
    connect(model, &SystemInfoModel::themeEnabledChanged, m_background, &GrubBackgroundItem::setThemeEnable);

    m_boot->setChecked(model->bootDelay());
    m_theme->setChecked(model->themeEnabled());
    m_updatingLabel->setVisible(model->updating());
    m_background->setThemeEnable(model->themeEnabled());

    setEntryList(model->entryLists());
    setDefaultEntry(model->defaultEntry());
}
#endif

void BootWidget::setEntryList(const QStringList &list)
{
    int currentIndex = 0;

    for(int i = 0; i<list.count(); i++)
    {
        const QString entry = list.at(i);

        QListWidgetItem* item = new QListWidgetItem(entry);
        item->setBackground(Qt::transparent);
        item->setSizeHint(QSize(200, 30));
        onCurrentItem(nullptr, item);
        m_bootList->addItem(item);

        if (m_defaultEntry == entry) {
            currentIndex = i;
        }
    }

    m_bootList->setCurrentRow(currentIndex);
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

        // m_defaultEntry is empty means the data is being initialized.
        if (!m_defaultEntry.isEmpty()) {
            emit defaultEntry(cur->text());
        }
    }
}

}
}
