/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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
#include "datetimemodule.h"
#include "datetimewidget.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::datetime;

dccV20::datetime::DatetimeModule::DatetimeModule(FrameProxyInterface *frameProxy, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frameProxy)
    , m_mainWidget(nullptr)
{

}

void dccV20::datetime::DatetimeModule::initialize()
{
    m_mainWidget = new DatetimeWidget;

//    QThread::currentThread()->msleep(500);

//    m_model = new dcc::DatetimeModel;
//    m_work = new dcc::DatetimeWork(m_model);

//    m_work->moveToThread(qApp->thread());
//    m_model->moveToThread(qApp->thread());

//    connect(m_work, &DatetimeWork::requestSetAutoHide, this, &DatetimeModule::setFrameAutoHide);
}

void dccV20::datetime::DatetimeModule::reset()
{

}

const QString dccV20::datetime::DatetimeModule::name() const
{
    return QStringLiteral("datetime");
}

void dccV20::datetime::DatetimeModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName)
}

QWidget *dccV20::datetime::DatetimeModule::moduleWidget()
{
    return m_mainWidget;
}

void dccV20::datetime::DatetimeModule::contentPopped(QWidget * const w)
{
    Q_UNUSED(w);
}
