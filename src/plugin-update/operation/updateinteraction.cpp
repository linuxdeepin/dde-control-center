// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updateinteraction.h"

#include "dccfactory.h"

#include <QtQml/qqml.h>

UpdateInteraction::UpdateInteraction(QObject *parent)
    : QObject{ parent }
    , m_work(nullptr)
    , m_model(nullptr)
{
    m_model = new UpdateModel(this);
    m_work = new UpdateWork(m_model, this);

    qmlRegisterType<UpdateWork>("org.deepin.dcc.update", 1, 0, "UpdateWork");
    qmlRegisterType<UpdateModel>("org.deepin.dcc.update", 1, 0, "UpdateModel");

  //  m_work->activate();
}

UpdateWork *UpdateInteraction::work() const
{
    return m_work;
}

void UpdateInteraction::setWork(UpdateWork *newWork)
{
    m_work = newWork;
}

UpdateModel *UpdateInteraction::model() const
{
    return m_model;
}

void UpdateInteraction::setModel(UpdateModel *newModel)
{
    m_model = newModel;
}

DCC_FACTORY_CLASS(UpdateInteraction)


#include "updateinteraction.moc"
