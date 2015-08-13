#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <QtPlugin>
#include "interfaces.h"

class QFrame;
class Display: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "display.json")
    Q_INTERFACES(ModuleInterface)

public:
    Display();
    ~Display() Q_DECL_OVERRIDE;
    QFrame *getContent() Q_DECL_OVERRIDE;

private:
    QFrame * m_frame;
};

#endif // DISPLAY_H

