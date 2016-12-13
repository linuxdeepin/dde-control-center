#ifndef WACOMMODULE_H
#define WACOMMODULE_H

#include "moduleinterface.h"
namespace dcc {
class ContentWidget;
namespace wacom {
class WacomWidget;
class WacomModel;
class WacomWorker;
class WacomModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit WacomModule(FrameProxyInterface *frame, QObject *parent = 0);

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();

    ModuleWidget *moduleWidget();

    const QString name() const;

private:
    ~WacomModule();

    void contentPopped(ContentWidget *const w);

private:
    WacomWidget *m_wacomWidget;
    WacomModel  *m_model;
    WacomWorker *m_worker;
};
}
}

#endif // WACOMMODULE_H
