/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#include "widgets/settingsgroup.h"
#include "modules/sound/portitem.h"
#include "modules/sound/soundmodel.h"

using namespace dcc::sound;
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::sound;

AdvancedPage::AdvancedPage(QWidget *parent)
    : QWidget(parent)
    , m_layout(new QVBoxLayout)
    , m_inputGroup(new SettingsGroup)
    , m_outputGroup(new SettingsGroup)
{
    m_layout->setMargin(10);
    QLabel *label = new QLabel(tr("Output"));
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_layout->addWidget(label);
    m_layout->addWidget(m_outputGroup);

    label = new QLabel(tr("Input"));
    label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_layout->addWidget(label);
    m_layout->addWidget(m_inputGroup);

    m_layout->addStretch(1);
    setLayout(m_layout);
}

AdvancedPage::~AdvancedPage()
{
}

void AdvancedPage::setModel(dcc::sound::SoundModel *model)
{
    m_model = model;

    connect(m_model, &SoundModel::portAdded, this, &AdvancedPage::addPort);
    connect(m_model, &SoundModel::portRemoved, this, &AdvancedPage::removePort);

    initList();
}

void AdvancedPage::initList()
{
    if (m_portItemList.size()) {
        for (auto item : m_portItemList)
            item->deleteLater();
    }
    m_portItemList.clear();

    auto ports = m_model->ports();
    for (auto port : ports) {
        addPort(port);
    }
}

void AdvancedPage::addPort(const Port *port)
{
    PortItem *pi = new PortItem(port);
    pi->setContentsMargins(20, 0, 10, 0);
    connect(pi, &PortItem::selectedChanged, [this, port] {
        Q_EMIT requestSetPort(port);
    });

    if (port->direction() == Port::Out) {
        m_outputGroup->appendItem(pi);
    } else {
        m_inputGroup->appendItem(pi);
    }
    m_portItemList.append(pi);
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
