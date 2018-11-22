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

#include "systeminfomodule.h"
#include "copyrightwidget.h"
#include "bootwidget.h"
#include "userlicense.h"

namespace dcc{
namespace systeminfo{

SystemInfoModule::SystemInfoModule(FrameProxyInterface *frame, QObject *parent)
    :QObject(parent),
      ModuleInterface(frame),
#ifndef DCC_DISABLE_GRUB
      m_bootWidget(nullptr),
#endif
      m_mainWidget(nullptr),
      m_copyrightWidget(nullptr)

{
}

SystemInfoModule::~SystemInfoModule()
{
    m_work->deleteLater();
    m_model->deleteLater();

    if(m_mainWidget)
        m_mainWidget->deleteLater();
}

void SystemInfoModule::initialize()
{
    m_model = new SystemInfoModel();
    m_work = new SystemInfoWork(m_model);

    m_work->moveToThread(qApp->thread());
    m_model->moveToThread(qApp->thread());

#ifndef DCC_DISABLE_GRUB
    connect(m_work, &SystemInfoWork::requestSetAutoHideDCC, this, &SystemInfoModule::onSetAutoHide);
#endif
}

void SystemInfoModule::moduleActive()
{
    m_work->activate();
}

void SystemInfoModule::moduleDeactive()
{
    m_work->deactivate();
}

void SystemInfoModule::reset()
{

}

ModuleWidget *SystemInfoModule::moduleWidget()
{
    if(!m_mainWidget)
    {
        m_mainWidget = new SystemInfoWidget(m_model);
        connect(m_mainWidget, SIGNAL(copyright()), this, SLOT(onPushCopyright()));
#ifndef DCC_DISABLE_GRUB
        connect(m_mainWidget, SIGNAL(boot()), this, SLOT(onPushBoot()));
#endif
    }

    return m_mainWidget;
}

const QString SystemInfoModule::name() const
{
    return QStringLiteral("systeminfo");
}

void SystemInfoModule::contentPopped(ContentWidget * const w)
{
    if(w == m_copyrightWidget)
        m_copyrightWidget = nullptr;
#ifndef DCC_DISABLE_GRUB
    else if(w == m_bootWidget)
        m_bootWidget = nullptr;
#endif

    w->deleteLater();
}

void SystemInfoModule::onPushCopyright()
{
#ifndef DCC_ENABLE_END_USER_LICENSE
    if(!m_copyrightWidget)
    {
        m_copyrightWidget = new CopyrightWidget();
    }

    m_frameProxy->pushWidget(this, m_copyrightWidget);
#else
    m_frameProxy->pushWidget(this, new UserLicense);
#endif
}

#ifndef DCC_DISABLE_GRUB
void SystemInfoModule::onPushBoot()
{
    m_work->loadGrubSettings();

    if(!m_bootWidget)
    {
        m_bootWidget = new BootWidget;
        m_bootWidget->setModel(m_model);

        connect(m_bootWidget, SIGNAL(bootdelay(bool)), m_work, SLOT(setBootDelay(bool)));
        connect(m_bootWidget, SIGNAL(enableTheme(bool)),m_work, SLOT(setEnableTheme(bool)));
        connect(m_bootWidget, SIGNAL(defaultEntry(QString)), m_work, SLOT(setDefaultEntry(QString)));
        connect(m_bootWidget, &BootWidget::requestSetBackground, m_work, &SystemInfoWork::setBackground);
    }

    m_frameProxy->pushWidget(this, m_bootWidget);
}
#endif

void SystemInfoModule::onSetAutoHide(const bool visible)
{
    m_frameProxy->setFrameAutoHide(this, visible);
}

}
}
