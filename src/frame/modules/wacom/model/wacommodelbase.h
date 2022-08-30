// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef WACOMMODELBASE_H
#define WACOMMODELBASE_H

#include <QObject>
#include <QDebug>
namespace dcc
{
namespace wacom
{
class WacomModelBase : public QObject
{
    Q_OBJECT
public:
    explicit WacomModelBase(QObject *parent = 0);
    void setPressureValue(const int &value);
    inline bool getPressureValue() {return m_pressureValue;}

Q_SIGNALS:
    void pressureValueChanged(const int &value);

private:
    int m_pressureValue = 0;
};
}
}

#endif // WACOMMODELBASE_H
