/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wp <wangpeng_cm@deepin.com>
 *
 * Maintainer: wp <wangpeng_cm@deepin.com>
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

#ifndef WACOMMODULE_H_V20
#define WACOMMODULE_H_V20

#include "window/interface/moduleinterface.h"
#include "window/interface/frameproxyinterface.h"
#include "window/namespace.h"

namespace dcc {
namespace wacom {
class WacomWorker;
class WacomModel;
}
}
class ComboBoxWidget;
namespace DCC_NAMESPACE {
namespace wacom {
class WacomWidget;
class WacomModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit WacomModule(FrameProxyInterface *frame, QObject *parent = 0);
    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    void preInitialize();
    QWidget *moduleWidget();
    const QString name() const;

private Q_SLOTS:
    void showModePage();

private:
    ~WacomModule();
    void contentPopped(QWidget *const w);

private:
    WacomWidget *m_wacomWidget;
    dcc::wacom::WacomModel  *m_model;
    dcc::wacom::WacomWorker *m_worker;
};
}
}

#endif // WACOMMODULE_H_V20

