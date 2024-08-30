//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "privacyplugin.h"
#include "privacysecuritymodel.h"
#include "privacysecurityworker.h"
#include "widgets/widgetmodule.h"

#include <servicesettingsmodule.h>

/*
相机、麦克风、所有文件夹、用户文件夹、日历、屏幕截图、
强制重启设备、卸载应用、修改系统目录
*/

using namespace DCC_NAMESPACE;

PrivacyModule::PrivacyModule(QObject *parent)
    : VListModule("privacyAndSecurity", tr("Privacy and Security"), tr("Privacy and Security"), DIconTheme::findQIcon("dcc_nav_privacy"),parent)
    , m_model(new PrivacySecurityModel(this))
    , m_work(new PrivacySecurityWorker(m_model, this))
{
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
    return QStringLiteral("privacyAndSecurity");
}

ModuleObject *PrivacyPlugin::module()
{
    if (m_moduleRoot)
        return m_moduleRoot;

    m_moduleRoot = new PrivacyModule;
    return m_moduleRoot;
}

QString PrivacyPlugin::location() const
{
    return "0";
}
