/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef PLUGINSCONTROLLER_H
#define PLUGINSCONTROLLER_H

#include "interfaces/frameproxyinterface.h"

#include <QObject>

using namespace dcc;

class PluginsController : public QObject, public FrameProxyInterface
{
    Q_OBJECT

public:
    explicit PluginsController(QObject *parent = 0);

Q_SIGNALS:
    void pluginAdded(QWidget * const w);
    void requestModulePage(const QString &module, const QString &page, bool animation);

public Q_SLOTS:
    void loadPlugins();

private:
    void pushWidget(ModuleInterface * const, ContentWidget * const) {}
    void setFrameAutoHide(ModuleInterface * const, const bool) {}
    void setModuleVisible(ModuleInterface * const, const bool) {}
    void showModulePage(const QString &module, const QString &page, bool animation);
};

#endif // PLUGINSCONTROLLER_H
