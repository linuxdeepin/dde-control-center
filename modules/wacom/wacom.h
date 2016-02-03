/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef WACOM_H
#define WACOM_H

#include <QObject>
#include <QtPlugin>
#include <QFrame>
#include <QLabel>

#include <libdui/dtextbutton.h>
#include <libdui/dslider.h>
#include <libdui/dsimplecombobox.h>
#include <libdui/dheaderline.h>
#include <libdui/dseparatorhorizontal.h>
#include <libdui/dsegmentedcontrol.h>

#include "moduleheader.h"
#include "interfaces.h"
#include "dbus/dbusinputwacom.h"

DUI_USE_NAMESPACE

class Wacom: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "wacom.json")
    Q_INTERFACES(ModuleInterface)

public:
    Wacom();
    ~Wacom() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

    void initUI();
    void initWacom();
    void initConnects();

public slots:
    void setTabletOrientation(int orientation);
    void setDigitalPenModes(int penmode);
    void setDownKey(int indexofdownkey);
    void setUpKey(int indexofupkey);
    void setPressure(int pressure);
    void setDoubleClickSpace(int space);

    void reset();

private:
    QFrame* m_frame;
    DBusInputWacom* m_wacomInterface;
    ModuleHeader* m_wacomModuleHeader;
    DSeparatorHorizontal* m_theHSeparator1;
    DSeparatorHorizontal* m_theHSeparator2;
    QWidget* wacomPanel;
    DHeaderLine* m_wacomHeaderLine;
    DSegmentedControl* m_tabletOriSCtrl;
    DSegmentedControl* m_digitalPenModesSCtrl;
    DSimpleComboBox* m_downKeySimpleComboBox;
    DSimpleComboBox* m_upKeySimpleComboBox;
    DSlider* m_pressureSlider;
    DSlider* m_doubleClickSpaceSlider;
};

#endif // WACOM_H
