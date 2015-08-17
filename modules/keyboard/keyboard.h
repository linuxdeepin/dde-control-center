#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QObject>
#include <QtPlugin>
#include "interfaces.h"

class QFrame;
class Keyboard: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "keyboard.json")
    Q_INTERFACES(ModuleInterface)

public:
    Keyboard();
    ~Keyboard() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

private:
    QFrame * m_frame;

    void initBackend();
    void initUI();
};

#endif //   KEYBOARD_H
