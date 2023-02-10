/*
 * Copyright (C) 2018 ~ 2028 Uniontech Technology Co., Ltd.
 *
 * Author:     chenwei <chenwei@uniontech.com>
 *
 * Maintainer: chenwei <chenwei@uniontech.com>
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
#include "widgets/accessibleinterface.h"

#include "accessiblefactoryinterface.h"

#include <QList>

static AccessibleFactoryInterface *s_accessibleFactoryInterface = nullptr;
static QList<QPair<QString, AccessibleFactoryBase *>> s_cacheFactory;

AccessibleFactoryBase *AccessibleFactoryManager::RegisterAccessibleFactory(
        const char *factoryName, AccessibleFactoryBase *factory)
{
    if (s_accessibleFactoryInterface) {
        s_accessibleFactoryInterface->registerAccessibleFactory(factoryName, factory);
    } else {
        s_cacheFactory.append({ factoryName, factory });
    }
    return factory;
}

AccessibleFactoryInterface::AccessibleFactoryInterface() { }

AccessibleFactoryInterface::~AccessibleFactoryInterface() { }

void AccessibleFactoryInterface::RegisterInstance(AccessibleFactoryInterface *inter)
{
    if (!s_accessibleFactoryInterface) {
        s_accessibleFactoryInterface = inter;
        for (auto &&factory : s_cacheFactory) {
            s_accessibleFactoryInterface->registerAccessibleFactory(factory.first.toLatin1().data(),
                                                                    factory.second);
        }
        s_cacheFactory.clear();
    }
}
