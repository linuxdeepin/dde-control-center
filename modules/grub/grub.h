#ifndef GRUB_H
#define GRUB_H

#include <QObject>
#include <QtPlugin>

#include "interfaces.h"

class QFrame;
class GrubWidget;
class Grub: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "grub.json")
    Q_INTERFACES(ModuleInterface)

public:
    Grub();
    ~Grub() Q_DECL_OVERRIDE;
    QFrame *getContent() Q_DECL_OVERRIDE;

private:
    QFrame *m_grubWidget;
};

#endif // GRUB

