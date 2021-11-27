/*
 * Copyright (C) 2011 ~ 2021 Uniontech Technology Co., Ltd.
 *
 * Author:     huangweihua <huangweihua@uniontech.com>
 *
 * Maintainer: huangweihua <huangweihua@uniontech.com>
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
