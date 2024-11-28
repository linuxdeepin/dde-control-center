// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UPDATEINTERACTION_H
#define UPDATEINTERACTION_H

#include <QObject>

#include "updatemodel.h"
#include "updatework.h"

class UpdateInteraction : public QObject
{
    Q_OBJECT
public:
    explicit UpdateInteraction(QObject *parent = nullptr);

    Q_INVOKABLE UpdateWork *work() const;
    void setWork(UpdateWork *newWork);

    Q_INVOKABLE UpdateModel *model() const;
    void setModel(UpdateModel *newModel);

signals:



private:
    UpdateWork* m_work;
    UpdateModel* m_model;
};

#endif // UPDATEINTERACTION_H
