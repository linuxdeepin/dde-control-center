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

#include "defaultappsmodule.h"
#include "defappwidget.h"

#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::defapp;

DefaultAppsModule::DefaultAppsModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_defaultappsWidget(nullptr)
{

}

DefaultAppsModule::~DefaultAppsModule()
{
}

void DefaultAppsModule::initialize()
{
}

void DefaultAppsModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName);
}

void DefaultAppsModule::reset()
{

}

QWidget *DefaultAppsModule::moduleWidget()
{
    if (!m_defaultappsWidget) {
        m_defaultappsWidget = new DefaultAppsWidget;
    }

    return m_defaultappsWidget;
}

const QString DefaultAppsModule::name() const
{
    return QStringLiteral("defapp");
}

void DefaultAppsModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}
