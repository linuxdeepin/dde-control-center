//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef TOUCHSCREENMODEL_P_H
#define TOUCHSCREENMODEL_P_H

#include "touchscreenmodel.h"
#include "touchscreenproxy.h"

#include <QDebug>
using namespace DCC_NAMESPACE;
class TouchScreenModelPrivate
{
    Q_DECLARE_PUBLIC(TouchScreenModel)
public:
    explicit TouchScreenModelPrivate(TouchScreenModel *object)
        : q_ptr(object)
        , m_displayProxy(new TouchScreenProxy(q_ptr))
    {
        init();
    }

public:
    TouchScreenModel *q_ptr;

protected:
    void touchScreenListChanged(const TouchscreenInfoList_V2 &newTouchScreenList)
    {
        Q_Q(TouchScreenModel);
        if (m_touchScreenList == newTouchScreenList)
            return;
        m_touchScreenList = newTouchScreenList;
        Q_EMIT q->touchScreenListChanged(m_touchScreenList);
    }

    void touchMapChanged(const TouchscreenMap &newTouchMap)
    {
        Q_Q(TouchScreenModel);
        if (m_touchMap == newTouchMap)
            return;
        m_touchMap = newTouchMap;
        Q_EMIT q->touchMapChanged();
    }

    void init()
    {
        QObject::connect(m_displayProxy, &TouchScreenProxy::TouchscreensV2Changed, q_ptr,
                         [this] (const TouchscreenInfoList_V2 &newTouchScreenList) {
                             touchScreenListChanged({newTouchScreenList});
                         });
        QObject::connect(m_displayProxy, &TouchScreenProxy::MonitorsChanged, q_ptr,
                         [this] (const QList<QDBusObjectPath> & monitors) {
                             monitorsChanged(monitors);
                         });
        QObject::connect(m_displayProxy, &TouchScreenProxy::TouchMapChanged, q_ptr,
                         [this] (TouchscreenMap  value) {
                             touchMapChanged(value);
                         });
        monitorsChanged(m_displayProxy->monitors());
        touchScreenListChanged(m_displayProxy->touchscreensV2());
        touchMapChanged(m_displayProxy->touchMap());
        return;
    }

    void monitorsChanged(const QList<QDBusObjectPath> & monitors);
    void assoiateTouch(const QString &monitor, const QString &touchscreenUUID);
    void assoiateTouchNotify();

private:
    TouchScreenProxy *m_displayProxy;
    TouchscreenInfoList_V2 m_touchScreenList;
    QStringList m_monitors;
    TouchscreenMap m_touchMap;
};

#endif // TOUCHSCREENMODEL_P_H
