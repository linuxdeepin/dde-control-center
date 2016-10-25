#include "datetime.h"
#include "contentwidget.h"
#include "datetimeutil.h"
#include "timezoneitem.h"
#include "datesettings.h"
#include <QFrame>
#include <QDebug>
#include <QPushButton>

Datetime::Datetime()
    :ModuleWidget()
{
    this->installEventFilter(parent());
    setTitle(tr("Time and Date"));
    Clock* clock = new Clock();
    clock->setDisplay(true);
    SettingsGroup* g = new SettingsGroup();
    g->appendItem(clock);

    m_group = new SettingsGroup();
    m_centeralLayout->addWidget(g);
    SettingsItem *f  = new SettingsItem();
    QPushButton* btn = new QPushButton();
    btn->setFixedHeight(30);
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(btn);
    f->setLayout(layout);
    m_group->appendItem(f);
    m_centeralLayout->addSpacerItem(new QSpacerItem(300,20));
    m_centeralLayout->addWidget(m_group);

    SettingsGroup* addTimezone = new SettingsGroup();
    SettingsItem* addItem = new SettingsItem();
    QHBoxLayout* addLayout = new QHBoxLayout();
    QPushButton* addBtn = new QPushButton(tr("Add Timezone"));
    addLayout->addWidget(addBtn);
    addItem->setLayout(addLayout);
    addTimezone->appendItem(addItem);

    SettingsGroup* timeSettings = new SettingsGroup();
    SettingsItem* timeItem = new SettingsItem();
    QHBoxLayout* timeLayout = new QHBoxLayout();
    QPushButton* timeBtn = new QPushButton(tr("Time Settings"));
    timeLayout->addWidget(timeBtn);
    timeItem->setLayout(timeLayout);

    timeSettings->appendItem(timeItem);

    m_centeralLayout->addSpacerItem(new QSpacerItem(300,20));
    m_centeralLayout->addWidget(addTimezone);
    m_centeralLayout->addSpacerItem(new QSpacerItem(300,20));
    m_centeralLayout->addWidget(timeSettings);

    DateSettings* ds = new DateSettings();
    m_dateSettings = new ContentWidget;
    m_dateSettings->setTitle(tr("Date time details"));
    m_dateSettings->setContent(ds);

    connect(addBtn, SIGNAL(clicked()), this, SLOT(slotClick()));
//    connect(timeBtn, &QPushButton::clicked, [this] { pushWidget(m_dateSettings); });
}

void Datetime::addTimezone(const QString &city)
{
    TimezoneItem* item = new TimezoneItem();
    item->setCity(city);
    item->setFixedHeight(40);
    m_group->appendItem(item);
}

void Datetime::slotClick()
{
    QStringList lists;
    lists<<"Wuhan"<<"Shahrak"<<"Chimanimani";

    for(int i = 0; i<lists.count(); i++)
        addTimezone(lists[i]);
}
