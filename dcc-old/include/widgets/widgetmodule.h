//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef WIDGETMODULE_H
#define WIDGETMODULE_H
#include "interface/moduleobject.h"

class QWidget;

template<typename T>
class WidgetModule : public DCC_NAMESPACE::ModuleObject
{
public:
    template<class classT>
    WidgetModule(const QString &name, const QString &displayName, classT *receiver, void (classT::*callback)(T *), QObject *parent = nullptr)
        : DCC_NAMESPACE::ModuleObject(name, displayName, parent)
    {
        connect(receiver, callback);
    }
    template<typename FunT>
    WidgetModule(const QString &name, const QString &displayName, FunT callback, QObject *parent = nullptr)
        : DCC_NAMESPACE::ModuleObject(name, displayName, parent)
    {
        auto fun = [](QWidget *w, FunT callback) {
            T *tWidget = static_cast<T *>(w);
            (callback)(tWidget);
        };
        m_callback = std::bind(fun, std::placeholders::_1, callback);
    }
    WidgetModule(const QString &name = QString(), const QString &displayName = QString(), QObject *parent = nullptr)
        : DCC_NAMESPACE::ModuleObject(name, displayName, parent)
        , m_callback(nullptr)
    {
    }
    template<class classT>
    void connect(classT *receiver, void (classT::*callback)(T *))
    {
        auto fun = [](QWidget *w, classT *receiver, void (classT::*callback)(T *)) {
            T *tWidget = static_cast<T *>(w);
            (receiver->*callback)(tWidget);
        };
        m_callback = std::bind(fun, std::placeholders::_1, receiver, callback);
    }

    virtual QWidget *page() override
    {
        QWidget *widget = new T();
        if (m_callback)
            m_callback(widget);
        return widget;
    }

private:
    std::function<void(QWidget *)> m_callback;
};

#endif // WIDGETMODULE_H
