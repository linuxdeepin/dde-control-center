//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef COMMONINFOINTERACTION_H
#define COMMONINFOINTERACTION_H

#include "commoninfomodel.h"
#include "commoninfowork.h"

#include <QObject>

class CommonInfoInteraction : public QObject
{
    Q_OBJECT
public:
    explicit CommonInfoInteraction(QObject *parent = nullptr);

    Q_INVOKABLE CommonInfoWork *work() const;
    void setWork(CommonInfoWork *newWork);

    Q_INVOKABLE CommonInfoModel *mode() const;
    void setMode(CommonInfoModel *newMode);

signals:


private:
    CommonInfoWork* m_work;
    CommonInfoModel* m_mode;
};

#endif // COMMONINFOINTERACTION_H
