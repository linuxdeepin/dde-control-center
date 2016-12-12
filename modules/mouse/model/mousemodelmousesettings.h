#ifndef MOUSEMODELMOUSESETTINGS_H
#define MOUSEMODELMOUSESETTINGS_H
#include <QObject>
#include "mousemodelbase.h"
namespace dcc
{
namespace mouse
{
class MouseModelMouseSettings : public MouseModelBase
{
    Q_OBJECT
public:
    explicit MouseModelMouseSettings(QObject *parent = 0);

    void setSwitchState(const bool state);

    inline bool getSwitchState() {return m_switchValue;}

signals:
    void switchChanged(const bool state);

private:
    bool m_switchValue;
};
}
}


#endif // MOUSEMODELMOUSESETTINGS_H
