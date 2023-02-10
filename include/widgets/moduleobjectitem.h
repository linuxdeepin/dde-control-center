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
#ifndef MODULEOBJECTITEM_H
#define MODULEOBJECTITEM_H

#include "interface/moduleobject.h"

#include <DStyle>

#include <QVariant>

namespace DCC_NAMESPACE {
class ModuleObjectItemPrivate;

class ModuleObjectItem : public ModuleObject
{
    Q_OBJECT
public:
    explicit ModuleObjectItem(const QString &name,
                              const QString &displayName,
                              QObject *parent = nullptr);
    ~ModuleObjectItem() override;

    void setRightIcon(DTK_WIDGET_NAMESPACE::DStyle::StandardPixmap st, int index = -1);
    void setRightIcon(const QString &icon, int index = -1);
    void setRightIcon(const QIcon &icon, int index = -1);
    void setRightText(const QString &text, int index = -1);
    DTK_WIDGET_NAMESPACE::DViewItemAction *getRightItem(int index = -1);
    void update();

    virtual void setData(int role, const QVariant &value);
    virtual QVariant data(int role) const;

    inline DCC_MODULE_TYPE getClassID() const override { return ITEM; }

Q_SIGNALS:
    void clicked();

    DCC_DECLARE_PRIVATE(ModuleObjectItem)
};
} // namespace DCC_NAMESPACE
#endif // MODULEOBJECTITEM_H
