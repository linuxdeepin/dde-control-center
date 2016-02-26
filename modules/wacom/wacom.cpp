/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "constants.h"
#include "wacom.h"
#include "moduleheader.h"

DUI_USE_NAMESPACE

Wacom::Wacom() :QObject(),
                ModuleInterface() {
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Wacom"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    initUI();
    initWacom();
    initConnects();
}

Wacom::~Wacom(){
    m_frame->deleteLater();
}

QFrame* Wacom::getContent(){
    return m_frame;
}

void Wacom::initUI(){
    const int labelWidth(105);
    const int segmentedCtrlAndComBoxWidth(160);
    const int dsliderWidth(170);
    const int pressureSliderRangeMaxValue(10);
    const int doubleClickSpaceRangeMaxValue(100);
    QStringList keylist;
    keylist.append(tr("Page Up"));
    keylist.append(tr("Page Down"));
    keylist.append(tr("Left Click"));
    keylist.append(tr("Right Click"));
    keylist.append(tr("Middle Click"));
    QString fontStyle("QLabel{color:%1;font-size:12px}");
    fontStyle = fontStyle.arg(DCC::TextNormalColor.name());

    m_frame = new QFrame();
    m_frame->setFixedWidth(DCC::ModuleContentWidth);
    m_frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QVBoxLayout* wacomVLayout = new QVBoxLayout(m_frame);
    wacomVLayout->setContentsMargins(0,0,0,0);
    wacomVLayout->setSpacing(0);

    // WacomDbusInterface ,this variable is important
    m_wacomInterface = new DBusInputWacom("com.deepin.daemon.InputDevices",
                                          "/com/deepin/daemon/InputDevice/Wacom",
                                          QDBusConnection::sessionBus(),
                                          this);

    // Moudule header,to show "Tablet"
    m_wacomModuleHeader = new ModuleHeader(tr("Tablet"));
    // horizontal line under "Tablet"
    m_theHSeparator1 = new DSeparatorHorizontal(m_frame);


    // Panels under "Tablet" title
    wacomPanel = new QWidget(m_frame);
    QVBoxLayout* wacomPanelVLayout = new QVBoxLayout(wacomPanel);
    wacomPanel->setLayout(wacomPanelVLayout);
    wacomPanel->setContentsMargins(0,0,0,0);

    QWidget* tabletOrientatinItem = new QWidget(m_frame);
    QHBoxLayout* tabletOriHLayout = new QHBoxLayout(tabletOrientatinItem);

    QLabel* tabletOriLabel = new QLabel(tr("Tablet Orientation"));
    tabletOriLabel->setStyleSheet(fontStyle);
    tabletOriLabel->setFixedWidth(labelWidth);
    tabletOriLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_tabletOriSCtrl = new DSegmentedControl(tabletOrientatinItem);
    m_tabletOriSCtrl->addSegmented(tr("Left Hand"));
    m_tabletOriSCtrl->addSegmented(tr("Right Hand"));
    m_tabletOriSCtrl->setFixedWidth(segmentedCtrlAndComBoxWidth);

    // to display them horizontally
    tabletOriHLayout->addWidget(tabletOriLabel);
    tabletOriHLayout->addWidget(m_tabletOriSCtrl);

    // DigitalPenModes Item
    QWidget* digitalPenModesItem = new QWidget(m_frame);
    QHBoxLayout* digitalPenModesHLayout = new QHBoxLayout(digitalPenModesItem);

    // DigitalPenModesItem contents
    QLabel* digitalPenModesLabel = new QLabel(tr("Digital Pen Modes"));
    digitalPenModesLabel->setStyleSheet(fontStyle);
    digitalPenModesLabel->setFixedWidth(labelWidth);
    digitalPenModesLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_digitalPenModesSCtrl = new DSegmentedControl(digitalPenModesItem);
    m_digitalPenModesSCtrl->addSegmented(tr("Pen Mode"));
    m_digitalPenModesSCtrl->addSegmented(tr("Mouse Mode"));
    m_digitalPenModesSCtrl->setFixedWidth(segmentedCtrlAndComBoxWidth);

    // to display them horizontally
    digitalPenModesHLayout->addWidget(digitalPenModesLabel);
    digitalPenModesHLayout->addWidget(m_digitalPenModesSCtrl);

    // DownKey Item
    QWidget* downKeyItem = new QWidget(m_frame);
    QHBoxLayout* downKeyHLayout = new QHBoxLayout(downKeyItem);

    // DownKeyItem contents
    QLabel* downKeyLabel = new QLabel(tr("Down Key"));
    downKeyLabel->setStyleSheet(fontStyle);
    downKeyLabel->setFixedWidth(labelWidth);
    downKeyLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_downKeySimpleComboBox = new DSimpleComboBox(downKeyItem);
    m_downKeySimpleComboBox->addItems(keylist);
    m_downKeySimpleComboBox->setCurrentIndex(-1);
    m_downKeySimpleComboBox->setFixedSize(segmentedCtrlAndComBoxWidth, BUTTON_HEIGHT);


    // to display them horizontally
    downKeyHLayout->addWidget(downKeyLabel);
    downKeyHLayout->addWidget(m_downKeySimpleComboBox);

    // UpKey Item
    QWidget* upKeyItem = new QWidget(m_frame);
    QHBoxLayout* upKeyHLayout = new QHBoxLayout(upKeyItem);

    // UpKeyItem contents
    QLabel* upKeyLabel = new QLabel(tr("Up Key"));
    upKeyLabel->setStyleSheet(fontStyle);
    upKeyLabel->setFixedWidth(labelWidth);
    upKeyLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_upKeySimpleComboBox = new DSimpleComboBox(upKeyItem);
    m_upKeySimpleComboBox->addItems(keylist);
    m_upKeySimpleComboBox->setCurrentIndex(-1);
    m_upKeySimpleComboBox->setFixedSize(segmentedCtrlAndComBoxWidth,BUTTON_HEIGHT);

    // to display them horizontally
    upKeyHLayout->addWidget(upKeyLabel);
    upKeyHLayout->addWidget(m_upKeySimpleComboBox);

    // pressure slider item
    QWidget* pressureSliderItem = new QWidget(m_frame);
    QHBoxLayout* pressureSliderHLayout = new QHBoxLayout(pressureSliderItem);


    // pressureSlider contents
    QLabel* pressureLabel = new QLabel(tr("Pressure"));
    pressureLabel->setStyleSheet(fontStyle);
    pressureLabel->setFixedWidth(labelWidth);
    pressureLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_pressureSlider = new DSlider(Qt::Horizontal,pressureSliderItem);
    m_pressureSlider->setLeftTip(tr("Soft"));
    m_pressureSlider->setRightTip(tr("Hard"));
    m_pressureSlider->setFixedWidth(dsliderWidth);
    m_pressureSlider->setHoverShowValue(true);
    m_pressureSlider->setRange(1,pressureSliderRangeMaxValue);

    // to display prssureSlider contents horizontally
    pressureSliderHLayout->addWidget(pressureLabel);
    pressureSliderHLayout->addWidget(m_pressureSlider);

    // DoubleClickSpaceItem
    QWidget* doubleClickSpaceItem = new QWidget(m_frame);
    QHBoxLayout* doubleClickSpaceSliderHLayout = new QHBoxLayout(doubleClickSpaceItem);

    // DoubleClickSpaceSlider contents
    QLabel* doubleClickSpaceLabel = new QLabel(tr("Double-click Space"));
    doubleClickSpaceLabel->setStyleSheet(fontStyle);
    doubleClickSpaceLabel->setFixedWidth(labelWidth);
    doubleClickSpaceLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_doubleClickSpaceSlider = new DSlider(Qt::Horizontal,doubleClickSpaceItem);
    m_doubleClickSpaceSlider->setLeftTip(tr("Narrow"));
    m_doubleClickSpaceSlider->setRightTip(tr("Wide"));
    m_doubleClickSpaceSlider->setFixedWidth(dsliderWidth);
    m_doubleClickSpaceSlider->setHoverShowValue(true);
    m_doubleClickSpaceSlider->setRange(0,doubleClickSpaceRangeMaxValue);

    // to display them horizontally
    doubleClickSpaceSliderHLayout->addWidget(doubleClickSpaceLabel);
    doubleClickSpaceSliderHLayout->addWidget(m_doubleClickSpaceSlider);


    // the bottom Horizontal line
    m_theHSeparator2 = new DSeparatorHorizontal(m_frame);

    //to display the panel widgets vertically
    wacomPanelVLayout->addWidget(tabletOrientatinItem);
    wacomPanelVLayout->addWidget(digitalPenModesItem);
    wacomPanelVLayout->addWidget(downKeyItem);
    wacomPanelVLayout->addWidget(upKeyItem);
    wacomPanelVLayout->addWidget(pressureSliderItem);
    wacomPanelVLayout->addWidget(doubleClickSpaceItem);


    // to display the whole Wacom vertically
    wacomVLayout->addWidget(m_wacomModuleHeader);
    wacomVLayout->addWidget(m_theHSeparator1);
    wacomVLayout->addWidget(wacomPanel);
    wacomVLayout->addWidget(m_theHSeparator2);
    wacomVLayout->addStretch(1);
}

void Wacom::initWacom(){
    /* initial TabletOrientation,
       set m_tabletOriSCtrl's currentIndex by judging that m_wacomInterface's leftHanded() is true(1) or false(0) */
    m_tabletOriSCtrl->setCurrentIndex(m_wacomInterface->leftHanded() == 0);

    /* initial DigitalPenModes,
       set m_digitalPenModesSCtrl's currentIndex by judging that m_wacomInterface's cursoorMode() is true(1) or false(0) */
    m_digitalPenModesSCtrl->setCurrentIndex(m_wacomInterface->cursorMode() == 1);

    // initial KeyDownAction
    QString keyDownActionStr = m_wacomInterface->keyDownAction();
    if(keyDownActionStr == "PageUp"){
        m_downKeySimpleComboBox->setCurrentIndex(0);
    }else if(keyDownActionStr == "PageDown"){
        m_downKeySimpleComboBox->setCurrentIndex(1);
    }else if(keyDownActionStr == "LeftClick"){
        m_downKeySimpleComboBox->setCurrentIndex(2);
    }else if(keyDownActionStr == "RightClick"){
        m_downKeySimpleComboBox->setCurrentIndex(3);
    }else{
        m_downKeySimpleComboBox->setCurrentIndex(4);
    }

    //initial KeyUpAction
    QString keyUpActionStr = m_wacomInterface->keyUpAction();
    if(keyUpActionStr == "PageUp"){
        m_upKeySimpleComboBox->setCurrentIndex(0);
    }else if(keyUpActionStr == "PageDown"){
        m_upKeySimpleComboBox->setCurrentIndex(1);
    }else if(keyUpActionStr == "LeftClick"){
        m_upKeySimpleComboBox->setCurrentIndex(2);
    }else if(keyUpActionStr == "RightClick"){
        m_upKeySimpleComboBox->setCurrentIndex(3);
    }else{
        m_upKeySimpleComboBox->setCurrentIndex(4);
    }

    // initial Pressure
    uint wacomPressure = m_wacomInterface->pressureSensitive();
    m_pressureSlider->setValue((int)wacomPressure);

    // initial Double-click Space
    uint wacomDoubleclickSpace = m_wacomInterface->doubleDelta();
    m_doubleClickSpaceSlider->setValue((int)wacomDoubleclickSpace);
}

void Wacom::initConnects(){
    connect(m_wacomModuleHeader,&ModuleHeader::resetButtonClicked,this,&Wacom::reset);

    connect(m_tabletOriSCtrl,SIGNAL(currentChanged(int)),this,SLOT(setTabletOrientation(int)));
    connect(m_wacomInterface,&DBusInputWacom::LeftHandedChanged,[=](){
        m_tabletOriSCtrl->setCurrentIndex(m_wacomInterface->leftHanded() == 0);
    });

    connect(m_digitalPenModesSCtrl,SIGNAL(currentChanged(int)),this,SLOT(setDigitalPenModes(int)));
    connect(m_wacomInterface,&DBusInputWacom::CursorModeChanged,[=](){
        m_digitalPenModesSCtrl->setCurrentIndex(m_wacomInterface->cursorMode() == 1);
    });

    connect(m_downKeySimpleComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setDownKey(int)));
    connect(m_wacomInterface,&DBusInputWacom::KeyDownActionChanged,[=](QString keyDownStr){
        if(keyDownStr == "PageUp"){
            m_downKeySimpleComboBox->setCurrentIndex(0);
        }else if(keyDownStr == "PageDown"){
            m_downKeySimpleComboBox->setCurrentIndex(1);
        }else if(keyDownStr == "LeftClick"){
            m_downKeySimpleComboBox->setCurrentIndex(2);
        }else if(keyDownStr == "RightClick"){
            m_downKeySimpleComboBox->setCurrentIndex(3);
        }else{
            m_downKeySimpleComboBox->setCurrentIndex(4);
        }
    });

    connect(m_upKeySimpleComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setUpKey(int)));
    connect(m_wacomInterface,&DBusInputWacom::KeyUpActionChanged,[=](QString keyUpStr){
        if(keyUpStr == "PageUp"){
            m_upKeySimpleComboBox->setCurrentIndex(0);
        }else if(keyUpStr == "PageDown"){
            m_upKeySimpleComboBox->setCurrentIndex(1);
        }else if(keyUpStr == "LeftClick"){
            m_upKeySimpleComboBox->setCurrentIndex(2);
        }else if(keyUpStr == "RightClick"){
            m_upKeySimpleComboBox->setCurrentIndex(3);
        }else{
            m_upKeySimpleComboBox->setCurrentIndex(4);
        }
    });

    connect(m_pressureSlider,SIGNAL(valueChanged(int)),this,SLOT(setPressure(int)));
    connect(m_wacomInterface,&DBusInputWacom::PressureSensitiveChanged,[=](uint pressure){
        m_pressureSlider->setValue((int)pressure);
    });

    connect(m_doubleClickSpaceSlider,SIGNAL(valueChanged(int)),this,SLOT(setDoubleClickSpace(int)));
    connect(m_wacomInterface,&DBusInputWacom::DoubleDeltaChanged,[=](uint doubleDelta){
        m_doubleClickSpaceSlider->setValue((int)doubleDelta);
    });
}

// Reset every parameter of Tablet
void Wacom::reset(){
    m_wacomInterface->Reset();
    qDebug() << m_wacomInterface->leftHanded() << "  "
             << m_wacomInterface->cursorMode() << "  "
             << m_wacomInterface->keyDownAction() << "  "
             << m_wacomInterface->keyUpAction() << "  "
             << m_wacomInterface->pressureSensitive() << "  "
             << m_wacomInterface->doubleDelta() <<  "  ";
}

void Wacom::setTabletOrientation(int indexOfTabletSegCtrl){
    // set m_wacomInterface's leftHand by judging that if m_tabletOriSCtrl's currentIndex is 1 or 0
    m_wacomInterface->setLeftHanded(indexOfTabletSegCtrl == 0);
}

void Wacom::setDigitalPenModes(int indexOfDigitalPenModes){
    // set m_wacomInterface's cursorMode by judging that if m_digitalPenModesSCtrl's currentIndex is 1 or 0
    m_wacomInterface->setCursorMode(indexOfDigitalPenModes == 1);
}

void Wacom::setDownKey(int indexofdownkey){
    // 0~4 indicate the index of m_upKeySimpleComboBox
    switch (indexofdownkey) {
    case 0:
        m_wacomInterface->setKeyDownAction("PageUp");
        break;
    case 1:
        m_wacomInterface->setKeyDownAction("PageDown");
        break;
    case 2:
        m_wacomInterface->setKeyDownAction("LeftClick");
        break;
    case 3:
        m_wacomInterface->setKeyDownAction("RightClick");
        break;
    case 4:
        m_wacomInterface->setKeyDownAction("MiddleClick");
    }
}

void Wacom::setUpKey(int indexofupkey){
    // 0~4 indicate the index of m_downKeySimpleComboBox
    switch (indexofupkey) {
    case 0:
        m_wacomInterface->setKeyUpAction("PageUp");
        break;
    case 1:
        m_wacomInterface->setKeyUpAction("PageDown");
        break;
    case 2:
        m_wacomInterface->setKeyUpAction("LeftClick");
        break;
    case 3:
        m_wacomInterface->setKeyUpAction("RightClick");
        break;
    case 4:
        m_wacomInterface->setKeyUpAction("MiddleClick");
        break;
    }
}

void Wacom::setPressure(int pressure){
    m_wacomInterface->setPressureSensitive((uint)pressure);
}

void Wacom::setDoubleClickSpace(int space){
    m_wacomInterface->setDoubleDelta((uint)space);
}
