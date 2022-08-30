// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "timezonelist.h"
#include "modules/datetime/timezoneitem.h"
#include "modules/datetime/timezone_dialog/timezone.h"
#include "modules/datetime/timezone_dialog/timezonechooser.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsitem.h"
#include "widgets/settingshead.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

using namespace dcc;
using namespace dcc::datetime;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::datetime;

TimezoneList::TimezoneList(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout)
    , m_addTimezoneButton(new DFloatingButton(DStyle::SP_IncreaseElement, this))
    , m_headGroup(new SettingsGroup)
    , m_headItem(new SettingsHead)
    , m_contentList(new TimezoneContentList(this))
{
    setAccessibleName("TimezoneList");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *m_headLayout = new QVBoxLayout;
    m_headItem->setEditEnable(true);
    m_headItem->setVisible(true);
    m_headItem->setTitle(tr("Timezone List"));
    m_headGroup->appendItem(m_headItem);
    m_headItem->removeBackground();

    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);
    m_centralLayout->addLayout(m_headLayout);
    m_centralLayout->addWidget(m_headGroup);
    m_centralLayout->addSpacing(15);
    m_centralLayout->addWidget(m_contentList);
    m_centralLayout->setContentsMargins(10,0,10,0);

    m_contentList->setSpace(List_Interval);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->setMargin(0);
    btnLayout->setAlignment(Qt::AlignBottom | Qt::AlignHCenter);
    btnLayout->addWidget(m_addTimezoneButton);
    m_centralLayout->addLayout(btnLayout);
    setLayout(m_centralLayout);

    //~ contents_path /datetime/Timezone List/Add Timezone
    //~ child_page Timezone List
    m_addTimezoneButton->setToolTip(tr("Add Timezone"));
    m_addTimezoneButton->setAccessibleName(tr("Add Timezone"));

    connect(m_addTimezoneButton, &QPushButton::clicked, this, [this]{
        m_contentList->onEditClicked(false);
        m_headItem->toCancel();
        requestAddTimeZone();
    });
    GSettingWatcher::instance()->bind("datetimeZonelistAddtimezone", m_addTimezoneButton);

    connect(m_headItem, &SettingsHead::editChanged, m_contentList, &TimezoneContentList::onEditClicked);
    connect(m_contentList, &TimezoneContentList::requestRemoveUserTimeZone, this, [this]() {
        //refresh right button to edit
        m_headItem->toCancel();
    });

    connect(m_contentList, &TimezoneContentList::notifyItemCount, this, [this](int value) {
        m_headGroup->setVisible(value > 1);
    });
}

TimezoneList::~TimezoneList()
{
    GSettingWatcher::instance()->erase("datetimeZonelistAddtimezone", m_addTimezoneButton);
}

TimezoneContentList *TimezoneList::getTimezoneContentListPtr()
{
    return m_contentList;
}
