#ifndef INTERFACES_H
#define INTERFACES_H

#include <QFrame>

class ModuleInterface
{
public:
    virtual ~ModuleInterface() {}
    virtual QFrame *getContent() = 0;
};


QT_BEGIN_NAMESPACE

#define ModuleInterface_iid "org.deepin.ControlCenter.ModuleInterface"

Q_DECLARE_INTERFACE(ModuleInterface, ModuleInterface_iid)
QT_END_NAMESPACE

#endif
