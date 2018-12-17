#ifndef SYNCMODULE_H
#define SYNCMODULE_H

#include "interfaces/moduleinterface.h"

#include <QObject>

namespace dcc {
namespace sync {
class SyncWidget;
class SyncModule : public QObject, public ModuleInterface {
    Q_OBJECT
public:
    explicit SyncModule(FrameProxyInterface * frame, QObject *parent = nullptr);
    ~SyncModule();

    void initialize() override;
    const QString name() const override;
    void moduleActive() override;
    void moduleDeactive() override;
    void reset() override;
    ModuleWidget *moduleWidget() override;
    void contentPopped(ContentWidget * const w) override;

Q_SIGNALS:

public Q_SLOTS:

private:
    SyncWidget *m_moduleWidget;
};
}  // namespace sync
}  // namespace dcc

#endif  // SYNCMODULE_H
