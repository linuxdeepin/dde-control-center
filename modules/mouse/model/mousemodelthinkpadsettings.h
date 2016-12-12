#ifndef MOUSEMODELTHINKPADSETTINGS_H
#define MOUSEMODELTHINKPADSETTINGS_H
#include <QObject>
#include "mousemodelbase.h"
namespace dcc
{
namespace mouse
{
class MouseModelThinkpadSettings : public MouseModelBase
{
    Q_OBJECT
public:
    explicit MouseModelThinkpadSettings(QObject *parent = 0);
};
}
}

#endif // MOUSEMODELTHINKPADSETTINGS_H
