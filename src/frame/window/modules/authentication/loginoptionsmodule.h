// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/moduleinterface.h"
#include "../../mainwindow.h"
#include "addirisinfodialog.h"

namespace dcc {
namespace authentication {
class FingerModel;
class FingerWorker;
class CharaMangerModel;
class CharaMangerWorker;
}
}

namespace DCC_NAMESPACE {
namespace authentication {
class LoginOptionsWidget;
class LoginOptionsModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit LoginOptionsModule(FrameProxyInterface *frame, QObject *parent = nullptr);

    virtual void preInitialize(bool sync = false, FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    void initialize() override;
    void reset() override;
    const QString name() const override;
    const QString displayName() const override;
    void showPage(const QString &pageName) override;
    void contentPopped(QWidget *const w) override;
    void active() override;
    int load(const QString &path) override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

public Q_SLOTS:
    void showFingerPage();
    void showFaceidPage();
    void showIrisPage();
    void updateModuleVisible();

private:
    ~LoginOptionsModule();
    void onSetMainWindowEnabled(const bool isEnabled);
    void initSearchData() override;

private:
    MainWindow *m_pMainWindow = nullptr;
    LoginOptionsWidget *m_loginOptionsWidget = nullptr;
    dcc::authentication::FingerModel *m_fingerModel{nullptr};
    dcc::authentication::FingerWorker *m_fingerWorker{nullptr};
    dcc::authentication::CharaMangerModel *m_charaMangerModel{nullptr};
    dcc::authentication::CharaMangerWorker *m_charaMangerWorker{nullptr};
};

}   // namespace authentication
}   // namespace dccV20
