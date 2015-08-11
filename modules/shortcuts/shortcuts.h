#ifndef SHORTCUTS_H
#define SHORTCUTS_H

#include <QObject>
#include <QtPlugin>

#include "interfaces.h"

class QFrame;
class MainWidget;
class Shortcuts: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "shortcuts.json")
    Q_INTERFACES(ModuleInterface)

public:
    Shortcuts();
    ~Shortcuts() Q_DECL_OVERRIDE;
    QFrame *getContent() Q_DECL_OVERRIDE;

private:
    QFrame *m_mainWidget;
};

#endif // SHORTCUTS_H

