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
#include "window/namespace.h"

#include "window/interface/moduleinterface.h"
#include "window/interface/frameproxyinterface.h"

#include <QObject>

namespace DCC_NAMESPACE {
namespace wacom {
class WacomWorker;
class WacomModel;
class WacomWidget;

class WacomModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit WacomModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    virtual ~WacomModule() override;

    virtual void preInitialize() override;
    virtual void initialize() override;
    virtual void active() override;
    virtual void deactive() override;
    virtual const QString name() const override;

private:
    WacomWidget *m_wacomWidget;
    WacomModel  *m_model;
    WacomWorker *m_worker;
};
}
}

#endif // WACOMMODULE_H_V20

