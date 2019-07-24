/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
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

#include "window/namespace.h"
#include "window/interface/moduleinterface.h"
#include <QObject>

namespace dcc
{
namespace mouse
{
class MouseModel;
class MouseWorker;
}
}

namespace DCC_NAMESPACE
{
namespace mouse
{
class MouseWidget;
class GeneralSettingWidget;
class MouseSettingWidget;
class TouchPadSettingWidget;
class PalmDetectSettingWidget;

class MouseModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit MouseModule(FrameProxyInterface *frame, QObject *parent = 0);

    void initialize() override;
    void reset() override;
    QWidget *moduleWidget() override;
    const QString name() const override;

private:
    ~MouseModule();
    void contentPopped(QWidget *const w) override;

public Q_SLOTS:
    void showGeneralSetting();
    void showMouseSetting();
    void showTouchpadSetting();
    void showPalmdetectSetting();
private:

    MouseWidget *m_mouseWidget;
    GeneralSettingWidget *m_generalSettingWidget;
    MouseSettingWidget *m_mouseSettingWidget;
    TouchPadSettingWidget *m_touchpadSettingWidget;
    PalmDetectSettingWidget *m_palmdetectSettingWidget;
    dcc::mouse::MouseModel  *m_model;
    dcc::mouse::MouseWorker *m_worker;
};
}
}
