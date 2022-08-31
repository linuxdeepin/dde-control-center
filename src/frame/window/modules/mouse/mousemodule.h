// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    void addChildPageTrans() const override;

private:
    void initSearchData() override;

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
