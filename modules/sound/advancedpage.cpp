/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "advancedpage.h"
#include "translucentframe.h"

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
    layout->addWidget(m_outputGroup);
    layout->addWidget(m_inputGroup);

    setContent(frame);
}

void AdvancedPage::setModel(SoundModel *model)
{
    m_model = model;

    connect(m_model, &SoundModel::portAdded, this, &AdvancedPage::addPort);
    connect(m_model, &SoundModel::portRemoved, this, &AdvancedPage::removePort);

    for (Port *port : m_model->ports()) {
        addPort(port);
    }
}

void AdvancedPage::addPort(const Port *port)
{
    PortItem *item = new PortItem(port);
    item->setContentsMargins(20, 0, 10, 0);
    connect(item, &PortItem::selectedChanged, [this, port] (bool selected) {
        if (selected) {
            emit requestSetPort(port);
        }
    });

    if (port->direction() == Port::Out) {
        m_outputGroup->appendItem(item);
    } else {
        m_inputGroup->appendItem(item);
    }
}

void AdvancedPage::removePort(const QString &portId)
{
    QList<PortItem *> outputPortItems = m_outputGroup->findChildren<PortItem*>();
    for (PortItem *item : outputPortItems) {
        if (item->port()->id() == portId) {
            m_outputGroup->removeItem(item);
            return;
        }
    }

    QList<PortItem *> inputPortItems = m_inputGroup->findChildren<PortItem*>();
    for (PortItem *item : inputPortItems) {
        if (item->port()->id() == portId) {
            m_inputGroup->removeItem(item);
            return;
        }
    }
}

} // namespace sound
} // namespace dcc
