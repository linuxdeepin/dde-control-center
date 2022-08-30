// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/moduleinterface.h"

#include <QObject>

namespace dcc {
namespace notification {
class NotificationWorker;
class NotificationModel;
}
}

namespace DCC_NAMESPACE {
class MainWindow;
namespace notification {

class NotificationWidget;

class NotificationModule: public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    NotificationModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);
    ~NotificationModule() override;

    virtual void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

private Q_SLOTS:
    void showSystemNotify();
    void showAppNotify(int index);

private:
    void initSearchData() override;

private:
    dcc::notification::NotificationModel *m_model;
    dcc::notification::NotificationWorker *m_worker;
    QPointer<NotificationWidget> m_widget;
    MainWindow * m_pMainWindow;
};

}// namespace msgnotify
}// namespace DCC_NAMESPACE
