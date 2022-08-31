// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "interface/moduleinterface.h"

QT_BEGIN_NAMESPACE
#include <QObject>
QT_END_NAMESPACE

namespace dcc {
namespace systeminfo {
class SystemInfoModel;
class SystemInfoWork;
}
}

namespace DCC_NAMESPACE {
class MainWindow;
namespace systeminfo {
class SystemInfoWidget;
class SystemInfoModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    enum SystemType {
        Default = -1,
        AboutThisPC,
        EditionLicense,
        EndUserLicenseAgreement,
        PrivacyPolicy,
        MaxType
    };

public:
    explicit SystemInfoModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    ~SystemInfoModule();

    virtual void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    void initialize() override;
    void reset() override;
    const QString name() const override;
    const QString displayName() const override;
    void contentPopped(QWidget *const w) override;
    void active() override;
    int load(const QString &path) override;
    QStringList availPage() const override;
    void addChildPageTrans() const override;

private:
    void initSearchData() override;

public Q_SLOTS:
    void onShowAboutNativePage();
    void onVersionProtocolPage();
    void onShowEndUserLicenseAgreementPage();
    void onShowPrivacyPolicyPage();

private:
    dcc::systeminfo::SystemInfoWork *m_work;
    dcc::systeminfo::SystemInfoModel *m_model;
    SystemInfoWidget *m_sysinfoWidget;
    MainWindow *m_pMainWindow;
};

}
}
