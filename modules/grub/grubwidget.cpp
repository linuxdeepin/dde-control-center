/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QPixmap>
#include <QDebug>
#include <QTimer>

#include <dtextbutton.h>
#include <dseparatorhorizontal.h>
#include <dcolorpicker.h>
#include <dsegmentedcontrol.h>
#include <dbuttonlist.h>
#include <dbuttongrid.h>

#include "moduleheader.h"

#include "grubwidget.h"
#include "dbustheme.h"
#include "bootmenulist.h"

GrubWidget::GrubWidget(QWidget *parent):
    QFrame(parent),
    m_layout(new QVBoxLayout()),
    m_header(new ModuleHeader(tr("Boot Menu"))),
    m_arrowDefaultBoot(new DArrowLineExpand()),
    m_arrowBootDelay(new DArrowLineExpand()),
    m_arrowTextColor(new DArrowLineExpand()),
    m_arrowSelectedTextColor(new DArrowLineExpand()),
    m_themeDbus(new GrubThemeDbus(this)),
    m_grubDbus(new GrubDbus(this)),
    m_grubBackground(new GrubBackground(m_themeDbus)),
    m_bootEntryList(new DButtonList())
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    init();
    m_grubBackground->installEventFilter(this);

    connect(m_themeDbus, &GrubThemeDbus::updatingChanged, this, &GrubWidget::updatingChanged);
    connect(m_grubDbus, &GrubDbus::UpdatingChanged, this, &GrubWidget::updatingChanged);
}

GrubWidget::~GrubWidget()
{

}

bool GrubWidget::eventFilter(QObject *obj, QEvent *e)
{
    if(obj == m_grubBackground){
        if(e->type() == QEvent::Resize){
            m_tooltip->move(0, m_grubBackground->height()-m_tooltip->height());
        }
    }

    return false;
}

void GrubWidget::init()
{
    m_layout->setMargin(0);

    connect(m_header, &ModuleHeader::resetButtonClicked, m_grubDbus, &GrubDbus::Reset);

    m_tooltip = new QLabel(m_grubBackground);
    m_tooltip->setStyleSheet("QLabel{background:rbga(0,0,0,110);color:white;}");
    m_tooltip->resize(310, 20);
    m_tooltip->setText(tr("Drag and drop an image to change background."));
    m_tooltip->setAlignment(Qt::AlignCenter);

    QStringList title_list = m_grubDbus->GetSimpleEntryTitles();
    m_bootEntryList->setItemWidth(310);
    m_bootEntryList->addButtons(title_list);
    m_bootEntryList->setFixedSize(310, m_bootEntryList->count()*MENU_ITEM_HEIGHT);
    m_bootMenuTitle = new BootMenuList(m_grubBackground);
    m_bootMenuTitle->addButtons(title_list);
    m_bootMenuTitle->move(50, 50);
    m_bootMenuTitle->setNormalColor(m_themeDbus->itemColor());
    m_bootMenuTitle->setEnabledColor(m_themeDbus->selectedItemColor());
    connect(m_themeDbus, &GrubThemeDbus::itemColorChanged, m_bootMenuTitle, &BootMenuList::setNormalColor);
    connect(m_themeDbus, &GrubThemeDbus::selectedItemColorChanged, m_bootMenuTitle, &BootMenuList::setEnabledColor);

    setDefaultEntry(m_grubDbus->defaultEntry());
    connect(m_grubDbus, &GrubDbus::DefaultEntryChanged, this, &GrubWidget::setDefaultEntry);
    connect(m_bootEntryList, &DButtonList::buttonChecked, m_grubDbus, &GrubDbus::setDefaultEntry);

    m_arrowDefaultBoot->setTitle(tr("Default Boot"));
    m_arrowDefaultBoot->setContent(m_bootEntryList);

    DButtonGrid *timeout_select = new DButtonGrid(1, 7);
    m_timeoutList<<"1s"<<"5s"<<"10s"<<"15s"<<"20s"<<"25s"<<"30s";
    timeout_select->addButtons(m_timeoutList);
    timeout_select->setItemSize(30, RADIO_ITEM_HEIGHT);
    timeout_select->setFixedHeight(RADIO_ITEM_HEIGHT);
    timeout_select->checkButtonByIndex(m_timeoutList.indexOf(QString("%1s").arg(m_grubDbus->timeout())));
    connect(m_grubDbus, &GrubDbus::TimeoutChanged, [=](int timeout){
        timeout_select->checkButtonByIndex(m_timeoutList.indexOf(QString("%1s").arg(timeout)));
    });
    connect(timeout_select, &DButtonGrid::buttonChecked, [=](QString title){
        m_grubDbus->setTimeout(title.replace('s', "").toInt());
    });

    m_arrowBootDelay->setTitle(tr("Boot delay"));
    m_arrowBootDelay->setContent(timeout_select);

    DColorPicker *picker1 = new DColorPicker(10, this);
    picker1->setCurrentColor(m_themeDbus->itemColor());
    connect(picker1, &DColorPicker::currentColorChanged, m_themeDbus, &GrubThemeDbus::setItemColor);
    connect(m_themeDbus, &GrubThemeDbus::itemColorChanged, picker1, &DColorPicker::setCurrentColor);

    m_arrowTextColor->setTitle(tr("Text Color"));
    m_arrowTextColor->setContent(picker1);

    DColorPicker *picker2 = new DColorPicker(10, this);
    picker2->setCurrentColor(m_themeDbus->selectedItemColor());
    connect(picker2, &DColorPicker::currentColorChanged, m_themeDbus, &GrubThemeDbus::setSelectedItemColor);
    connect(m_themeDbus, &GrubThemeDbus::selectedItemColorChanged, picker2, &DColorPicker::setCurrentColor);

    m_arrowSelectedTextColor->setTitle(tr("Selected Text Color"));
    m_arrowSelectedTextColor->setContent(picker2);

    m_layout->setSpacing(0);
    m_layout->addWidget(m_header);
    m_layout->addWidget(new DSeparatorHorizontal());
    m_layout->addWidget(m_grubBackground);
    m_layout->addWidget(new DSeparatorHorizontal());
    m_layout->addWidget(m_arrowDefaultBoot);
    m_layout->addWidget(m_arrowBootDelay);
    m_layout->addWidget(m_arrowTextColor);
    m_layout->addWidget(m_arrowSelectedTextColor);
    m_layout->addStretch(1);

    setLayout(m_layout);
}

void GrubWidget::setDefaultEntry(const QString &entry)
{
    QStringList title_list = m_grubDbus->GetSimpleEntryTitles();
    int default_index = title_list.indexOf(entry);

    if(default_index >= 0 && default_index < m_bootEntryList->count()){
        m_bootEntryList->checkButtonByIndex(default_index);
        m_bootMenuTitle->checkButtonByIndex(default_index);
    }
}

void GrubWidget::updatingChanged(bool updating)
{
    m_tooltip->setStyleSheet(m_tooltip->styleSheet()+"QLabel{color:#DF8000;}");
    if(updating){
        m_tooltip->setText(tr("Updating..."));
    }else{
        m_tooltip->setText(tr("Successfully updated, reboot to view."));
        QTimer::singleShot(2000, this, SLOT(resetTooltip()));
    }
}

void GrubWidget::resetTooltip()
{
    m_tooltip->setStyleSheet(m_tooltip->styleSheet()+"QLabel{color:white;}");
    m_tooltip->setText(tr("Drag and drop an image to change background."));
}
