#ifndef POWERMODULE_H
#define POWERMODULE_H

#include "moduleinterface.h"
#include "powerwidget.h"
#include "powerworker.h"
#include "powermodel.h"

#include <QObject>

class PowerModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit PowerModule(FrameProxyInterface *frame, QObject *parent = 0);

private:
    void initialize();
    void moduleActive();
    void moduleDeactive();
    void contentPopped(ContentWidget * const w);
    ModuleWidget *moduleWidget();
    const QString name() const;

private:
    PowerModel *m_powerModel;
    PowerWidget *m_powerWidget;
    PowerWorker *m_powerWorker;
};

#endif // POWERMODULE_H
