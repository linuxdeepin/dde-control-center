// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef WACOMMODULE_H_V20
#define WACOMMODULE_H_V20
#include "interface/namespace.h"

#include "interface/moduleinterface.h"
#include "interface/frameproxyinterface.h"

#include <QObject>

namespace DCC_NAMESPACE {
namespace wacom {
class WacomWorker;
class WacomModel;
class WacomWidget;

class WacomModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit WacomModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    virtual ~WacomModule() override;

    virtual void preInitialize(bool sync = false , FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    virtual void initialize() override;
    virtual void active() override;
    virtual void deactive() override;
    virtual const QString name() const override;
    virtual const QString displayName() const override;
    QStringList availPage() const override;
    virtual void addChildPageTrans() const override;

private:
    void initSearchData() override;

private:
    WacomWidget *m_wacomWidget;
    WacomModel  *m_model;
    WacomWorker *m_worker;
};
}
}

#endif // WACOMMODULE_H_V20

