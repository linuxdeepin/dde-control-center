// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/moduleinterface.h"
#include "modules/update/common.h"

#include <QObject>
#include <QGSettings>
#include <QSharedPointer>
#include <QThread>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {
namespace update {
class UpdateWorker;
class UpdateModel;
class MirrorsWidget;
}
}

namespace DCC_NAMESPACE {
namespace update {
class UpdateWidget;
class MirrorsWidget;

class UpdateModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    UpdateModule(FrameProxyInterface *frameProxy, QObject *parent = nullptr);
    ~UpdateModule() override;
    virtual void preInitialize(bool sync = false, FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void initialize() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    virtual void active() override;
    virtual void deactive() override;
    virtual int load(const QString &path) override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

private Q_SLOTS:
    void onNotifyDealMirrorWidget(bool state);

private:
    void initSearchData() override;

private:
    void notifyDisplayReminder(dcc::update::UpdatesStatus status);

    /*!
     * \brief onUpdatablePackagesChanged 根据是否有更新包,处理更新项小红点显示
     * \param isUpdatablePackages 当更新包数量大于0时为true
     */
    void onUpdatablePackagesChanged(const bool isUpdatablePackages);

private:
    dcc::update::UpdateModel *m_model;
    QSharedPointer<dcc::update::UpdateWorker> m_work;
    QPointer<UpdateWidget> m_updateWidget;
    MirrorsWidget *m_mirrorsWidget;
    QGSettings *m_versionTypeModue{nullptr};
    QStringList versionTypeList;
    QSharedPointer<QThread> m_workThread;
};

}// namespace datetime
}// namespace DCC_NAMESPACE
