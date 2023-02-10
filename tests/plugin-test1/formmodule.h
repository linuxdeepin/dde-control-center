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
#ifndef FORMMODULE_H
#define FORMMODULE_H

#include "interface/moduleobject.h"

class QFormLayout;
class QScrollArea;

class FormModule : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    explicit FormModule(const QString &name,
                        const QString &displayName = {},
                        QObject *parent = nullptr);

    QWidget *page() override;

private Q_SLOTS:
    void onCurrentModuleChanged(ModuleObject *child);

private:
    void onAddChild(DCC_NAMESPACE::ModuleObject *const childModule);
    void onRemoveChild(DCC_NAMESPACE::ModuleObject *const childModule);
    void clearData();

private:
    QMap<ModuleObject *, QWidget *> m_mapWidget;
    QScrollArea *m_area;
    QFormLayout *m_layout;
};

#endif // FORMMODULE_H
