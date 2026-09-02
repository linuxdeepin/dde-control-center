// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QPointer>

class QAbstractItemModel;
class DSAppletManager : public QObject
{
    Q_OBJECT
public:
    static DSAppletManager *instance();

    // 返回 dde-apps 的 appModel；此处仅暴露观察指针。
    QAbstractItemModel *appModel() const;

private:
    explicit DSAppletManager(QObject *parent = nullptr);

    QPointer<QAbstractItemModel> m_appModel;
};
