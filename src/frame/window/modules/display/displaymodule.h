/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
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

#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include "interface/moduleinterface.h"
#include "interface/namespace.h"
#include "../../mainwindow.h"
#include "modules/display/recognizewidget.h"

using namespace dcc::display;

class Resolution;

namespace dcc {
namespace display {
class DisplayModel;
class Monitor;
class DisplayWorker;
} // namespace display
} // namespace dcc

namespace DCC_NAMESPACE {

namespace display {

class DisplayWidget;

class DisplayModule : public QObject
    , public ModuleInterface
{
    Q_OBJECT
    Q_INTERFACES(DCC_NAMESPACE::ModuleInterface)
public:
    explicit DisplayModule(DCC_NAMESPACE::FrameProxyInterface *frame, QObject *parent = nullptr);
    ~DisplayModule() override;
    void windowUpdate();

public:
    void initialize() override;
    const QString name() const override;
    const QString displayName() const override;
    void active() override;
    int load(const QString &path) override;
    void preInitialize(bool sync = false, FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    QStringList availPage() const override;

private Q_SLOTS:
    void onRequestSetResolution(dcc::display::Monitor *monitor, const int mode);
    void onRequestSetRotate(dcc::display::Monitor *monitor, const int rotate);
    void showSingleScreenWidget();
    void showMultiScreenWidget();
    void pushScreenWidget();
    int showTimeoutDialog(dcc::display::Monitor *monitor);

private:
    dcc::display::DisplayModel *m_displayModel;
    dcc::display::DisplayWorker *m_displayWorker;
    DisplayWidget *m_displayWidget;
    MainWindow *m_pMainWindow;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // DISPLAYMODULE_H
