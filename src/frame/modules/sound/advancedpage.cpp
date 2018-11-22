/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "advancedpage.h"
#include "widgets/translucentframe.h"

using namespace dcc;

namespace dcc {
namespace sound {

AdvancedPage::AdvancedPage(SoundModel *model)
    : ContentWidget(),
      m_model(model),
      m_outputGroup(new SettingsGroup(tr("Output"))),
      m_inputGroup(new SettingsGroup(tr("Input")))
{
    setTitle(tr("Advanced"));
    setModel(model);

    TranslucentFrame *frame = new TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setSpacing(10);
    layout->setMargin(0);
    layout->addSpacing(10);
    layout->addWidget(m_outputGroup);
    layout->addWidget(m_inputGroup);

    setContent(frame);
}

AdvancedPage::~AdvancedPage()
{
    for (PortItem *p :m_portItemList) {
        if(p)
            p->deleteLater();
    }
}

void AdvancedPage::setModel(SoundModel *model)
{
    m_model = model;

    connect(m_model, &SoundModel::portAdded, this, &AdvancedPage::addPort);
    connect(m_model, &SoundModel::portRemoved, this, &AdvancedPage::removePort);

    for (Port* const port : m_model->ports()) {
        addPort(port);
    }
}

void AdvancedPage::addPort(const Port *port)
{
    PortItem *item = new PortItem(port);
    item->setContentsMargins(20, 0, 10, 0);
    connect(item, &PortItem::selectedChanged, [this, port] {
        Q_EMIT requestSetPort(port);
    });

    if (port->direction() == Port::Out) {
        m_outputGroup->appendItem(item);
    } else {
        m_inputGroup->appendItem(item);
    }
    m_portItemList.append(item);
}

void AdvancedPage::removePort(const QString &portId, const uint &cardId)
{
    for (PortItem *item : m_portItemList) {
        if (item->port()->id() == portId && item->port()->cardId() == cardId) {

            if (item->direction() == Port::Out)
                m_outputGroup->removeItem(item);
            else
                m_inputGroup->removeItem(item);

            m_portItemList.removeOne(item);
            item->deleteLater();
        }
    }
}

} // namespace sound
} // namespace dcc
