#ifndef MOUSEMODELTHINKPADSETTINGS_H
#define MOUSEMODELTHINKPADSETTINGS_H
#include "mousemodelbase.h"
#include <QObject>
class MouseModelThinkpadSettings : public MouseModelBase
{
    Q_OBJECT
public:
    explicit MouseModelThinkpadSettings(QObject *parent = 0);
};

#endif // MOUSEMODELTHINKPADSETTINGS_H
