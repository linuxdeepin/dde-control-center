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

#ifndef MONITORCONTROLWIDGET_H
#define MONITORCONTROLWIDGET_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QPushButton;
QT_END_NAMESPACE

namespace dcc {

namespace display {

class Monitor;
class DisplayModel;
class MonitorsGround;
class MonitorControlWidget : public QFrame
{
    Q_OBJECT

public:
    explicit MonitorControlWidget(QWidget *parent = nullptr);

    void setModel(DisplayModel *model, Monitor *moni = nullptr);
    void setScreensMerged(const int mode);

Q_SIGNALS:
    void requestRecognize() const;
    void requestGatherWindows(const QPoint cursor) const;
    void requestSetMonitorPosition(Monitor *mon, const int x, const int y) const;
    void requestMonitorPress(Monitor *mon);
    void requestMonitorRelease(Monitor *mon);

public Q_SLOTS:
    void onGatherEnabled(const bool enable);

private:
    MonitorsGround *m_screensGround;
    QPushButton *m_recognize;
    QPushButton *m_gather;
};

} // namespace display

} // namespace dcc

#endif // MONITORCONTROLWIDGET_H
