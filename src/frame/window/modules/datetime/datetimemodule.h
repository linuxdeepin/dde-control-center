/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "window/interface/moduleinterface.h"

#include <QObject>

namespace dcc {
class DatetimeWork;
class DatetimeModel;
}

namespace DCC_NAMESPACE {
namespace datetime {
class DatetimeModule : public QObject, public ModuleInterface
{
//public:
//    DatetimeModule();

    Q_OBJECT
public:
    DatetimeModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);

    virtual void initialize() override;
    virtual void reset() override;
    virtual const QString name() const override;
    virtual void showPage(const QString &pageName) override;
    virtual QWidget *moduleWidget() override;
    virtual void contentPopped(QWidget *const w) override;

private:
    QWidget* m_mainWidget;
    dcc::DatetimeWork* m_work;
    dcc::DatetimeModel* m_model;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
