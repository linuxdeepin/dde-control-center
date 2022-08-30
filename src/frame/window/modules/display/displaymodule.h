// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    void addChildPageTrans() const override;

private Q_SLOTS:
    void onRequestSetResolution(dcc::display::Monitor *monitor, const int mode);
    void onRequestSetRotate(dcc::display::Monitor *monitor, const int rotate);
    void onRequestSetFillMode(dcc::display::Monitor *monitor, const QString fillMode);
    void showSingleScreenWidget();
    void showMultiScreenWidget();
    void pushScreenWidget();
    int showTimeoutDialog(dcc::display::Monitor *monitor, const bool isFillMode = false);
    void onSetFillMode(QString currFullMode = "");
    void initSearchData() override;
    void showDisplayRecognize();

private:
    dcc::display::DisplayModel *m_displayModel;
    dcc::display::DisplayWorker *m_displayWorker;
    DisplayWidget *m_displayWidget;
    MainWindow *m_pMainWindow;
    QMap<QString, RecognizeWidget *> m_recognizeWidget;
};

} // namespace display

} // namespace DCC_NAMESPACE

#endif // DISPLAYMODULE_H
