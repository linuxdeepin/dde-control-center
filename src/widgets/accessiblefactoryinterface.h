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
#ifndef ACCESSIBLEFACTORYINTERFACE_H
#define ACCESSIBLEFACTORYINTERFACE_H
class AccessibleFactoryBase;

class AccessibleFactoryInterface
{
public:
    explicit AccessibleFactoryInterface();
    virtual ~AccessibleFactoryInterface();
    virtual AccessibleFactoryBase *registerAccessibleFactory(const char *factoryName,
                                                             AccessibleFactoryBase *factory) = 0;
    static void RegisterInstance(AccessibleFactoryInterface *inter);
};
#endif // ACCESSIBLEFACTORYINTERFACE_H
