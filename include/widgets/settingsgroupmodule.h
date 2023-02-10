/*
 * Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
 *
 * Author:     caixiangrong <caixiangrong@uniontech.com>
 *
 * Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
    explicit SettingsGroupModule(const QString &name,
                                 const QString &displayName,
                                 QObject *parent = nullptr);
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
} // namespace DCC_NAMESPACE
#endif // SETTINGSGROUPMODULE_H
