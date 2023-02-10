#ifndef WACOMMODULE_H
#define WACOMMODULE_H

#include "interface/pagemodule.h"

#include <QObject>

class WacomModel;

class WacomModule : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT
public:
    explicit WacomModule(QObject *parent = nullptr);

    ~WacomModule() override { }

    QWidget *initModeModule(ModuleObject *module);
    QWidget *initPressureModule(ModuleObject *module);

private:
    WacomModel *m_model;
};

#endif // WACOMMODULE_H
