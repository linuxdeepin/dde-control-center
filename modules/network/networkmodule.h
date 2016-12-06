#ifndef NETWORKMODULE_H
#define NETWORKMODULE_H

#include "moduleinterface.h"

class NetworkModuleWidget;

namespace dcc {
namespace network {

class NetworkModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit NetworkModule(FrameProxyInterface *frame, QObject *parent = 0);

private:
    void initialize();
    void reset();
    void moduleActive();
    void moduleDeactive();
    void contentPopped(ContentWidget * const w);
    const QString name() const;
    ModuleWidget *moduleWidget();

private:
    NetworkModuleWidget *m_networkWidget;
};

} // namespace network
} // namespace dcc

#endif // NETWORKMODULE_H
