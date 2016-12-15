#ifndef MOUSEMODULE_H
#define MOUSEMODULE_H

#include "moduleinterface.h"
#include <QObject>

namespace dcc
{
namespace mouse
{

class MouseWidget;
class MouseModel;
class MouseWorker;
class MouseModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit MouseModule(FrameProxyInterface *frame, QObject *parent = 0);

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();

    ModuleWidget *moduleWidget();

    const QString name() const;

private:
    ~MouseModule();

    void contentPopped(ContentWidget *const w);

private:

    MouseWidget *m_mouseWidget;
    MouseModel  *m_model;
    MouseWorker *m_worker;
};
}
}


#endif // MOUSEMODULE_H
