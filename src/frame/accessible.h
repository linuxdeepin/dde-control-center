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

#include "widgets/accessibleinterface.h"
#include "src/widgets/accessiblefactoryinterface.h"

#include <QPushButton>
#include <QWidget>
#include <QLineEdit>
#include <QToolButton>
#include <QSlider>
#include <QFrame>
#include <QMenu>
#include <QLabel>
#include <QListWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QComboBox>
#include <QMainWindow>

/**************************************************************************************/
// Qt控件
SET_FORM_ACCESSIBLE(QWidget, m_w->objectName().isEmpty() ? "widget" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(QPushButton, m_w->text().isEmpty() ? "qpushbutton" : m_w->text())
SET_EDITABLE_ACCESSIBLE(QLineEdit, m_w->text().isEmpty() ? "qlineedit" : m_w->text())
SET_BUTTON_ACCESSIBLE(QToolButton, m_w->text().isEmpty() ? "qtoolbutton" : m_w->text())
SET_SLIDER_ACCESSIBLE(QSlider, "qslider")
SET_FORM_ACCESSIBLE(QMenu, "qmenu")
SET_FORM_ACCESSIBLE(QFrame, "qframe")
SET_FORM_ACCESSIBLE(QListView, "qlistview")
SET_FORM_ACCESSIBLE(QListWidget, "qlistwidget")
SET_FORM_ACCESSIBLE(QScrollArea, "qscrollarea")
SET_FORM_ACCESSIBLE(QScrollBar, "QScrollBar")
SET_FORM_ACCESSIBLE(QComboBox, "QComboBox")
SET_FORM_ACCESSIBLE(QMainWindow, "QMainWindow")
SET_LABEL_ACCESSIBLE(QLabel, m_w->text().isEmpty() ? "qlabel" : m_w->text())

class AccessibleFactory : public AccessibleFactoryInterface
{
public:
    explicit AccessibleFactory()
        : AccessibleFactoryInterface()
    {
        AccessibleFactoryInterface::RegisterInstance(this);
    }
    virtual ~AccessibleFactory() { }
    virtual AccessibleFactoryBase *registerAccessibleFactory(const char *factoryName, AccessibleFactoryBase *factory) override
    {
        if (!m_factoryMap.contains(factoryName))
            m_factoryMap.insert(factoryName, factory);
        return factory;
    }
    QAccessibleInterface *createObject(const QString &classname, QObject *object)
    {
        if (m_factoryMap.contains(classname))
            return m_factoryMap.value(classname)->createObject(object);
                                     qInfo()<<"no Accessible:"<<classname;
        return nullptr;
    }

private:
    QMap<QString, AccessibleFactoryBase *> m_factoryMap;
};

QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = nullptr;
   static AccessibleFactory * s_accessibleFactory = nullptr;
   if (!s_accessibleFactory)
        s_accessibleFactory = new AccessibleFactory();

   if (object && object->isWidgetType())
        interface = s_accessibleFactory->createObject(classname.split("::").last(),object);

    return interface;
}
