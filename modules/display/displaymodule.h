#ifndef DISPLAYMODULE_H
#define DISPLAYMODULE_H

#include "moduleinterface.h"
#include "modulewidget.h"
#include "displaywidget.h"
#include "displayworker.h"

namespace dcc {

namespace display {

class DisplayModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit DisplayModule(FrameProxyInterface *frame, QObject *parent = 0);

private:
    void initialize();
    void reset();
    void moduleActive();
    void moduleDeactive();
    void contentPopped(ContentWidget * const w);
    const QString name() const;
    ModuleWidget *moduleWidget();

private:
    DisplayWorker *m_displayWorker;
    DisplayWidget *m_displayWidget;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYMODULE_H
