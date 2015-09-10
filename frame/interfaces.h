#ifndef INTERFACES_H
#define INTERFACES_H

#include <QFrame>

class ModuleInterface
{
public:
    virtual ~ModuleInterface() {}
    virtual QFrame *getContent() = 0;

public slots:
    // plugin can remove there own data on this function(remove popup window for example).
    virtual void preUnload() {}
};


QT_BEGIN_NAMESPACE

#define ModuleInterface_iid "org.deepin.ControlCenter.ModuleInterface"

Q_DECLARE_INTERFACE(ModuleInterface, ModuleInterface_iid)
QT_END_NAMESPACE

#endif
