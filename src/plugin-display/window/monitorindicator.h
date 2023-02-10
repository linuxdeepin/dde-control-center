/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
} // namespace DCC_NAMESPACE

#endif // MONITORFULLWIDGET_H
