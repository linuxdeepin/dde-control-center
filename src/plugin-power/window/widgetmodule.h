/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#ifndef WIDGETMODULE_H
#define WIDGETMODULE_H
#include "interface/moduleobject.h"

class QWidget;

class WidgetModuleBase : public DCC_NAMESPACE::ModuleObject
{
    Q_OBJECT
public:
    WidgetModuleBase(const QString &name, const QString &displayName, QObject *parent = nullptr);
    // 发出信号
    virtual void onUpdateWidget(QWidget *widget);
Q_SIGNALS:
    void updateWidget(QWidget *widget);

protected:
    QWidget *m_widget;
};

template<typename T>
class WidgetModule : public WidgetModuleBase
{
public:
    template<class classT>
    WidgetModule(const QString &name, const QString &displayName, classT *receiver, void (classT::*callback)(T *), QObject *parent = nullptr)
        : WidgetModuleBase(name, displayName, parent)
    {
        connect(receiver, callback);
    }
    template<typename FunT>
    WidgetModule(const QString &name, const QString &displayName, FunT callback, QObject *parent = nullptr)
        : WidgetModuleBase(name, displayName, parent)
    {
        auto fun = [](QWidget *w, FunT callback) {
            T *widget = static_cast<T *>(w);
            (callback)(widget);
        };
        m_callback = std::bind(fun, std::placeholders::_1, callback);
    }
    WidgetModule(const QString &name = QString(), const QString &displayName = QString(), QObject *parent = nullptr)
        : WidgetModuleBase(name, displayName, parent)
        , m_callback(nullptr)
    {
    }
    template<class classT>
    void connect(classT *receiver, void (classT::*callback)(T *))
    {
        auto fun = [](QWidget *w, classT *receiver, void (classT::*callback)(T *)) {
            T *widget = static_cast<T *>(w);
            (receiver->*callback)(widget);
        };
        m_callback = std::bind(fun, std::placeholders::_1, receiver, callback);
    }

    virtual QWidget *page() override
    {
        m_widget = new T();
        onUpdateWidget(m_widget);
        return m_widget;
    }
    T *widget() { return static_cast<T *>(m_widget); }
    void onUpdateWidget(QWidget *w) override
    {
        if (m_callback)
            m_callback(w);
        WidgetModuleBase::onUpdateWidget(w);
    }

private:
    std::function<void(QWidget *)> m_callback;
};

#endif // WIDGETMODULE_H
