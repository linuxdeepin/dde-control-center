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
#ifndef HORIZONTALMODULE_H
#define HORIZONTALMODULE_H

#include "interface/moduleobject.h"
DCC_BEGIN_NAMESPACE
class HorizontalModulePrivate;
class HorizontalModule : public ModuleObject
{
    Q_OBJECT
public:
    enum StretchType {
        NoStretch = 0,
        LeftStretch = 1,
        RightStretch = 2,
        AllStretch = LeftStretch | RightStretch,
    };
    explicit HorizontalModule(const QString &name, const QString &displayName, QObject *parent = nullptr);
    ~HorizontalModule() override;

    void setStretchType(StretchType stretchType);
    void setSpacing(const int spacing);

    void appendChild(ModuleObject *const module) override;
    void insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module) override;
    void insertChild(const int index, ModuleObject *const module) override;
    void removeChild(ModuleObject *const module) override;
    void removeChild(const int index) override;

    void appendChild(ModuleObject *const module, int stretch, Qt::Alignment alignment = Qt::Alignment());
    void insertChild(QList<ModuleObject *>::iterator before, ModuleObject *const module, int stretch, Qt::Alignment alignment = Qt::Alignment());
    void insertChild(const int index, ModuleObject *const module, int stretch, Qt::Alignment alignment = Qt::Alignment());

    QWidget *page() override;
    inline DCC_MODULE_TYPE getClassID() const override { return HORIZONTAL; }

    DCC_DECLARE_PRIVATE(HorizontalModule)
};
DCC_END_NAMESPACE
#endif // HORIZONTALMODULE_H
