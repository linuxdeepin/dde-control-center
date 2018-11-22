/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "chainstypepage.h"
#include "widgets/translucentframe.h"
#include "widgets/settingsgroup.h"
#include "widgets/optionitem.h"

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::network;

ChainsTypePage::ChainsTypePage(QWidget *parent) : ContentWidget(parent)
{
    setTitle(tr("Proxy Type"));

    OptionItem* http = new OptionItem;
    http->setTitle("http");
    http->setContentsMargins(20, 0, 10, 0);

    OptionItem* socks4 = new OptionItem;
    socks4->setTitle("socks4");
    socks4->setContentsMargins(20, 0, 10, 0);

    OptionItem* socks5 = new OptionItem;
    socks5->setTitle("socks5");
    socks5->setContentsMargins(20, 0, 10, 0);

    SettingsGroup *grp = new SettingsGroup;

    grp->appendItem(http);
    grp->appendItem(socks4);
    grp->appendItem(socks5);

    m_list << http << socks4 << socks5;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(0);

    layout->addSpacing(10);
    layout->addWidget(grp);

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(layout);

    setContent(w);

    for (OptionItem *item : m_list)
        connect(item, &OptionItem::selectedChanged, this, &ChainsTypePage::onTypeChanged);
}

void ChainsTypePage::setDefault(const QString &title)
{
    for (OptionItem *item : m_list) {
        item->blockSignals(true);
        item->setSelected(item->title() == title);
        item->blockSignals(false);
    }
}

void ChainsTypePage::onTypeChanged()
{
    OptionItem *item = qobject_cast<OptionItem*>(sender());

    Q_ASSERT(item);

    for (OptionItem *w : m_list) {
        if (w == item) {
            Q_EMIT requestTypeChanged(w->title());
            Q_EMIT back();
            break;
        }
    }

}
