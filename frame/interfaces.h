#ifndef INTERFACES_H
#define INTERFACES_H

#include <QFrame>

class ControlCenterProxy;
class ModuleInterface
{
public:
    virtual ~ModuleInterface() {}
    virtual QFrame *getContent() = 0;

public slots:
    // plugin can remove there own data on this function(remove popup window for example).
    // TODO
    // preUnload should be executed before every unload operations.
    virtual void preUnload() {}
    virtual inline void setProxy(ControlCenterProxy *proxy) {m_controlCenterProxy = proxy;}

protected:
    ControlCenterProxy *m_controlCenterProxy;
};


QT_BEGIN_NAMESPACE

#define ModuleInterface_iid "org.deepin.ControlCenter.ModuleInterface"

Q_DECLARE_INTERFACE(ModuleInterface, ModuleInterface_iid)
QT_END_NAMESPACE

#endif
