//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
        return m_factoryMap.contains(classname) ? m_factoryMap.value(classname)->createObject(object) : nullptr;
    }

private:
    QMap<QString, AccessibleFactoryBase *> m_factoryMap;
};

inline QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = nullptr;
   static AccessibleFactory * s_accessibleFactory = nullptr;
   if (!s_accessibleFactory)
        s_accessibleFactory = new AccessibleFactory();

   if (object && object->isWidgetType())
        interface = s_accessibleFactory->createObject(classname.split("::").last(),object);

    return interface;
}
