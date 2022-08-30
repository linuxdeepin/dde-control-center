// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/moduleinterface.h"
#include "modules/defapp/defappworker.h"

namespace dcc {
namespace defapp {
class DefAppModel;
class DefAppWorker;
}
}
namespace DCC_NAMESPACE {
class MainWindow;
namespace defapp {
class DefaultAppsModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit DefaultAppsModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    ~DefaultAppsModule();

    virtual void preInitialize(bool sync = false, FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual void contentPopped(QWidget *const w) override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

private:
    void initSearchData() override;

Q_SIGNALS:
    void requestSetDefappCategory(dcc::defapp::DefAppWorker::DefaultAppsCategory category);
    void requestAddDefault();

private Q_SLOTS:
    void showDetailWidget(dcc::defapp::DefAppWorker::DefaultAppsCategory category = dcc::defapp::DefAppWorker::Browser);

private:
    dcc::defapp::DefAppModel *m_defAppModel{nullptr};
    dcc::defapp::DefAppWorker *m_defAppWorker{nullptr};
    MainWindow *m_pMainWindow;
};
}
}
