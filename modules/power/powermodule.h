#ifndef POWERMODULE_H
#define POWERMODULE_H

#include "moduleinterface.h"
#include "powerwidget.h"
#include "powerworker.h"
#include "powermodel.h"

#include <QObject>

class PowerModule : public QObject, public dcc::ModuleInterface
{
    Q_OBJECT

public:
    explicit PowerModule(dcc::FrameProxyInterface *frame, QObject *parent = 0);

private:
    ~PowerModule();

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    void contentPopped(dcc::ContentWidget * const w);
    dcc::ModuleWidget *moduleWidget();
    const QString name() const;


private:
    PowerModel *m_powerModel;
    PowerWidget *m_powerWidget;
    PowerWorker *m_powerWorker;
};

#endif // POWERMODULE_H
