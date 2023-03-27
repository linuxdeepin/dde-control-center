// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef PRIVACYMODULE_H
#define PRIVACYMODULE_H

#include "interface/namespace.h"
#include "interface/moduleinterface.h"
#include "interface/frameproxyinterface.h"
#include "modules/privacy/applicationitem.h"

#include <QObject>

class PrivacySecurityWorker;
class PrivacySecurityModel;
class PermissionListWidget;

class PrivacyModule : public QObject, public DCC_NAMESPACE::ModuleInterface
{
    Q_OBJECT
    //    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Privacy" FILE "PrivacyModule.json")
    //    Q_INTERFACES(DCC_NAMESPACE::ModuleInterface)
public:
    explicit PrivacyModule(DCC_NAMESPACE::FrameProxyInterface *frameProxy, QObject *parent = nullptr);
    ~PrivacyModule() override;

    const QString name() const override;
    const QString displayName() const override;

    void preInitialize(bool sync = false, DCC_NAMESPACE::FrameProxyInterface::PushType type = DCC_NAMESPACE::FrameProxyInterface::PushType::Normal) override;
    void initialize() override;
    void active() override;
    void deactive() override;

    void showPage(const QString &pageName) override;
    QWidget *moduleWidget() override;
    int load(const QString &path) override;

    QString path() const override;
    QString follow() const override;

    QStringList availPage() const override;
    void addChildPageTrans() const override;
public Q_SLOTS:
    void onShowPage(int id);

private:
    void initSearchData() override;

    void activePage(int id);

private:
    PrivacySecurityModel *m_model = nullptr;
    PrivacySecurityWorker *m_worker = nullptr;
    QPointer<PermissionListWidget> m_listWidget;
    QList<PrivacyPage> m_privacyPages;
};

#endif // PRIVACYMODULE_H
