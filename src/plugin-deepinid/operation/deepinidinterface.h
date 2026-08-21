// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DEEPINIDINTERFACE_H
#define DEEPINIDINTERFACE_H

#include "deepinidmodel.h"
#include "deepinidworker.h"

class DeepinIDInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DeepinidModel *model READ getModel NOTIFY deepinidModelChanged)
    Q_PROPERTY(DeepinWorker *worker READ getWorker NOTIFY deepinidWorkerChanged)

public:
    explicit DeepinIDInterface(QObject *parent = nullptr);

    DeepinidModel *getModel() const { return m_model; };
    DeepinWorker *getWorker() const { return m_worker; };

    Q_INVOKABLE QString editionName() const;

signals:
    void deepinidModelChanged(DeepinidModel *model);
    void deepinidWorkerChanged(DeepinWorker *worker);

private:
    DeepinidModel *m_model;
    DeepinWorker *m_worker;

};

#endif // DEEPINIDINTERFACE_H
