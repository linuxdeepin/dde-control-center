//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef TOUCHSCREENMODEL_H
#define TOUCHSCREENMODEL_H

#include "types/touchscreeninfolist_v2.h"
#include "types/touchscreenmap.h"

#include <QObject>

#define DCC_DECLARE_PRIVATE(Class) \
private: \
    QScopedPointer<Class##Private> d_ptr##Class; \
    Q_DECLARE_PRIVATE_D(d_ptr##Class, Class)\
    Q_DISABLE_COPY(Class)

#define DCC_INIT_PRIVATE(Class) d_ptr##Class(new Class##Private(this))

class TouchScreenMatchModel;
class TouchScreenModelPrivate;

class TouchScreenModel : public QObject
{
    Q_OBJECT
    DCC_DECLARE_PRIVATE(TouchScreenModel)
public:
    explicit TouchScreenModel(QObject *parent = nullptr);
    ~TouchScreenModel();

    Q_PROPERTY(const TouchscreenInfoList_V2 touchScreenList READ touchScreenList NOTIFY touchScreenListChanged)
    Q_PROPERTY(const QStringList monitors READ monitors NOTIFY monitorsChanged)
    Q_PROPERTY(TouchscreenMap touchMap READ touchMap NOTIFY touchMapChanged)

    const TouchscreenInfoList_V2 &touchScreenList() const;

    const QStringList &monitors();

    const TouchscreenMap &touchMap() const;

    Q_INVOKABLE void assoiateTouch(const QString &monitor, const QString &touchscreenUUID);
    void assoiateTouchNotify();

public Q_SLOTS:

    inline TouchScreenMatchModel *touchScreenMatchModel() const { return m_touchScreenMatchModel; }

Q_SIGNALS:
    void touchScreenListChanged(const TouchscreenInfoList_V2 &newTouchScreenList);
    void monitorsChanged(const QStringList monitors);
    void touchMapChanged();

private:
    TouchscreenMap m_touchMap;
    TouchScreenMatchModel *m_touchScreenMatchModel;
};
#endif // TOUCHSCREENMODEL_H
