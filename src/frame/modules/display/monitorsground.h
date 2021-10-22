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

#ifndef MONITORSGROUND_H
#define MONITORSGROUND_H

#include "monitor.h"

#include <QWidget>
#include <QGraphicsScene>

#include <DGraphicsView>
#include <DApplicationHelper>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace display {

class DisplayModel;
class MonitorProxyWidget;
class MonitorsGround : public DGraphicsView {
    Q_OBJECT

public:
    explicit MonitorsGround(int activateHeight, QWidget *parent = nullptr);
    ~MonitorsGround();

    void setMergeMode(bool val) { m_setMergeMode = val; }
    void setModel(DisplayModel *model, Monitor *moni = nullptr);
Q_SIGNALS:
    void requestApplySettings(QHash<Monitor *, QPair<int, int>> monitorposition);
    void requestMonitorPress(Monitor *mon);
    void requestMonitorRelease(Monitor *mon);
    void showSecondaryScreen();

private Q_SLOTS:
    void resetMonitorsView();
    void onRequestMouseMove(MonitorProxyWidget *pw);
    void onRequestKeyPress(MonitorProxyWidget *pw, int keyValue);
    void centeredMonitorsView();
    void adjustAll();
    void onRequestMonitorRelease(Monitor *mon);
    void onResize();
    void onRotateChanged(const quint16 rotate);
    void onGeometryChanged();
    void onTakeEffect();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;

private:
    void applySettings();
    double screenScale() const;

    /*1050-5401*/
    QPointF multiScreenSortAlgo(bool &isRestore, bool isMove = true); //排序算法 返回值为计算之后需要移动的XY值
    void multiScreenAutoAdjust(); // 手动调整完如果出现没有完全连通的情况，需要启动自动调整算法
    void updateConnectedState(bool isInit = false);                           //更新连通状态
    QList<MonitorProxyWidget *> getConnectedDomain(MonitorProxyWidget *item); //获取每个屏幕的连通域
    void singleScreenAdjust();     //单屏幕调整
    void autoCenterData(QMap<MonitorProxyWidget *, QPointF *> &mapPos,
                        QPointF &dPos,
                        QPointF &offsetPos); //获取自动居中数据源

    void autoRebound(); //自动回弹流程
    void autoFitItemsInView();

    void initMonitorProxyWidget(Monitor *mon);

private:
    DisplayModel *m_model;
    QGraphicsScene m_graphicsScene; //场景
    QScrollArea *m_scrollArea;

    QMap<MonitorProxyWidget *, Monitor *> m_monitors;

    /*1050-5401*/
    QList<MonitorProxyWidget *> m_lstItems;
    QList<MonitorProxyWidget *> m_lstSortItems;
    MonitorProxyWidget *m_movingItem;                                         //正在移动的块
    QList<QPair<MonitorProxyWidget *, qreal>> m_lstMoveingItemToCenterPosLen; //所有块的中心点到移动点的距离
    QMap<MonitorProxyWidget *, QList<MonitorProxyWidget *>> m_mapItemConnectedState; //所有块的实时连通状态
    QMap<MonitorProxyWidget *, QList<MonitorProxyWidget *>> m_mapInitItemConnectedState; //所有块的初始连通状态

    QTimer *m_refershTimer;
    QTimer *m_effectiveTimer;

    int m_isSingleDisplay; //当前界面只显示单个屏幕
    bool m_isInit;         //初始化完成
    int m_nEffectiveTime;  //多屏设置生效时间
    bool m_setMergeMode;
};

} // namespace display

} // namespace dcc

#endif // MONITORSGROUND_H
