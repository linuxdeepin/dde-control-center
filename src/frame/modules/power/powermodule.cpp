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

#include "powermodule.h"
#include "powerwidget.h"
#include "powerworker.h"
#include "powermodel.h"

using namespace dcc;
using namespace dcc::power;

PowerModule::PowerModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
      ModuleInterface(frame),

      m_powerWidget(nullptr)
{

}

PowerModule::~PowerModule()
{
    m_powerModel->deleteLater();
    m_powerWorker->deleteLater();
}

void PowerModule::initialize()
{
    m_powerModel = new PowerModel;
    m_powerWorker = new PowerWorker(m_powerModel);

    m_powerModel->moveToThread(qApp->thread());
    m_powerWorker->moveToThread(qApp->thread());
}

void PowerModule::moduleActive()
{
    m_powerWorker->active();
}

void PowerModule::moduleDeactive()
{
    m_powerWorker->deactive();
}

void PowerModule::reset()
{

}

void PowerModule::contentPopped(ContentWidget * const w)
{
    Q_UNUSED(w);
}

ModuleWidget *PowerModule::moduleWidget()
{
    if (!m_powerWidget)
    {
        m_powerWidget = new PowerWidget;
        m_powerWidget->setModel(m_powerModel);

        connect(m_powerWidget, &PowerWidget::requestSetScreenBlackLock, m_powerWorker, &PowerWorker::setScreenBlackLock);
        connect(m_powerWidget, &PowerWidget::requestSetSleepLock, m_powerWorker, &PowerWorker::setSleepLock);
        connect(m_powerWidget, &PowerWidget::requestSetSleepOnLidClosed, m_powerWorker, &PowerWorker::setSleepOnLidClosed);
        connect(m_powerWidget, &PowerWidget::requestSetScreenBlackDelayOnPower, m_powerWorker, &PowerWorker::setScreenBlackDelayOnPower);
        connect(m_powerWidget, &PowerWidget::requestSetScreenBlackDelayOnBattery, m_powerWorker, &PowerWorker::setScreenBlackDelayOnBattery);
        connect(m_powerWidget, &PowerWidget::requestSetSleepDelayOnPoewr, m_powerWorker, &PowerWorker::setSleepDelayOnPower);
        connect(m_powerWidget, &PowerWidget::requestSetSleepDelayOnBattery, m_powerWorker, &PowerWorker::setSleepDelayOnBattery);
#ifndef DCC_DISABLE_POWERSAVE
        connect(m_powerWidget, &PowerWidget::requestSetEnableAutoPSM, m_powerWorker, &PowerWorker::setAutoEnablePowerSave);
        connect(m_powerWidget, &PowerWidget::requestSetPowerSaveMode, m_powerWorker, &PowerWorker::setEnablePowerSave);
#endif
    }

    return m_powerWidget;
}

const QString PowerModule::name() const
{
    return QStringLiteral("power");
}
