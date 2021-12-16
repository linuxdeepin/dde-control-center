/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "networkdetailpage.h"
#include "window/utils.h"

#include <QVBoxLayout>

#include <widgets/settingshead.h>
#include <widgets/settingsgroup.h>
#include <widgets/titlevalueitem.h>

#include <networkcontroller.h>
#include <networkdetails.h>

using namespace dcc::widgets;
using namespace dde::network;

NetworkDetailPage::NetworkDetailPage(QWidget *parent)
    : ContentWidget(parent)
{
    QWidget *mainWidget = new TranslucentFrame;

    m_groupsLayout = new QVBoxLayout(mainWidget);
    m_groupsLayout->setSpacing(0);
    m_groupsLayout->setMargin(0);
    m_groupsLayout->setContentsMargins(ThirdPageContentsMargins);
    m_groupsLayout->setAlignment(Qt::AlignTop);

    mainWidget->setLayout(m_groupsLayout);

    setTitle(tr("Network Details"));
    layout()->setContentsMargins(0, 8, 0, 8);
    setContent(mainWidget);

    connect(NetworkController::instance(), &NetworkController::activeConnectionChange, this, &NetworkDetailPage::onUpdateNetworkInfo);

    onUpdateNetworkInfo();
}

NetworkDetailPage::~NetworkDetailPage()
{
}

void NetworkDetailPage::onUpdateNetworkInfo()
{
    auto appendInfo = [](SettingsGroup *group, const QString &title, const QString &value) {
        TitleValueItem *i = new TitleValueItem;
        i->setTitle(title);
        i->setValue(value);
        group->appendItem(i);
        if (title == "IPv6")
            i->setWordWrap(false);
    };

    while (QLayoutItem *item = m_groupsLayout->takeAt(0)) {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    QList<NetworkDetails *> netDetails = NetworkController::instance()->networkDetails();
    int size = netDetails.size();
    for (int i = 0; i < size; i++) {
        NetworkDetails *detail = netDetails[i];
        SettingsGroup *itemGroup = new SettingsGroup;
        SettingsHead *head = new SettingsHead;
        head->setEditEnable(false);
        head->setContentsMargins(10, 0, 0, 0);
        head->setTitle(detail->name());
        itemGroup->appendItem(head, SettingsGroup::BackgroundStyle::NoneBackground);
        QList<QPair<QString, QString>> items = detail->items();
        for (const QPair<QString, QString> &item : items)
            appendInfo(itemGroup, item.first, item.second);

        m_groupsLayout->addWidget(itemGroup);
        if (i < size - 1)
            m_groupsLayout->addSpacing(30);
    }
}
