#ifndef MOUSEWORKER_H
#define MOUSEWORKER_H

#include "mousemodel.h"


#include <com_deepin_daemon_inputdevice_mouse.h>
#include <com_deepin_daemon_inputdevice_touchpad.h>
#include <com_deepin_daemon_inputdevice_trackpoint.h>

#include "model/mousemodelbase.h"
#include "model/mousemodelbasesettings.h"
#include "model/mousemodelmousesettings.h"
#include "model/mousemodelthinkpadsettings.h"

#include <QObject>

using com::deepin::daemon::inputdevice::Mouse;
using com::deepin::daemon::inputdevice::TouchPad;
using com::deepin::daemon::inputdevice::TrackPoint;

namespace dcc {
namespace mouse {
class MouseWorker : public QObject
{
    Q_OBJECT
public:
    explicit MouseWorker(MouseModel *model, QObject *parent = 0);
    void active();
    void deactive();
    void init();

public slots:
    void setLeftHandState(const bool state);
    void setMouseNaturalScrollState(const bool state);
    void setTouchNaturalScrollState(const bool state);
    void setDisTyping(const bool state);
    void setDisTouchPad(const bool state);
    void setTapClick(const bool state);
    void setDouClick(const int &value);
    void setMouseMotionAcceleration(const double &value);
    void setTouchpadMotionAcceleration(const double &value);
    void setTrackPointMotionAcceleration(const double &value);
    void onDefaultReset();
    void onLeftHandStateChanged(const bool state);
    void onMouseNaturalScrollStateChanged(const bool state);
    void onTouchNaturalScrollStateChanged(const bool state);
    void onDisTypingChanged(const bool state);
    void onDisTouchPadChanged(const bool state);
    void onTapClick(const bool state);
    void onDouClickChanged(const int &value);
    void onMouseMotionAccelerationChanged(const int &value);
    void onTouchpadMotionAccelerationChanged(const int &value);
    void onTrackPointMotionAccelerationChanged(const int &value);

private:
    int converToDouble(int value);
    int converToDoubleModel(int value);
    double converToMotionAcceleration(int value);
    int converToModelMotionAcceleration(double value);

private:
    Mouse      *m_dbusMouse;
    TouchPad   *m_dbusTouchPad;
    TrackPoint *m_dbusTrackPoint;
    MouseModel *m_model;
};
}
}



#endif // MOUSEWORKER_H
