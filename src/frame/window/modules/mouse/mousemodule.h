/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
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

#include "interface/namespace.h"
#include "interface/moduleinterface.h"

#include <QObject>

namespace dcc {
namespace mouse {
class MouseModel;
class MouseWorker;
class MouseDBusProxy;
}
}

namespace DCC_NAMESPACE {
class MainWindow;
namespace mouse {
class MouseWidget;
class GeneralSettingWidget;
class MouseSettingWidget;
class TouchPadSettingWidget;
class TrackPointSettingWidget;

class MouseModule : public QObject, public ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ModuleInterface_iid)
    Q_INTERFACES(DCC_NAMESPACE::ModuleInterface)

public:
    explicit MouseModule() = default;
    explicit MouseModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    void initialize() override;
    void reset() override;
    void active() override;
    const QString name() const override;
    const QString displayName() const override;
    void contentPopped(QWidget *const w) override;
    int load(const QString &path) override;
    QStringList availPage() const override;

private:
    void initSearchData();

public Q_SLOTS:
    void showGeneralSetting();
    void showMouseSetting();
    void showTouchpadSetting();
    void showTrackPointSetting();

private:
    MouseWidget *m_mouseWidget = nullptr;
    GeneralSettingWidget *m_generalSettingWidget = nullptr;
    MouseSettingWidget *m_mouseSettingWidget = nullptr;
    TouchPadSettingWidget *m_touchpadSettingWidget = nullptr;
    TrackPointSettingWidget *m_trackPointSettingWidget = nullptr;
    dcc::mouse::MouseModel  *m_model = nullptr;
    dcc::mouse::MouseWorker *m_worker = nullptr;
    dcc::mouse::MouseDBusProxy *m_dbusProxy = nullptr;
    MainWindow *m_pMainWindow;
};
}
}
