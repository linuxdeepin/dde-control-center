/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     duanhongyu <duanhongyu@uniontech.com>
*
* Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#include "privacyplugin.h"
#include "privacysecuritymodel.h"
#include "privacysecurityworker.h"
#include "widgets/widgetmodule.h"

#include <servicesettingsmodule.h>

/*
相机、麦克风、所有文件夹、用户文件夹、日历、屏幕截图、
强制重启设备、卸载应用、修改系统目录
*/

DCC_USE_NAMESPACE

PrivacyModule::PrivacyModule(QObject *parent)
    : ModuleObject("Privacy and Security", tr("Privacy and Security"), tr("Privacy and Security"), QIcon::fromTheme("dcc_nav_privacy"),parent)
    , m_model(new PrivacySecurityModel(this))
    , m_work(new PrivacySecurityWorker(m_model, this))
{
    // 一级
    setChildType(ModuleObject::VList);

    for (DCC_PRIVACY_NAMESPACE::DATE iter : m_model->getModuleInfo()) {
        // 添加三级页面
        appendChild(new ServiceSettingsModule(iter, m_model, m_work, this));
    }
}

void PrivacyModule::active()
{
    m_work->activate();
}

PrivacyPlugin::PrivacyPlugin()
    : PluginInterface()
    , m_moduleRoot(nullptr)
{
}

PrivacyPlugin::~PrivacyPlugin()
{
    m_moduleRoot = nullptr;
}

QString PrivacyPlugin::name() const
{
    return QStringLiteral("Privacy and Security");
}

ModuleObject *PrivacyPlugin::module()
{
    if (m_moduleRoot)
        return m_moduleRoot;

    m_moduleRoot = new PrivacyModule;
    return m_moduleRoot;
}

int PrivacyPlugin::location() const
{
    return 0;
}
