/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     fpc_diesel <fanpengcheng@uniontech.com>
 *
 * Maintainer: fpc_diesel <fanpengcheng@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "accessibleinterface.h"

#include "mainwindow.h"
#include "searchwidget.h"

#include <QPushButton>
#include <QWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QSlider>
#include <QFrame>
#include <QMenu>
#include <QLabel>

#include <DSwitchButton>
#include <DBackgroundGroup>
#include <DFloatingButton>
#include <DLineEdit>
#include <DLabel>
#include <DListView>
#include <DCommandLinkButton>
#include <DSearchEdit>
#include <DTitlebar>
#include <DComboBox>
/**************************************************************************************/
DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

// 主窗口
SET_FORM_ACCESSIBLE(MainWindow, "mainwindow")
SET_FORM_ACCESSIBLE(SearchWidget, "searchwidget")

// Qt控件
SET_FORM_ACCESSIBLE(QWidget, m_w->objectName().isEmpty() ? "widget" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(QPushButton, m_w->text().isEmpty() ? "qpushbutton" : m_w->text())
SET_EDITABLE_ACCESSIBLE(QLineEdit, m_w->text().isEmpty() ? "qlineedit" : m_w->text())
SET_BUTTON_ACCESSIBLE(QToolButton, m_w->text().isEmpty() ? "qtoolbutton" : m_w->text())
SET_SLIDER_ACCESSIBLE(QSlider, "qslider")
SET_FORM_ACCESSIBLE(QMenu, "qmenu")
SET_LABEL_ACCESSIBLE(QLabel, m_w->text().isEmpty() ? "qlabel" : m_w->text())

// Dtk控件
SET_FORM_ACCESSIBLE(DBackgroundGroup, m_w->objectName().isEmpty() ? "dbackgroundgroup" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DSwitchButton, m_w->text().isEmpty() ? "switchbutton" : m_w->text())
SET_BUTTON_ACCESSIBLE(DFloatingButton,  m_w->toolTip().isEmpty() ? "DFloatingButton" : m_w->toolTip())
SET_DTK_EDITABLE_ACCESSIBLE(DLineEdit, "DLineEdit")
SET_FORM_ACCESSIBLE(DSearchEdit, m_w->objectName().isEmpty() ? "DSearchEdit" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DIconButton, m_w->objectName().isEmpty() ? "DIconButton" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DCommandLinkButton, "DCommandLinkButton")
SET_LABEL_ACCESSIBLE(DLabel, m_w->text().isEmpty() ? m_w->objectName().isEmpty() ? "DLabel" : m_w->objectName() : m_w->text())
SET_FORM_ACCESSIBLE(DTitlebar, m_w->objectName().isEmpty() ? "DTitlebar" : m_w->objectName())


QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = nullptr;

    // 应用主窗口
    USE_ACCESSIBLE(QString(classname).replace("dccV23::", ""), MainWindow);
    // USE_ACCESSIBLE(QString(classname).replace("dccV20::search::", ""), SearchWidget);
    USE_ACCESSIBLE_BY_OBJECTNAME(classname, QWidget, "contentwindow");
    USE_ACCESSIBLE_BY_OBJECTNAME(classname, QWidget, "AvatarItem");
    USE_ACCESSIBLE_BY_OBJECTNAME(QString(classname).replace("Dtk::Widget::", ""), DBackgroundGroup, "modulepage");

    // Qt控件
    USE_ACCESSIBLE(classname, QPushButton);
    USE_ACCESSIBLE(classname, QToolButton);
    USE_ACCESSIBLE(classname, QLineEdit);
    USE_ACCESSIBLE(classname, QSlider);
    USE_ACCESSIBLE(classname, QMenu);
    USE_ACCESSIBLE(classname, QLabel);

    // Dtk控件
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DLineEdit);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DSwitchButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DIconButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DCommandLinkButton);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DSearchEdit);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DTitlebar);
    USE_ACCESSIBLE(QString(classname).replace("Dtk::Widget::", ""), DLabel);

    return interface;
}