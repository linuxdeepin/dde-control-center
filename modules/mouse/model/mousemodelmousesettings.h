#ifndef MOUSEMODELMOUSESETTINGS_H
#define MOUSEMODELMOUSESETTINGS_H
#include "mousemodelbase.h"
#include <QObject>
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

#endif // MOUSEMODELMOUSESETTINGS_H
