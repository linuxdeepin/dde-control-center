//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "powerinterface.h"
#include "powermodel.h"
#include "dccfactory.h"
#include "poweroperatormodel.h"
#include "utils.h"

PowerInterface::PowerInterface(QObject *parent) 
: QObject(parent)
, m_model(new PowerModel(this))
, m_worker(new PowerWorker(m_model, this))
, m_powerLidClosedOperatorModel(new PowerOperatorModel(this))
, m_powerPressedOperatorModel(new PowerOperatorModel(this))
, m_batteryLidClosedOperatorModel(new PowerOperatorModel(this))
, m_batteryPressedOperatorModel(new PowerOperatorModel(this))
{
    m_model->setSuspend(!IsServerSystem && true);
    m_model->setHibernate(!IsServerSystem && true);
    m_model->setShutdown(true);
}

DCC_FACTORY_CLASS(PowerInterface)

#include "powerinterface.moc"