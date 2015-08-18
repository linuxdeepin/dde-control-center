#ifndef DATETIME_H
#define DATETIME_H

#include <QObject>
#include <QtPlugin>
#include "interfaces.h"

class QFrame;
class Datetime: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "datetime.json")
    Q_INTERFACES(ModuleInterface)

public:
    Datetime();
    ~Datetime() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

private:
    QFrame * m_frame;

    void initBackend();
    void initUI();
};

#endif //DATETIME_H
