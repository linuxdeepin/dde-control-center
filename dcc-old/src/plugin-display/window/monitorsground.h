//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later


#ifndef MONITORSGROUND_H
#define MONITORSGROUND_H

#include "interface/namespace.h"
#include "operation/monitor.h"

#include <math.h>
#include <float.h>

#include <DGraphicsView>

class QGraphicsScene;
class QTimer;
class QScroller;
class QScrollArea;

DWIDGET_USE_NAMESPACE
namespace DCC_NAMESPACE {

class DisplayModel;
class MonitorProxyWidget;
class MonitorsGround : public DGraphicsView
{
    Q_OBJECT

public:
    explicit MonitorsGround(int activateHeight, QWidget *parent = nullptr);
    ~MonitorsGround();

    inline void setMergeMode(bool val) { m_setMergeMode = val; }
    void setModel(DisplayModel *model, Monitor *moni = nullptr);
Q_SIGNALS:
    void requestApplySettings(QHash<Monitor *, QPair<int, int>> monitorposition);
    void requestMonitorPress(Monitor *mon);
    void requestMonitorRelease(Monitor *mon);
    void showsecondaryScreen();
    void setEffectiveReminderVisible(bool visible, int nEffectiveTime);

private Q_SLOTS:
    void onRequestMouseMove(MonitorProxyWidget *pw);
    void onRequestKeyPress(MonitorProxyWidget *pw, int keyValue);
    void onRequestMonitorRelease();
    void onGeometryChanged();
    void onCurrentModeChanged();
    void onRotateChanged();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void enterEvent(QEvent *) override;
    void leaveEvent(QEvent *) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void applySettings();
    void adjustAll();
    void onResize();
    void resetMonitorsView();
    void centeredMonitorsView();
    void executemultiScreenAlgo(const bool isRebound);

    /*1050-5401*/
    QPointF multiScreenSortAlgo(bool &isRestore, const bool isRebound = true);//排序算法 返回值为计算之后需要移动的XY值
    void multiScreenAutoAdjust(); // 手动调整完如果出现没有完全连通的情况，需要启动自动调整算法
    bool updateConnectedState(bool isInit = false); //更新连通状态
    QList<MonitorProxyWidget *> getConnectedDomain(MonitorProxyWidget *item); //获取每个屏幕的连通域
    void updateScale();
    void singleScreenAdjest();//单屏幕调整
    void autoRebound(); //自动回弹流程
    void initMonitorProxyWidget(Monitor *mon);

private:
    DisplayModel *m_model;
    QGraphicsScene m_graphicsScene; //场景
    QScrollArea *m_scrollArea;

    QMap<MonitorProxyWidget *, Monitor *> m_monitors;

    /*1050-5401*/
    QList<MonitorProxyWidget *> m_lstItems;
    QList<MonitorProxyWidget *> m_lstSortItems;
    MonitorProxyWidget * m_movingItem;              //正在移动的块
    QList<QPair<MonitorProxyWidget *, qreal>> m_lstMoveingItemToCenterPosLen;           //所有块的中心点到移动点的距离
    QMap<MonitorProxyWidget *, QList<MonitorProxyWidget *>> m_mapItemConnectedState;    //所有块的实时连通状态
    QMap<MonitorProxyWidget *, QList<MonitorProxyWidget *>> m_mapInitItemConnectedState; //所有块的初始连通状态

    QTimer *m_refershTimer;
    QTimer *m_effectiveTimer;

    int m_isSingleDisplay; //当前界面只显示单个屏幕
    double m_scale; //缩放比例
    bool m_isInit; //初始化完成
    int m_nEffectiveTime; //多屏设置生效时间
    bool m_setMergeMode;
};
}

#endif // MONITORSGROUND_H
