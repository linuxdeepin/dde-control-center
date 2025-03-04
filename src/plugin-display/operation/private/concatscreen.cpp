// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "concatscreen.h"

#include "../dccscreen.h"

#include <QQuickItem>

namespace dccV25 {

ScreenData::ScreenData(QQuickItem *item, qreal scale)
    : m_item(item)
    , m_screen(item->property("screen").value<DccScreen *>())
    , m_scale(scale)
    , m_rect(QRectF(item->x() / scale, item->y() / scale, item->width() / scale, item->height() / scale))
{
}

ScreenData::ScreenData(DccScreen *screen)
    : m_item(nullptr)
    , m_screen(screen)
    , m_scale(1)
    , m_rect(QRectF(screen->x(), screen->y(), screen->width(), screen->height()))
{
}

QRectF ScreenData::rect() const
{
    return m_rect;
}

// 外扩0.05个像素, 规避由于计算导致精度丢失或者坐标值完全一致的情况下不能判定为相交的情况
QRectF ScreenData::rectEx() const
{
    return m_rect.adjusted(-0.05, -0.05, 0.05, 0.05);
}

QRectF ScreenData::justIntersectRect() const
{
    return m_rect.adjusted(1, 1, -1, -1);
}

DccScreen *ScreenData::screen() const
{
    return m_screen;
}

void ScreenData::moveBy(qreal dx, qreal dy)
{
    m_rect.translate(dx, dy);
    if (m_item) {
        m_item->setX(m_rect.x() * m_scale);
        m_item->setY(m_rect.y() * m_scale);
    }
}

void ScreenData::rebound()
{
    m_rect = QRectF(m_screen->x(), m_screen->y(), m_screen->width(), m_screen->height());
    if (m_item) {
        m_item->setX(m_rect.x() * m_scale);
        m_item->setY(m_rect.y() * m_scale);
    }
}

// 自动吸附实现
ConcatScreen::ConcatScreen(QList<ScreenData *> listItems, ScreenData *pw)
    : m_listItems(listItems)
    , m_movingItem(pw)
{
}

void ConcatScreen::adsorption()
{
    // 当鼠标移动的时候开始响应并执行自动吸附的逻辑
    // 保证 bufferboundingRect 相交且 boundingRect 不相交，保证移动的块item始终在其他item的外边缘移动
    qreal top = 0.0;
    qreal bottom = 0.0;
    qreal right = 0.0;
    qreal left = 0.0;

    qreal topLeft = 0.0;
    qreal topRight = 0.0;
    qreal bottomLeft = 0.0;
    qreal bottomRight = 0.0;
    qreal rightTop = 0.0;
    qreal rightBottom = 0.0;
    qreal leftTop = 0.0;
    qreal leftBottom = 0.0;

    int space = 200;

    auto minMoveLen = [=](qreal temp, qreal &len) {
        if (fabs(len) > 0.0) {
            if (fabs(temp) < fabs(len))
                len = temp;
        } else {
            len = temp;
        }
    };
    QRectF pwRect(m_movingItem->rect());
    for (auto item : m_listItems) {
        if (item == m_movingItem)
            continue;
        QRectF itemRect(item->rect());
        if (pwRect.intersects(itemRect))
            return;

        // 与boundingRect 不相交
        if (pwRect.intersects(QRectF(itemRect.topLeft(), itemRect.bottomRight()).adjusted(0, -space, 0, 0))) {
            // 上相交
            qreal temp = itemRect.topRight().y() - pwRect.bottomLeft().y();
            minMoveLen(temp, top);
            // 判断边对齐
            temp = itemRect.topRight().x() - pwRect.bottomRight().x();
            minMoveLen(temp, topLeft);
            temp = itemRect.topLeft().x() - pwRect.bottomLeft().x();
            minMoveLen(temp, topRight);
        } else if (pwRect.intersects(QRectF(itemRect.bottomLeft(), itemRect.bottomRight()).adjusted(0, 0, 0, space))) {
            // 下相交
            qreal temp = itemRect.bottomLeft().y() - pwRect.topLeft().y();
            minMoveLen(temp, bottom);
            // 判断边对齐的可能
            temp = itemRect.topRight().x() - pwRect.bottomRight().x();
            minMoveLen(temp, bottomLeft);
            temp = itemRect.topLeft().x() - pwRect.bottomLeft().x();
            minMoveLen(temp, bottomRight);
        } else if (pwRect.intersects(QRectF(itemRect.topLeft(), itemRect.bottomLeft()).adjusted(-space, 0, 0, 0))) {
            // 左相交
            qreal temp = itemRect.bottomLeft().x() - pwRect.bottomRight().x();
            minMoveLen(temp, left);
            // 判断边对齐的可能
            temp = itemRect.topRight().y() - pwRect.topRight().y();
            minMoveLen(temp, leftTop);
            temp = itemRect.bottomLeft().y() - pwRect.bottomLeft().y();
            minMoveLen(temp, leftBottom);
        } else if (pwRect.intersects(QRectF(itemRect.topRight(), itemRect.bottomRight()).adjusted(0, 0, space, 0))) {
            // 右相交
            qreal temp = itemRect.bottomRight().x() - pwRect.bottomLeft().x();
            minMoveLen(temp, right);
            // 判断边对齐的可能
            temp = itemRect.topRight().y() - pwRect.topRight().y();
            minMoveLen(temp, rightTop);
            temp = itemRect.bottomLeft().y() - pwRect.bottomLeft().y();
            minMoveLen(temp, rightBottom);
        }
    }
    auto edgeAlignment = [=](qreal x1, qreal x2) {
        if (fabs(x1) > fabs(x2))
            return fabs(x2) < space ? x2 : 0;
        else
            return fabs(x1) < space ? x1 : 0;
    };

    QPointF autoAdsorptionPos(0.0, 0.0), edgeAlignmentPos(0.0, 0.0);

    if (qFuzzyIsNull(top)) {
        edgeAlignmentPos.setX(edgeAlignment(bottomRight, bottomLeft));
        autoAdsorptionPos.setY(bottom);
    } else if (qFuzzyIsNull(bottom)) {
        edgeAlignmentPos.setX(edgeAlignment(topRight, topLeft));
        autoAdsorptionPos.setY(top);
    } else {
        autoAdsorptionPos.setY((fabs(top) < fabs(bottom)) ? top : bottom);
        edgeAlignmentPos.setX((fabs(top) < fabs(bottom)) ? edgeAlignment(topRight, topLeft) : edgeAlignment(bottomRight, bottomLeft));
    }
    if (qFuzzyIsNull(left)) {
        autoAdsorptionPos.setX(right);
        edgeAlignmentPos.setY(edgeAlignment(rightTop, rightBottom));
    } else if (qFuzzyIsNull(right)) {
        autoAdsorptionPos.setX(left);
        edgeAlignmentPos.setY(edgeAlignment(leftTop, leftBottom));
    } else {
        autoAdsorptionPos.setX((fabs(left) < fabs(right)) ? left : right);
        edgeAlignmentPos.setY((fabs(left) < fabs(right)) ? edgeAlignment(leftTop, leftBottom) : edgeAlignment(rightTop, rightBottom));
    }
    if (!qFuzzyIsNull(autoAdsorptionPos.x())) {
        edgeAlignmentPos.setX(0.0);
    }
    if (!qFuzzyIsNull(autoAdsorptionPos.y())) {
        edgeAlignmentPos.setY(0.0);
    }

    m_movingItem->moveBy(edgeAlignmentPos.x() + autoAdsorptionPos.x(), edgeAlignmentPos.y() + autoAdsorptionPos.y());
}

void ConcatScreen::executemultiScreenAlgo(bool isRebound)
{
    bool isRestore = false;
    if (multiScreenSortAlgo(isRestore, isRebound)) {
        multiScreenSortAlgo(isRestore, true);
    }

    if (isRestore == true)
        return;
    multiScreenAutoAdjust();
    updateConnectedState();
}

bool ConcatScreen::multiScreenSortAlgo(bool &isRestore, const bool isRebound)
{
    // 排列算法
    // 1、寻找位置变化的屏幕块，确定为被移动的块
    // 2、判断被移动块和其他块之间的位置关系来进行图形拼接移动
    //*根据中心点判定
    QList<ScreenData *> lstActivedItemsX;
    QList<ScreenData *> lstActivedItemsY;
    QList<ScreenData *> lstNoMovedItems;
    QList<ScreenData *> lstShelterItems;

    QList<QPair<ScreenData *, qreal>> lstMoveingItemToCenterPosLen; // 所有块的中心点到移动点的距离
    bool isMove = true;
    isRestore = false;
    bool isAutoAdsorption = false;
    bool isIntersect = false;
    qreal g_dx = 0.0;
    qreal g_dy = 0.0;

    qreal intersectedArea = 0.0; // 相交的面积

    QRectF moveItemIntersect, moveItemRect;

    bool g_bXYTogetherMoved = false; // 标志XY方向是否一起移动，其余情况按哪个方向离得近向哪个方向移动

    auto mapToSceneIntersectRect = [](ScreenData *item) {
        return item->rect().adjusted(1, 1, -1, -1);
    };

    auto mapToSceneBoundingRect = [](ScreenData *item) {
        return item->rect();
    };

    moveItemIntersect = mapToSceneIntersectRect(m_movingItem);
    moveItemRect = mapToSceneBoundingRect(m_movingItem);
    for (auto &&item : m_listItems) {
        if (m_movingItem != item) {
            QRectF rect = moveItemIntersect.intersected(mapToSceneBoundingRect(item));
            intersectedArea += rect.width() * rect.height();
            // 移动块完全覆盖一个块 2、移动块与另外一个块十字相交时 执行自动回弹操作
            if (moveItemRect.contains(mapToSceneBoundingRect(item))
                || (rect.top() < moveItemIntersect.top() && rect.bottom() > moveItemIntersect.bottom() && qFuzzyCompare(rect.left(), moveItemIntersect.left()) && qFuzzyCompare(rect.right(), moveItemIntersect.right()))
                || (rect.right() < moveItemIntersect.right() && rect.left() > moveItemIntersect.left() && qFuzzyCompare(rect.top(), moveItemIntersect.top()) && qFuzzyCompare(rect.bottom(), moveItemIntersect.bottom()))) {
                lstShelterItems.append(item);
            }

            // 要么不相交,要相交就是要点线相交的
            if (!mapToSceneIntersectRect(m_movingItem).intersects(mapToSceneBoundingRect(item)) && mapToSceneBoundingRect(m_movingItem).intersects(mapToSceneBoundingRect(item))) {
                isAutoAdsorption = true;
            }
            // 出现相交的情况
            if (mapToSceneIntersectRect(m_movingItem).intersects(mapToSceneBoundingRect(item)) && mapToSceneBoundingRect(m_movingItem).intersects(mapToSceneBoundingRect(item))) {
                isIntersect = true;
            }

            lstNoMovedItems.append(item);
        }
    }
    // 移动块被完全包含在其他的块
    if (qFuzzyCompare(moveItemIntersect.width() * moveItemIntersect.height(), intersectedArea)) {
        qDebug() << "存在包含关系";
        lstShelterItems.append(m_movingItem);
    }
    // 移动块到其他块中心点的距离排序
    QPointF movedPos = moveItemRect.center();
    for (auto item : lstNoMovedItems) {
        QPointF tempPos = mapToSceneBoundingRect(item).center();
        qreal len = sqrt(pow(tempPos.x() - movedPos.x(), 2) + pow(tempPos.y() - movedPos.y(), 2));
        lstMoveingItemToCenterPosLen.append(qMakePair(item, len));
    }
    // 获取距离最近的块
    std::sort(lstMoveingItemToCenterPosLen.begin(), lstMoveingItemToCenterPosLen.end(), [=](const QPair<ScreenData *, qreal> &item1, const QPair<ScreenData *, qreal> &item2) {
        return item1.second < item2.second;
    });
    // 自动回弹的触发条件: 1. 一个屏幕完全包含另一个屏的时候 2. 一个屏幕剩下的屏幕集合所包围
    if (lstShelterItems.size() > 0) {
        if (isMove) {
            if (isRebound) {
                autoRebound();
                qDebug() << "自动回弹流程触发! == " << lstShelterItems.size();
                isRestore = true;
                // return QPointF(0.0, 0.0);
                return false;
            } else {
                // 当改变方向时出现覆盖多个块的情况，会触发自动会弹流程，但是改变方向的上一个操作状态不存在或者说回弹到上一个状态没有意义，会导致屏幕重叠现象
                // 如果是由于方向改变导致的重叠，那就将此块移动到items外接矩形的左下角再重新执行拼接算法。
                m_movingItem->moveBy(-10000, 10000);
                return true;
            }
        }
    } else { // 没有完全重合的情况处理逻辑
        std::sort(lstNoMovedItems.begin(), lstNoMovedItems.end(), [=](const ScreenData *item1, const ScreenData *item2) {
            return item1->rect().bottomLeft().x() < item2->rect().bottomLeft().x();
        });

        // 将MovedItem放入链表中排序，左排 右排，判定覆盖几个块
        lstNoMovedItems.append(m_movingItem);
        // 左排序
        std::sort(lstNoMovedItems.begin(), lstNoMovedItems.end(), [=](const ScreenData *item1, const ScreenData *item2) {
            if (item1 == m_movingItem) {
                return item1->rect().bottomRight().x() < item2->rect().bottomLeft().x();
            } else if (item2 == m_movingItem) {
                return item1->rect().bottomLeft().x() < item2->rect().bottomRight().x();
            }
            return item1->rect().bottomLeft().x() < item2->rect().bottomLeft().x();
        });
        int nBIndexLeft = lstNoMovedItems.indexOf(m_movingItem);
        // 右排序
        std::sort(lstNoMovedItems.begin(), lstNoMovedItems.end(), [=](const ScreenData *item1, const ScreenData *item2) {
            if (item1 == m_movingItem) {
                return item1->rect().bottomLeft().x() < item2->rect().bottomRight().x();
            } else if (item2 == m_movingItem) {
                return item1->rect().bottomRight().x() < item2->rect().bottomLeft().x();
            }
            return item1->rect().bottomRight().x() < item2->rect().bottomRight().x();
        });
        int nBIndexRight = lstNoMovedItems.indexOf(m_movingItem);

        // 上排序
        std::sort(lstNoMovedItems.begin(), lstNoMovedItems.end(), [=](const ScreenData *item1, const ScreenData *item2) {
            if (item1 == m_movingItem) {
                return item1->rect().bottomLeft().y() < item2->rect().topLeft().y();
            } else if (item2 == m_movingItem) {
                return item1->rect().topLeft().y() < item2->rect().bottomLeft().y();
            }
            return item1->rect().topLeft().y() < item2->rect().topLeft().y();
        });
        int nBIndexTop = lstNoMovedItems.indexOf(m_movingItem);

        // 下排序
        std::sort(lstNoMovedItems.begin(), lstNoMovedItems.end(), [=](const ScreenData *item1, const ScreenData *item2) {
            if (item1 == m_movingItem) {
                return item1->rect().topLeft().y() < item2->rect().bottomLeft().y();
            } else if (item2 == m_movingItem) {
                return item1->rect().bottomLeft().y() < item2->rect().topLeft().y();
            }
            return item1->rect().bottomLeft().y() < item2->rect().bottomLeft().y();
        });
        int nBIndexBottom = lstNoMovedItems.indexOf(m_movingItem);

        int nItemSize = lstNoMovedItems.size() - 1;

        // 说明移动块在其他块X坐标有重合
        if (nBIndexLeft != 0 && nBIndexRight != nItemSize) {

            // 移动X
            // 将MovedItem放入链表中排序，上排 下排，判定在Y轴移动的方向和距离
            // 处理
            qreal movex1 = m_movingItem->rect().topLeft().x();
            qreal movex2 = m_movingItem->rect().topRight().x();

            for (auto pair : lstMoveingItemToCenterPosLen) {
                qreal x1 = pair.first->rect().topLeft().x();
                qreal x2 = pair.first->rect().topRight().x();

                if (!((x1 >= movex1 && x1 >= movex2) || (x2 <= movex1 && x2 <= movex2))) {
                    lstActivedItemsX.append(pair.first);
                }
            }

            lstActivedItemsX.append(m_movingItem);
            // 上排序
            std::sort(lstActivedItemsX.begin(), lstActivedItemsX.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                return item1->rect().topLeft().y() < item2->rect().topLeft().y();
            });
            int nIndexTop = lstActivedItemsX.indexOf(m_movingItem);

            // 下排序
            std::sort(lstActivedItemsX.begin(), lstActivedItemsX.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                return item1->rect().bottomLeft().y() < item2->rect().bottomLeft().y();
            });
            int nIndexBottom = lstActivedItemsX.indexOf(m_movingItem);

            // 先做预移动，如果有重合，把重合块加入激活块重新排序
            if (nIndexTop == nIndexBottom) {
                // 移动块在最上面
                if (nIndexTop == 0) {
                    qreal dy = lstActivedItemsX.first()->rect().topLeft().y() - m_movingItem->rect().bottomLeft().y();
                    m_movingItem->moveBy(0, dy);
                    // m_graphicsScene.update();

                    QList<ScreenData *> lstShelterItemsTemp;
                    lstNoMovedItems.removeOne(m_movingItem);
                    for (int i = 0; i < lstNoMovedItems.size(); i++) {
                        if (m_movingItem->rect().intersects(lstNoMovedItems[i]->rect())) {
                            lstShelterItemsTemp.append(lstNoMovedItems[i]);
                        }
                    }

                    if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(0, -dy); // 先移回去
                        // m_graphicsScene.update();

                        // 下排序
                        std::sort(lstActivedItemsX.begin(), lstActivedItemsX.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                            return item1->rect().bottomLeft().y() < item2->rect().bottomLeft().y();
                        });
                        // int nIndexBottomTemp = lstActivedItemsX.indexOf(m_movingItem);

                        // 上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsX.contains(t))
                                lstActivedItemsX.append(t);
                        }
                        std::sort(lstActivedItemsX.begin(), lstActivedItemsX.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                            return item1->rect().topLeft().y() < item2->rect().topLeft().y();
                        });
                        int nIndexTopTemp = lstActivedItemsX.indexOf(m_movingItem);

                        lstActivedItemsX.removeOne(m_movingItem);

                        if (!lstActivedItemsX.isEmpty()) {
                            if (nIndexTopTemp == 0) {
                                g_dy = lstActivedItemsX.first()->rect().topLeft().y() - m_movingItem->rect().bottomLeft().y();
                                qDebug() << "g_dy" << g_dy;
                            } else {
                                g_dy = lstActivedItemsX.last()->rect().bottomLeft().y() - m_movingItem->rect().topLeft().y();
                                qDebug() << "g_dy" << g_dy;
                            }
                        }
                    }
                }
                // 移动块在最下面
                else {
                    if (!lstActivedItemsX.contains(m_movingItem))
                        lstActivedItemsX.append(m_movingItem);
                    // 下排序
                    std::sort(lstActivedItemsX.begin(), lstActivedItemsX.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                        return item1->rect().bottomLeft().y() < item2->rect().bottomLeft().y();
                    });

                    qreal dy = lstActivedItemsX.last()->rect().bottomLeft().y() - m_movingItem->rect().topLeft().y();
                    m_movingItem->moveBy(0, dy);
                    // m_graphicsScene.update();

                    lstNoMovedItems.removeOne(m_movingItem);
                    QList<ScreenData *> lstShelterItemsTemp;
                    for (int i = 0; i < lstNoMovedItems.size(); i++) {
                        if (m_movingItem->rect().intersects(lstNoMovedItems[i]->rect())) {
                            lstShelterItemsTemp.append(lstNoMovedItems[i]);
                        }
                    }

                    if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(0, -dy); // 先移回去
                        // m_graphicsScene.update();

                        // 上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsX.contains(t))
                                lstActivedItemsX.append(t);
                        }

                        std::sort(lstActivedItemsX.begin(), lstActivedItemsX.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                            return item1->rect().topLeft().y() < item2->rect().topLeft().y();
                        });
                        int nIndexTopTemp = lstActivedItemsX.indexOf(m_movingItem);

                        // 下排序
                        std::sort(lstActivedItemsX.begin(), lstActivedItemsX.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                            return item1->rect().bottomLeft().y() < item2->rect().bottomLeft().y();
                        });
                        // int nIndexBottomTemp = lstActivedItemsX.indexOf(m_movingItem);

                        lstActivedItemsX.removeOne(m_movingItem);

                        if (!lstActivedItemsX.isEmpty()) {
                            if (nIndexTopTemp == 0) {
                                g_dy = lstActivedItemsX.first()->rect().topLeft().y() - m_movingItem->rect().bottomLeft().y();
                                qDebug() << "g_dy" << g_dy;
                            } else {
                                g_dy = lstActivedItemsX.last()->rect().bottomLeft().y() - m_movingItem->rect().topLeft().y();
                                qDebug() << "g_dy" << g_dy;
                            }
                        }
                    }
                }
            }
        }

        // 说明移动块在其他块Y坐标有重合
        if (nBIndexTop != 0 && nBIndexBottom != nItemSize) {

            // 移动Y
            // 处理
            qreal movey1 = m_movingItem->rect().topLeft().y();
            qreal movey2 = m_movingItem->rect().bottomLeft().y();

            for (auto pair : lstMoveingItemToCenterPosLen) {
                qreal y1 = pair.first->rect().topLeft().y();
                qreal y2 = pair.first->rect().bottomLeft().y();

                if (!((y1 >= movey1 && y1 >= movey2) || (y2 <= movey1 && y2 <= movey2))) {
                    lstActivedItemsY.append(pair.first);
                }
            }

            lstActivedItemsY.append(m_movingItem);
            // 左排序
            std::sort(lstActivedItemsY.begin(), lstActivedItemsY.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                return item1->rect().topLeft().x() < item2->rect().topLeft().x();
            });
            int nIndexLeft = lstActivedItemsY.indexOf(m_movingItem);

            // 右排序
            std::sort(lstActivedItemsY.begin(), lstActivedItemsY.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                return item1->rect().topRight().x() < item2->rect().topRight().x();
            });
            int nIndexRight = lstActivedItemsY.indexOf(m_movingItem);

            if (nIndexLeft == nIndexRight) {
                // 移动块在最右侧
                if (nIndexLeft == 0) {
                    qreal dx = lstActivedItemsY.first()->rect().topLeft().x() - m_movingItem->rect().topRight().x();
                    m_movingItem->moveBy(dx, 0);
                    // m_graphicsScene.update();

                    QList<ScreenData *> lstShelterItemsTemp;
                    lstNoMovedItems.removeOne(m_movingItem);
                    for (int i = 0; i < lstNoMovedItems.size(); i++) {
                        if (m_movingItem->rect().intersects(lstNoMovedItems[i]->rect())) {
                            lstShelterItemsTemp.append(lstNoMovedItems[i]);
                        }
                    }

                    if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(-dx, 0);
                        // m_graphicsScene.update();

                        // 下排序
                        std::sort(lstActivedItemsY.begin(), lstActivedItemsY.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                            return item1->rect().topRight().x() < item2->rect().topRight().x();
                        });
                        // int nIndexRightTemp = lstActivedItemsY.indexOf(m_movingItem);

                        // 上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsY.contains(t))
                                lstActivedItemsY.append(t);
                        }
                        std::sort(lstActivedItemsY.begin(), lstActivedItemsY.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                            return item1->rect().topLeft().x() < item2->rect().topLeft().x();
                        });
                        int nIndexLeftTemp = lstActivedItemsY.indexOf(m_movingItem);

                        lstActivedItemsY.removeOne(m_movingItem);

                        if (!lstActivedItemsY.isEmpty()) {
                            if (nIndexLeftTemp == 0) {
                                g_dx = lstActivedItemsY.first()->rect().topLeft().x() - m_movingItem->rect().topRight().x();
                                qDebug() << "g_dx" << g_dx;
                            } else {
                                g_dx = lstActivedItemsY.last()->rect().topRight().x() - m_movingItem->rect().topLeft().x();
                                qDebug() << "g_dx" << g_dx;
                            }
                        }
                    }
                }
                // 移动块在最左侧
                else {
                    if (!lstActivedItemsY.contains(m_movingItem))
                        lstActivedItemsY.append(m_movingItem);
                    // 右排序
                    std::sort(lstActivedItemsY.begin(), lstActivedItemsY.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                        return item1->rect().topRight().x() < item2->rect().topRight().x();
                    });

                    qreal dx = lstActivedItemsY.last()->rect().topRight().x() - m_movingItem->rect().topLeft().x();
                    m_movingItem->moveBy(dx, 0);
                    // m_graphicsScene.update();

                    lstNoMovedItems.removeOne(m_movingItem);
                    QList<ScreenData *> lstShelterItemsTemp;
                    for (int i = 0; i < lstNoMovedItems.size(); i++) {
                        if (m_movingItem->rect().intersects(lstNoMovedItems[i]->rect())) {
                            lstShelterItemsTemp.append(lstNoMovedItems[i]);
                        }
                    }

                    if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(-dx, 0);
                        // m_graphicsScene.update();

                        // 上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsY.contains(t))
                                lstActivedItemsY.append(t);
                        }
                        std::sort(lstActivedItemsY.begin(), lstActivedItemsY.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                            return item1->rect().topLeft().x() < item2->rect().topLeft().x();
                        });
                        int nIndexLeftTemp = lstActivedItemsY.indexOf(m_movingItem);

                        // 下排序
                        std::sort(lstActivedItemsY.begin(), lstActivedItemsY.end(), [=](const ScreenData *item1, const ScreenData *item2) {
                            return item1->rect().topRight().x() < item2->rect().topRight().x();
                        });
                        // int nIndexRightTemp = lstActivedItemsY.indexOf(m_movingItem);

                        lstActivedItemsY.removeOne(m_movingItem);

                        if (!lstActivedItemsY.isEmpty()) {
                            if (nIndexLeftTemp == 0) {
                                g_dx = lstActivedItemsY.first()->rect().topLeft().x() - m_movingItem->rect().topRight().x();
                                qDebug() << "g_dx" << g_dx;
                            } else {
                                g_dx = lstActivedItemsY.last()->rect().topRight().x() - m_movingItem->rect().topLeft().x();
                                qDebug() << "g_dx" << g_dx;
                            }
                        }
                    }
                }
            }
        }
        // 说明移动块在其他块的四周
        if ((nBIndexLeft == 0 || nBIndexRight == nItemSize) || (nBIndexTop == 0 || nBIndexBottom == nItemSize)) {
            // 表示X方向没有重叠的
            // TODO：
            // 找距离他最近的顶点
            // LT 左上
            lstNoMovedItems.removeOne(m_movingItem);
            QRectF movedRectF = m_movingItem->rect();
            QList<QPair<ScreenData *, qreal>> lstTempLen;
            if (nBIndexLeft == 0 && nBIndexTop == 0) {
                for (auto item : lstNoMovedItems) {
                    QPointF tempPos = item->rect().topLeft();
                    qreal len = sqrt(pow(tempPos.x() - movedRectF.bottomRight().x(), 2) + pow(tempPos.y() - movedRectF.bottomRight().y(), 2));
                    lstTempLen.append(qMakePair(item, len));
                }
                // 获取距离最近的块
                std::sort(lstTempLen.begin(), lstTempLen.end(), [=](const QPair<ScreenData *, qreal> &item1, const QPair<ScreenData *, qreal> &item2) {
                    return item1.second < item2.second;
                });

                QPointF dPos = lstTempLen.first().first->rect().topLeft() - m_movingItem->rect().bottomRight();
                g_dx = dPos.x();
                g_dy = dPos.y();
                qDebug() << "g_dx" << g_dx;
                qDebug() << "g_dy" << g_dy;
                g_bXYTogetherMoved = true;
            }

            // LB 左下
            if (nBIndexLeft == 0 && nBIndexBottom == nItemSize) {
                for (auto item : lstNoMovedItems) {
                    QPointF tempPos = item->rect().bottomLeft();
                    qreal len = sqrt(pow(tempPos.x() - movedRectF.topRight().x(), 2) + pow(tempPos.y() - movedRectF.topRight().y(), 2));
                    lstTempLen.append(qMakePair(item, len));
                }
                // 获取距离最近的块
                std::sort(lstTempLen.begin(), lstTempLen.end(), [=](const QPair<ScreenData *, qreal> &item1, const QPair<ScreenData *, qreal> &item2) {
                    return item1.second < item2.second;
                });

                QPointF dPos = lstTempLen.first().first->rect().bottomLeft() - m_movingItem->rect().topRight();
                g_dx = dPos.x();
                g_dy = dPos.y();
                g_bXYTogetherMoved = true;
                qDebug() << "g_dx" << g_dx;
                qDebug() << "g_dy" << g_dy;
            }

            // RT 右上
            if (nBIndexRight == nItemSize && nBIndexTop == 0) {
                for (auto item : lstNoMovedItems) {
                    QPointF tempPos = item->rect().topRight();
                    qreal len = sqrt(pow(tempPos.x() - movedRectF.bottomLeft().x(), 2) + pow(tempPos.y() - movedRectF.bottomLeft().y(), 2));
                    lstTempLen.append(qMakePair(item, len));
                }
                // 获取距离最近的块
                std::sort(lstTempLen.begin(), lstTempLen.end(), [=](const QPair<ScreenData *, qreal> &item1, const QPair<ScreenData *, qreal> &item2) {
                    return item1.second < item2.second;
                });

                QPointF dPos = lstTempLen.first().first->rect().topRight() - m_movingItem->rect().bottomLeft();
                g_dx = dPos.x();
                g_dy = dPos.y();
                g_bXYTogetherMoved = true;
                qDebug() << "g_dx" << g_dx;
                qDebug() << "g_dy" << g_dy;
            }

            // RB 右下

            if (nBIndexRight == nItemSize && nBIndexBottom == nItemSize) {
                for (auto item : lstNoMovedItems) {
                    QPointF tempPos = item->rect().bottomRight();
                    qreal len = sqrt(pow(tempPos.x() - movedRectF.topLeft().x(), 2) + pow(tempPos.y() - movedRectF.topLeft().y(), 2));
                    lstTempLen.append(qMakePair(item, len));
                }
                // 获取距离最近的块
                std::sort(lstTempLen.begin(), lstTempLen.end(), [=](const QPair<ScreenData *, qreal> &item1, const QPair<ScreenData *, qreal> &item2) {
                    return item1.second < item2.second;
                });

                QPointF dPos = lstTempLen.first().first->rect().bottomRight() - m_movingItem->rect().topLeft();
                g_dx = dPos.x();
                g_dy = dPos.y();
                qDebug() << "g_dx" << g_dx;
                qDebug() << "g_dy" << g_dy;
                g_bXYTogetherMoved = true;
            }
        }
    }

    qreal dx = g_dx, dy = g_dy;
    // 是自动吸附并且没有相交的情况，不需要要移动
    if (isAutoAdsorption && !isIntersect) {
        if (isMove)
            m_movingItem->moveBy(0, 0);
        else {
            dx = 0.0;
            dy = 0.0;
        }
    }
    // 其他情况需要移动
    else {
        // 是否XY一起移动
        if (g_bXYTogetherMoved) {
            if (isMove)
                m_movingItem->moveBy(g_dx, g_dy);
            qDebug() << "XY_XY_XY" << g_dx << g_dy;
        } else {
            // XY其中一个为0的情况， 执行移动
            if (qFuzzyIsNull(g_dx) || qFuzzyIsNull(g_dy)) {
                if (isMove)
                    m_movingItem->moveBy(g_dx, g_dy);

                qDebug() << "DBL_MIN" << g_dx << g_dy;
            }
            // X和Y都不为0的情况下，哪个移动的绝对值小移动哪一个
            else {
                if (isMove)
                    fabs(g_dx) < fabs(g_dy) ? m_movingItem->moveBy(g_dx, 0) : m_movingItem->moveBy(0, g_dy);
                else {
                    fabs(g_dx) < fabs(g_dy) ? dy = 0.0 : dx = 0.0;
                }
                qDebug() << "fabs(g_dx) < fabs(g_dy) ?" << g_dx << g_dy;
            }
        }
    }
    return false;
}

void ConcatScreen::multiScreenAutoAdjust()
{
    // 在自动拼接之后已经达成全连通状态,无效执行自动调整
    updateConnectedState();

    // 判断是否为全连通状态
    if (getConnectedDomain(m_movingItem).size() == m_listItems.size()) {
        updateConnectedState(true);
        return;
    }

    // 存储移动块到临时变量
    ScreenData *pwTemp = m_movingItem;
    QList<ScreenData *> lstChangedItems = m_listItems;

    // 处理之前变化的块为空则返回
    if (lstChangedItems.isEmpty())
        return;

    QMap<ScreenData *, QList<ScreenData *>> maplstItems;
    QMap<ScreenData *, bool> mapItemsNeedMove;
    QList<ScreenData *> itemTemp;

    // 这个列表是存放的就是所有移动块相关的块
    // 判断改变连通的块与移动块的剩余联通块是否存在连接
    // 【这种是在移动块初始连接块是两个及以上的情况下触发的，如果是一个连通块的话不会改变连通状态的】

    // 获取屏幕集群
    for (auto iter : m_listItems) {

        if (!lstChangedItems.contains(iter))
            continue;

        itemTemp = getConnectedDomain(iter);
        for (auto k : itemTemp) {
            if (lstChangedItems.contains(k) && k != iter) {
                lstChangedItems.removeOne(k);
            }
        }
    }

    for (auto item : lstChangedItems) {
        qDebug() << "处理之后变化的块:" << item->rect() << item;
    }

    for (auto iter : lstChangedItems) {
        maplstItems.insert(iter, getConnectedDomain(iter));
    }

    for (auto iter : maplstItems.keys()) {
        qDebug() << "屏幕集群:" << iter->rect() << iter;
        for (auto k : maplstItems[iter]) {
            qDebug() << "val" << k->rect() << k;
        }
    }

    if (maplstItems.size() > 0) {
        // 说明除了移动块剩下的被拆分成了几部分，需要重新向其中一个部分移动了
        // 找到中心屏幕集群(包含移动块)
        for (auto iter1 = maplstItems.begin(); iter1 != maplstItems.end(); ++iter1) {
            if (iter1.value().contains(pwTemp)) {
                m_lstSortItems = iter1.value();
                break;
            }
        }

        // 其他屏幕集群依次向中心屏幕集群移动
        for (auto iter = maplstItems.begin(); iter != maplstItems.end(); ++iter) {
            if (iter.value().contains(pwTemp))
                continue;

            QList<QPointF> lstPos;
            QList<qreal> lstX, lstY;
            for (auto item : iter.value()) {
                m_movingItem = item;
                bool isRestore = false;
                multiScreenSortAlgo(isRestore);

                if (!m_lstSortItems.contains(item))
                    m_lstSortItems.append(item);
            }
        }
    }
    m_lstSortItems = m_listItems;
    m_movingItem = pwTemp;

    // 自动调整完毕后,更新连通域
    updateConnectedState(true);
}

bool ConcatScreen::updateConnectedState(bool isInit)
{
    bool isIntersect = false;
    QList<ScreenData *> listItemsTemp;
    for (int i = 0; i < m_listItems.size(); i++) {
        listItemsTemp.clear();

        for (int j = 0; j < m_listItems.size(); j++) {
            if (j != i && m_listItems[i]->rectEx().intersects(m_listItems[j]->rect()) && !listItemsTemp.contains(m_listItems[j]) && !m_listItems[i]->justIntersectRect().intersects(m_listItems[j]->rect())) {
                listItemsTemp.append(m_listItems[j]);
            }
            if (j != i && m_listItems[i]->justIntersectRect().intersects(m_listItems[j]->rect())) {
                isIntersect = true;
            }
        }

        if (isInit) {
            m_mapInitItemConnectedState.insert(m_listItems[i], listItemsTemp);
        }

        m_mapItemConnectedState.insert(m_listItems[i], listItemsTemp);
    }

    return isIntersect;
}

// 获取连通域
QList<ScreenData *> ConcatScreen::getConnectedDomain(ScreenData *item)
{
    QList<ScreenData *> lstTemp1Items;

    QList<ScreenData *> lstTemp;
    QList<ScreenData *> lstItems;

    lstItems.append(item);

    for (auto iter : m_listItems) /*标记循环次数*/ {
        Q_UNUSED(iter)

        lstTemp.clear();
        // 获取所有的value
        for (auto a : lstItems) {
            for (auto t : m_mapItemConnectedState[a]) {
                if (!lstTemp1Items.contains(t))
                    lstTemp.append(t);
            }
        }

        // 去重插入
        for (auto t : lstTemp) {
            if (!lstTemp1Items.contains(t) && t != item)
                lstTemp1Items.append(t);
            else {
                lstTemp.removeOne(t);
            }
        }

        lstItems.clear();
        lstItems.append(lstTemp);
    }

    lstTemp1Items.append(item);

    return lstTemp1Items;
}

void ConcatScreen::autoRebound()
{
    for (auto &&item : m_listItems) {
        item->rebound();
    }
}

} // namespace dccV25
