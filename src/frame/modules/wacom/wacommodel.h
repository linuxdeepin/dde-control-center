// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef WACOMMODEL_H
#define WACOMMODEL_H

#include <QObject>

namespace  dcc
{
namespace wacom
{
class WacomModelBase;
class WacomModel : public QObject
{
    Q_OBJECT
    friend class MouseWorker;
public:
    explicit WacomModel(QObject *parent = 0);
    ~WacomModel();
    WacomModelBase *getWacomModelBase();

    bool exist() const;
    void setExist(bool exist);

Q_SIGNALS:
    void existChanged(bool exist);
    void cursorModeChanged(const bool cursorMode);

private:
    WacomModelBase *m_wacomModelBase;
    bool m_exist;
};
}
}

#endif // WACOMMODEL_H
