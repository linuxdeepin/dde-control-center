// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/moduleinterface.h"
#include "window/dconfigwatcher.h"

#include <QGSettings>
#include <QObject>

#include <types/zoneinfo.h>

namespace dcc {
namespace power {
class PowerWorker;
class PowerModel;
} // namespace power
} // namespace dcc

namespace DCC_NAMESPACE {
class MainWindow;
namespace power {

class PowerWidget;

class PowerModule : public QObject
    , public ModuleInterface
{
    Q_OBJECT
public:
    enum powerType {
        DEFAULT = -1,
        GENERAL,
        USE_ELECTRIC,
        USE_BATTERY,
        COUNT
    };

    enum powerServerType {
        SERVER_DEFAULT = -1,
        SERVER_USE_ELECTRIC,
        SERVER_USE_BATTERY,
        SERVER_COUNT
    };

    PowerModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);

    virtual void preInitialize(bool sync = false, FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

Q_SIGNALS:

public Q_SLOTS:
    void onBatteryPercentageChanged(const double value);

private:
    void showGeneral();
    void showUseElectric();
    void showUseBattery();
    bool getDoubleCompare(const double value1, const double value2);
    void initSearchData() override;

private:
    dcc::power::PowerModel *m_model;
    dcc::power::PowerWorker *m_work;
    QTimer *m_timer;
    QPointer<PowerWidget> m_widget;
    double m_nBatteryPercentage;
    QGSettings *m_powerSetting;
    MainWindow *m_pMainWindow;
    DConfig *m_dconfig;
};

} // namespace power
} // namespace DCC_NAMESPACE
