// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef TOUCHSCREENMODULE_H
#define TOUCHSCREENMODULE_H

#include "interface/moduleinterface.h"
#include "interface/namespace.h"

namespace dcc {
namespace display {
class TouchscreenModel;
class TouchscreenWorker;
class RecognizeWidget;
} // namespace display
} // namespace dcc

namespace DCC_NAMESPACE {

namespace touchscreen {

class TouchscreenWidget;

class TouchscreenModule : public QObject
    , public ModuleInterface
{
    Q_OBJECT
    Q_INTERFACES(DCC_NAMESPACE::ModuleInterface)
public:
    explicit TouchscreenModule(DCC_NAMESPACE::FrameProxyInterface *frame, QObject *parent = nullptr);
    ~TouchscreenModule() override;

    virtual void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void initialize() override;
    virtual void active() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

private Q_SLOTS:
    void showTouchRecognize();

private:
    void initSearchData() override;

private:
    dcc::display::TouchscreenModel *m_touchscreenModel;
    dcc::display::TouchscreenWorker *m_touchscreenWorker;
    TouchscreenWidget *m_touchscreenWidget;
    QMap<QString, dcc::display::RecognizeWidget *> m_recognizeWidget;
};

} // namespace touchscreen

} // namespace DCC_NAMESPACE

#endif // TOUCHSCREENMODULE_H
