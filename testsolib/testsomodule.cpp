/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
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

#include "testsomodule.h"
#include "testsowidget.h"
#include "thirdpage1.h"

#include <QtDebug>

using namespace DCC_NAMESPACE;

TestsoModule::TestsoModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
{
}

void TestsoModule::preInitialize(bool sync)
{
}

void TestsoModule::initialize()
{

}

void TestsoModule::reset()
{

}

void TestsoModule::active()
{
    TestsoWidget *soWidget = new TestsoWidget;
    soWidget->initUI();

    connect(soWidget, &TestsoWidget::showPage1, this, &TestsoModule::showGeneralSetting);

    m_frameProxy->pushWidget(this, soWidget);
    showGeneralSetting();
}

const QString TestsoModule::name() const
{
    return QStringLiteral("testso");
}

const QString TestsoModule::displayName() const
{
    return tr("testso");
}

int TestsoModule::load(QString path)
{
    return 1;
}

QStringList TestsoModule::availPage() const
{
    QStringList sl;
    sl << "General" << "Mouse";

    return sl;
}

void TestsoModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void TestsoModule::showGeneralSetting()
{
    ThirdPage1 *thirdPage = new ThirdPage1();
    m_frameProxy->pushWidget(this, thirdPage);
}
