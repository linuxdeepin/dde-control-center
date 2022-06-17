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
#ifndef LAYOUTV20_H
#define LAYOUTV20_H
#include "interface/layoutbase.h"

#define LayoutV20_KEY (DCC_NAMESPACE::ModuleObject::Page | 20)

#define WIDGETCHANGED_FLAG 0x00000100
#define widgetChanged() setFlagState(WIDGETCHANGED_FLAG, !getFlagState(WIDGETCHANGED_FLAG))

class QHBoxLayout;
class QScrollArea;
class LayoutV20 : public DCC_NAMESPACE::LayoutBase
{
public:
    explicit LayoutV20();
    ~LayoutV20() { }

    virtual void setCurrent(DCC_NAMESPACE::ModuleObject *const child);
    virtual QWidget *layoutModule(DCC_NAMESPACE::ModuleObject *const module, QWidget *const parent, const QList<DCC_NAMESPACE::ModuleObject *> &children);

private:
    QList<DCC_NAMESPACE::ModuleObject *> m_modules;
    QHBoxLayout *m_layout;
};

#endif // LAYOUTV20_H
