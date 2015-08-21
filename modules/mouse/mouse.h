#ifndef MOUSE_H
#define MOUSE_H

#include <QObject>
#include <QtPlugin>
#include <QLabel>

#include <libdui/dtextbutton.h>
#include <libdui/dheaderline.h>
#include <libdui/dbaseline.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dslider.h>
#include <libdui/dsegmentedcontrol.h>
#include <libdui/dswitchbutton.h>

#include "interfaces.h"
#include "dbus/dbusmouse.h"
#include "dbus/dbustouchpad.h"

DUI_USE_NAMESPACE

class ContainerWidget;
class ModuleHeader;
class QLabel;
class QFrame;
class Mouse: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "mouse.json")
    Q_INTERFACES(ModuleInterface)

public:
    Mouse();
    ~Mouse() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

public slots:
    void setMousePrimaryButton(int index);
    void setMousePointSpeed(int speed);
    void setMouseDoubleClickInterval(int interval);
    void disableTouchpadWhenMousePluggedIn(bool flag);

    void enableTouchpad(bool flag);
    void setTouchpadPointSpeed(int speed);
    void setTouchpadDoubleClickInterval(int interval);
    void enableTouchpadNatureScroll(bool flag);
    void enableTouchpadTapToClick(bool flag);
    void enableTouchpadTwoFingerScroll(bool flag);
    void enableTouchpadEdgeScroll(bool flag);
    void onTouchPadExistChanged();
    void reset();
private:
    QLabel * m_label;

    ComDeepinDaemonInputDeviceMouseInterface * m_mouseInterface;
    ComDeepinDaemonInputDeviceTouchPadInterface * m_touchpadInterface;

    ModuleHeader * m_topHeaderLine;
    DSeparatorHorizontal * m_firstHSeparator;
    QWidget * m_mouseSettingPanel;
    QVBoxLayout * m_mouseSettingPanelLayout;
    QWidget * m_primaryButtonContainer;
    QHBoxLayout * m_primaryButtonContainerLayout;
    QLabel * m_primaryButtonSetting;
    DSegmentedControl * m_mousePrimaryButtonSetting;
    QWidget * m_pointSpeedContainer;
    QHBoxLayout * m_pointSpeedContainerLayout;
    QLabel * m_pointSpeedLabel;
    DSlider * m_mousePointSpeedSlider;
    QWidget * m_doubleClickSpeedContainer;
    QHBoxLayout * m_doubleClickContainerLayout;
    QLabel * m_doubleClickSpeedLabel;
    DSlider * m_mouseDoubleClickIntervalSlider;
    QWidget * m_forbiddenTouchpadWhenMouseContainer;
    QHBoxLayout * m_forbiddenTouchpadWhenMouseLayout;
    QLabel * m_forbiddenTouchpadWhenMouseLabel;
    DSwitchButton * m_forbiddenTouchpadWhenMouseSwitchButton;
    DSeparatorHorizontal * m_secondHSeparator;
    DHeaderLine * m_touchpadHeaderLine;
    DSwitchButton * m_touchpadSwitchButton;
    DSeparatorHorizontal * m_thirdHSeparator;
    ContainerWidget * m_touchpadSettingPanel;
    DSegmentedControl * m_touchpadPrimaryButtonSetting;
    DSlider * m_touchpadPointSpeedSlider;
    DSlider * m_touchpadDoubleClickSpeed;
    DSlider * m_touchpadDragThreshold;
    DSwitchButton * m_touchpadNatureScrollSwitch;
    DSwitchButton * m_touchpadTapToClickSwitch;
    DSwitchButton * m_touchpadTwoFingerScrollSwitch;
    DSwitchButton * m_touchpadEdgeScrollSwitch ;
    DSeparatorHorizontal * m_fourthHSeparator;
    double m_mousePointSpeed;
    double m_touchpadPointSpeed;

    void setWidgetsValue();
};


class ContainerWidget: public QWidget
{
    Q_OBJECT
public:
    ContainerWidget(QWidget * parent = 0);
    ~ContainerWidget();

    void addRow(const QString & text, QWidget * widget);
    void addRow(const QString & test, int stretch, QWidget * widget, int spacing = 5);

private:
    int m_labelWidth;
    int m_rowHeight;
};

#endif // MOUSE_H
