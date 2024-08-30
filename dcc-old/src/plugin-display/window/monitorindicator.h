//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MONITORFULLWIDGET_H
#define MONITORFULLWIDGET_H

#include "interface/namespace.h"

#include <DPlatformWindowHandle>

#include <QFrame>

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {

class MonitorIndicator : public QFrame
{
    Q_OBJECT

public:
    explicit MonitorIndicator(QWidget *parent = 0);
    ~MonitorIndicator() override;

public Q_SLOTS:

    virtual void setVisible(bool visible) override;

private:
    void updateGeometry();

private:
    QFrame *m_topLine;
    QFrame *m_bottomLine;
    QFrame *m_leftLine;
    QFrame *m_rightLine;
};
}

#endif // MONITORFULLWIDGET_H
