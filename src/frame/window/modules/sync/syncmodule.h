// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/moduleinterface.h"

#include <QObject>

namespace dcc {
namespace cloudsync {
class SyncWorker;
class SyncModel;
}
}

namespace DCC_NAMESPACE {
namespace sync {
class SyncModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    SyncModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);

    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void contentPopped(QWidget *const w) override;
    virtual void active() override;
    virtual void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

private:
    void initSearchData() override;

private:
    dcc::cloudsync::SyncModel  *m_model;
    dcc::cloudsync::SyncWorker *m_worker;
};
} // namespace sync
} // namespace DCC_NAMESPACE
