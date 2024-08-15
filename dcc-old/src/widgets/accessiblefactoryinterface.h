//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ACCESSIBLEFACTORYINTERFACE_H
#define ACCESSIBLEFACTORYINTERFACE_H
class AccessibleFactoryBase;

class AccessibleFactoryInterface
{
public:
    explicit AccessibleFactoryInterface();
    virtual ~AccessibleFactoryInterface();
    virtual AccessibleFactoryBase * registerAccessibleFactory(const char *factoryName, AccessibleFactoryBase *factory) = 0;
    static void RegisterInstance(AccessibleFactoryInterface *inter);
};
#endif // ACCESSIBLEFACTORYINTERFACE_H
