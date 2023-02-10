//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SETTINGSGROUPMODULE_H
#define SETTINGSGROUPMODULE_H

#include "interface/moduleobject.h"
#include "settingsgroup.h"
namespace DCC_NAMESPACE {
class SettingsGroupModulePrivate;
class SettingsGroupModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit SettingsGroupModule(const QString &name, const QString &displayName, QObject *parent = nullptr);
    ~SettingsGroupModule() override;

    void setHeaderVisible(const bool visible);
    void setSpacing(const int spacing);
    void setBackgroundStyle(SettingsGroup::BackgroundStyle bgStyle);
    SettingsGroup::BackgroundStyle backgroundStyle() const;
    void setSizePolicy(QSizePolicy::Policy hor, QSizePolicy::Policy ver);

    QWidget *page() override;
    inline DCC_MODULE_TYPE getClassID() const override { return SETTINGSGROUP; }

    DCC_DECLARE_PRIVATE(SettingsGroupModule)
};
}
#endif // SETTINGSGROUPMODULE_H
