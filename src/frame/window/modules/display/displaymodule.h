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

#ifndef DISPLAYMODULE_H_V20
#define DISPLAYMODULE_H_V20

#include "interface/moduleinterface.h"
#include "interface/namespace.h"
#include "customsettingdialog.h"
#include "../../mainwindow.h"

using namespace dcc::display;
using namespace DCC_NAMESPACE::display;

class Resolution;

namespace dcc {

namespace display {
class DisplayModel;
class Monitor;
class DisplayWorker;
}

}

namespace DCC_NAMESPACE {

namespace display {

class DisplayWidget;

class DisplayModule : public QObject, public DCC_NAMESPACE::ModuleInterface
{
    Q_OBJECT
public:
    explicit DisplayModule(DCC_NAMESPACE::FrameProxyInterface *frame, QObject *parent = 0);
    ~DisplayModule();
    void windowUpdate();

public:
    void initialize() override;
    const QString name() const override;
    const QString displayName() const override;
    void active() override;
    int load(const QString &path) override;
    void preInitialize(bool sync = false) override;
    QStringList availPage() const override;

private Q_SLOTS:
    void showScalingPage();
    void showResolutionDetailPage();
    void onDetailPageRequestSetResolution(dcc::display::Monitor *mon, const int mode);
    void onCustomPageRequestSetResolution(dcc::display::Monitor *mon, CustomSettingDialog::ResolutionDate mode);
    void showBrightnessPage();
    void showRotate(dcc::display::Monitor *mon = nullptr);
    void showMultiScreenSettingPage();
    void showCustomSettingDialog();
    void showRefreshRotePage();
    void showTouchScreenPage();
    int showTimeoutDialog(dcc::display::Monitor *mon);
    void showRecognize();
    void updateGeometry(dcc::display::Monitor *mon);

private:
    dcc::display::DisplayModel *m_displayModel{nullptr};
    dcc::display::DisplayWorker *m_displayWorker{nullptr};
    DCC_NAMESPACE::display::DisplayWidget *m_displayWidget{nullptr};
    MainWindow *m_pMainWindow = nullptr;
    //由于切换合并和拆分模式会比较耗费资源，所以这边做延迟操作，防止一直进行切换的操作
    QTimer *m_joinTimer;
    QTimer *m_splitTimer;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYMODULE_H_V20