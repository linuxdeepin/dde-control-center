// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once
#include <QObject>

namespace dccV25 {
#define DccFactory_iid_v10 "org.deepin.dde.dcc-factory/v1.0"
class DccObject;

class DccFactory : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;

    // 作为数据返回，会导出为dccData供main.qml使用
    virtual QObject *create(QObject * = nullptr) { return nullptr; }

    // 未提供qml的，可在此自己加载qml返回DccObject对象
    virtual DccObject *dccObject(QObject * = nullptr) { return nullptr; }
};
} // namespace dccV25
Q_DECLARE_INTERFACE(dccV25::DccFactory, DccFactory_iid_v10)
