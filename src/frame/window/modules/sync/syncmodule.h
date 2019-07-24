/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@outlook.com>
 *
 * Maintainer: justforlxz <justforlxz@outlook.com>
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
namespace cloudsync {
class SyncWorker;
class SyncModel;
}
}

namespace DCC_NAMESPACE {
namespace sync {
class SyncWidget;
class SyncModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    SyncModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);

    virtual void initialize() override;
    virtual void reset() override;
    virtual const QString name() const override;
    virtual void showPage(const QString &pageName) override;
    virtual QWidget *moduleWidget() override;
    virtual void contentPopped(QWidget *const w) override;

private:
    SyncWidget                 *m_mainWidget;
    dcc::cloudsync::SyncModel  *m_model;
    dcc::cloudsync::SyncWorker *m_worker;
};
} // namespace sync
} // namespace DCC_NAMESPACE
