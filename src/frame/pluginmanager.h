/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#pragma once

#include "interface/namespace.h"

#include <QObject>

class QPluginLoader;

DCC_BEGIN_NAMESPACE
class ModuleObject;
class PluginInterface;
class LayoutFactoryBase;
class LayoutManager;
DCC_END_NAMESPACE

struct PluginData
{
    QString Follow;
    int Location;
    DCC_NAMESPACE::ModuleObject *Module;
    QList<DCC_NAMESPACE::LayoutFactoryBase *> layoutFactory;
};

Q_DECLARE_METATYPE(PluginData)

namespace DCC_NAMESPACE
{
class PluginManager : public QObject
{
    Q_OBJECT
public:
    explicit PluginManager(QObject *parent = nullptr);
    void loadModules(ModuleObject *root, LayoutManager *layoutManager);

Q_SIGNALS:
    void loadedModule(const PluginData &data);

private:
    ModuleObject *findModule(ModuleObject *module, const QString &name);
    void initModules(const PluginData &data);

    QList<PluginData> m_datas;      //cache for other plugin
    ModuleObject *m_rootModule;     //root module from MainWindow
    LayoutManager *m_layoutManager; //layoutManager from MainWindow
};

} // namespace DCC_NAMESPACE
