//SPDX-FileCopyrightText: 2018 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "commoninfointeraction.h"
#include "dccfactory.h"

#include <QtQml/qqml.h>

CommonInfoInteraction::CommonInfoInteraction(QObject *parent)
    : QObject{ parent }
    , m_work(NULL)
    , m_mode(NULL)
{
    m_mode = new CommonInfoModel(this);
    m_work = new CommonInfoWork(m_mode, this);

    m_work->active();
}

CommonInfoWork *CommonInfoInteraction::work() const
{
    return m_work;
}

void CommonInfoInteraction::setWork(CommonInfoWork *newWork)
{
    m_work = newWork;
}

CommonInfoModel *CommonInfoInteraction::mode() const
{
    return m_mode;
}

void CommonInfoInteraction::setMode(CommonInfoModel *newMode)
{
    m_mode = newMode;
}

DCC_FACTORY_CLASS(CommonInfoInteraction,\
                   qmlRegisterType<CommonInfoWork>("org.deepin.dcc.systemInfo", 1, 0, "CommonInfoWork"),\
                   qmlRegisterType<CommonInfoModel>("org.deepin.dcc.systemInfo", 1, 0, "CommonInfoModel"))


#include "commoninfointeraction.moc"
