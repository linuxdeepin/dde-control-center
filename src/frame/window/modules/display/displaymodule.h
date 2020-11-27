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
#include "modules/display/recognizedialog.h"

#include <QSharedPointer>
#include <QThread>

using namespace dcc::display;

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

class DisplayModule : public QObject, public ModuleInterface
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
    void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
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
    void showDisplayRecognize();
    void showTouchRecognize();
    void showMultiResolutionPage();
    void showMultiRefreshRatePage();

private:
    dcc::display::DisplayModel *m_displayModel{nullptr};
    QSharedPointer<dcc::display::DisplayWorker> m_displayWorker;
    QSharedPointer<QThread> m_workThread;
    DisplayWidget *m_displayWidget{nullptr};
    MainWindow *m_pMainWindow = nullptr;
    QMap<QString, RecognizeDialog*> m_recognizeDialg;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYMODULE_H_V20
