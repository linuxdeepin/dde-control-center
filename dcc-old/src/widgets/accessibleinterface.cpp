// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/accessibleinterface.h"

#include "accessiblefactoryinterface.h"

#include <QList>

static AccessibleFactoryInterface *s_accessibleFactoryInterface = nullptr;

auto &get_cacheFactory()
{
    static QList<QPair<QString, AccessibleFactoryBase *>> s_cacheFactory;
    return s_cacheFactory;
}

AccessibleFactoryBase *AccessibleFactoryManager::RegisterAccessibleFactory(
        const char *factoryName, AccessibleFactoryBase *factory)
{
    if (s_accessibleFactoryInterface) {
        s_accessibleFactoryInterface->registerAccessibleFactory(factoryName, factory);
    } else {
        auto &s_cacheFactory = get_cacheFactory();
        s_cacheFactory.append({ factoryName, factory });
    }
    return factory;
}

AccessibleFactoryInterface::AccessibleFactoryInterface() { }

AccessibleFactoryInterface::~AccessibleFactoryInterface() { }

void AccessibleFactoryInterface::RegisterInstance(AccessibleFactoryInterface *inter)
{
    if (!s_accessibleFactoryInterface) {
        auto &s_cacheFactory = get_cacheFactory();
        s_accessibleFactoryInterface = inter;
        for (auto &&factory : s_cacheFactory) {
            s_accessibleFactoryInterface->registerAccessibleFactory(factory.first.toLatin1().data(),
                                                                    factory.second);
        }
        s_cacheFactory.clear();
    }
}
