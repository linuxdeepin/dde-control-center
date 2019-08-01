/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "personalizationmodule.h"
#include "personalizationlist.h"
#include "personalizationgeneral.h"
#include "modules/personalization/personalizationmodel.h"
#include "modules/personalization/personalizationwork.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;

PersonalizationModule::PersonalizationModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_model(nullptr)
    , m_work(nullptr)
{

}

PersonalizationModule::~PersonalizationModule()
{
    m_model->deleteLater();
    m_work->deleteLater();
}

void PersonalizationModule::initialize()
{
    m_model  = new dcc::personalization::PersonalizationModel;
    m_work = new dcc::personalization::PersonalizationWork(m_model);

    m_model->moveToThread(qApp->thread());
    m_work->moveToThread(qApp->thread());
}

void PersonalizationModule::reset()
{

}

const QString PersonalizationModule::name() const
{
    return QStringLiteral("personalization");
}

void PersonalizationModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

QWidget *PersonalizationModule::moduleWidget()
{
    PersonalizationList *firstWidget = new PersonalizationList();
    connect(firstWidget, &PersonalizationList::requestShowGeneral, this, &PersonalizationModule::showGenaralWidget);
    return firstWidget;
}

void PersonalizationModule::contentPopped(QWidget * const w)
{
    Q_UNUSED(w);
}

void PersonalizationModule::showGenaralWidget()
{
}
