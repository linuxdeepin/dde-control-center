//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SERVICESETTINGSMODULE_H
#define SERVICESETTINGSMODULE_H

#include "interface/pagemodule.h"
#include <dtkwidget_global.h>
#include <QObject>
#include <privacysecuritymodel.h>

namespace DCC_NAMESPACE {
class SwitchWidget;
class SettingsGroup;
}

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DListView;
DWIDGET_END_NAMESPACE

class PrivacySecurityWorker;

// 三级详情页
class ServiceSettingsModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit ServiceSettingsModule(DCC_PRIVACY_NAMESPACE::DATE& serviceDate, PrivacySecurityModel *model, PrivacySecurityWorker *work, QObject *parent = nullptr);
    ~ServiceSettingsModule();

    void initTopTipsLabel(QLabel *tipsLabel);
    void initSwitchWidget(DCC_NAMESPACE::SwitchWidget *titleSwitch);
    void initListView(DTK_WIDGET_NAMESPACE::DListView *settingsGrp);
    void initNoServiceLabel(QWidget *noServiceLabel);
    void creatPluginAppsView(Dtk::Widget::DListView *appsListView);

private:
    QString getTopTipsDoc(DCC_PRIVACY_NAMESPACE::ServiceCategory category);
    QString getNoneTipsDoc(DCC_PRIVACY_NAMESPACE::ServiceCategory category);
signals:

private:
    DCC_PRIVACY_NAMESPACE::DATE m_currentServiceDate;
    ServiceControlItems *m_serviceItemDate;

    PrivacySecurityModel *m_model;
    PrivacySecurityWorker *m_worker;

    ModuleObject *m_appsListView;
};

#endif // SERVICESETTINGSMODULE_H
