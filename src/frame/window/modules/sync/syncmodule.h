#pragma once

#include "window/interface/moduleinterface.h"

#include <QObject>

namespace dcc {
class ContentWidget;
class SyncWorker;
class SyncModel;
}

namespace DCC_NAMESPACE {
namespace sync {
class SyncModule : public QObject, public ModuleInterface {
    Q_OBJECT
public:
    SyncModule(dcc::FrameProxyInterface *frameProxy, QObject *parent = nullptr);

    virtual void initialize() override;
    virtual void reset() override;
    virtual const QString name() const override;
    virtual void showPage(const QString &pageName) override;
    virtual QWidget *moduleWidget() override;
    virtual void contentPopped(QWidget *const w) override;

private:
    dcc::ContentWidget* mainWidget;
    dcc::SyncWorker* m_worker;
    dcc::SyncModel* m_model;
};
} // namespace sync
} // namespace DCC_NAMESPACE
