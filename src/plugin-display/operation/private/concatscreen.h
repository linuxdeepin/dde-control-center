// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef CONCATSCREEN_H
#define CONCATSCREEN_H

#include <QList>
#include <QMap>
#include <QPointF>
#include <QRectF>

QT_BEGIN_NAMESPACE
class QQuickItem;
QT_END_NAMESPACE

namespace dccV25 {
class DccScreen;

class ScreenData
{
public:
    explicit ScreenData(QQuickItem *item, qreal scale);
    ScreenData(DccScreen *screen);
    QRectF rect() const;
    QRectF rectEx() const;
    QRectF justIntersectRect() const;
    DccScreen *screen() const;
    void moveBy(qreal dx, qreal dy);
    void rebound();

private:
    QQuickItem *m_item;
    DccScreen *m_screen;
    qreal m_scale;
    QRectF m_rect;
};

class ConcatScreen
{
public:
    explicit ConcatScreen(QList<ScreenData *> listItems, ScreenData *pw);
    void adsorption();
    void executemultiScreenAlgo(bool isRebound);

private:
    bool multiScreenSortAlgo(bool &isRestore, const bool isRebound = true); // 排序算法 返回值 是否要重新拼接//为计算之后需要移动的XY值
    void multiScreenAutoAdjust();                                           // 手动调整完如果出现没有完全连通的情况，需要启动自动调整算法
    bool updateConnectedState(bool isInit = false);                         // 更新连通状态
    QList<ScreenData *> getConnectedDomain(ScreenData *item);               // 获取每个屏幕的连通域
    void autoRebound();                                                     // 自动回弹流程

private:
    QList<ScreenData *> m_listItems;
    ScreenData *m_movingItem;
    QList<ScreenData *> m_lstSortItems;
    QMap<ScreenData *, QList<ScreenData *>> m_mapItemConnectedState;     // 所有块的实时连通状态
    QMap<ScreenData *, QList<ScreenData *>> m_mapInitItemConnectedState; // 所有块的初始连通状态
};
} // namespace dccV25
#endif // CONCATSCREEN_H
