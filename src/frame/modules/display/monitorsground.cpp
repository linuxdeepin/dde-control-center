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

#include "monitorsground.h"
#include "monitorproxywidget.h"
#include "displaymodel.h"

#include <math.h>
#include <float.h>

#include <QScroller>
#include <QScrollArea>

using namespace dcc::display;
const int MIN_W = 484; //窗口的最小宽度
const int MAX_W = 726; //窗口的最大宽度

MonitorsGround::MonitorsGround(int activateHeight, QWidget *parent)
    : DGraphicsView(parent)
    , m_scrollArea(nullptr)
    , m_refershTimer(new QTimer(this))
    , m_effectiveTimer(new QTimer(this))
    , m_isSingleDisplay(false)
    , m_scale(0.1)
    , m_isInit(false)
    , m_nEffectiveTime(2)
    , m_setMergeMode(false)

{
    setFixedHeight(activateHeight);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 隐藏水平/竖直滚动条
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 反锯齿
    setRenderHints(QPainter::Antialiasing);

    //自动填色和无边框
    setAutoFillBackground(true);
    setStyleSheet("border:0px");
    DPalette pa = DApplicationHelper::instance()->palette(parentWidget());
    QColor curThemeColor = pa.base().color();
    curThemeColor.setAlphaF(0.001);  // 设置透明度
    pa.setBrush(QPalette::Base, QBrush(curThemeColor));
    DApplicationHelper::instance()->setPalette(this, pa);

    //添加场景
    setScene(&m_graphicsScene);

    Q_EMIT setEffectiveReminderVisible(false, m_nEffectiveTime);

    m_effectiveTimer->setInterval(m_nEffectiveTime*1000);
    m_effectiveTimer->setSingleShot(true);
    connect(m_effectiveTimer, &QTimer::timeout, this, [=]() {
        Q_EMIT setEffectiveReminderVisible(false, m_nEffectiveTime);
        for (auto mon : m_model->monitorList()) {
            disconnect(mon, &Monitor::geometryChanged, this, &MonitorsGround::onGeometryChanged);
        }

        applySettings();
        Q_EMIT requestMonitorRelease(m_monitors[m_movingItem]);
    });
}

MonitorsGround::~MonitorsGround()
{
    qDeleteAll(m_monitors.keys());
}

void MonitorsGround::setModel(DisplayModel *model, Monitor *moni)
{
    m_setMergeMode = (model->displayMode() == MERGE_MODE);

    Q_EMIT setEffectiveReminderVisible(false, m_nEffectiveTime);
    m_effectiveTimer->stop();
    qDeleteAll(m_monitors.keys());
    m_monitors.clear();
    m_lstItems.clear();
    m_lstSortItems.clear();
    m_graphicsScene.clear();
    m_model = model;

    if (!moni) {
        m_isSingleDisplay = false;
        for (auto mon : model->monitorList()) {
            initMonitorProxyWidget(mon);
        }

        for (auto pw : m_monitors.keys()) {
            if(!m_isSingleDisplay)
                pw->moveBy(pw->x(),pw->y());
        }

        for (auto mon : model->monitorList()) {
            connect(mon, &Monitor::wChanged, this, &MonitorsGround::onRotateChanged);
            connect(mon, &Monitor::hChanged, this, &MonitorsGround::onRotateChanged);
            connect(mon, &Monitor::geometryChanged, this, &MonitorsGround::onGeometryChanged);
        }

        connect(m_model, &DisplayModel::primaryScreenChanged, this, static_cast<void (MonitorsGround::*)()>(&MonitorsGround::update), Qt::QueuedConnection);
    } else {
        initMonitorProxyWidget(moni);
        connect(moni, &Monitor::geometryChanged, this, &MonitorsGround::onGeometryChanged);
        connect(moni, &Monitor::wChanged, this, &MonitorsGround::onRotateChanged);
        connect(moni, &Monitor::hChanged, this, &MonitorsGround::onRotateChanged);
        m_isSingleDisplay = true;
    }

    QTimer::singleShot(1, this, [=](){
        if (!m_isSingleDisplay) {
            updateScale();
        }    
        resetMonitorsView();
    });
}


void MonitorsGround::initMonitorProxyWidget(Monitor *mon)
{
    MonitorProxyWidget *pw = new MonitorProxyWidget(mon, m_model);

    m_graphicsScene.addItem(pw);
    m_monitors[pw] = mon;
    m_lstItems.append(pw);
    m_lstSortItems.append(pw);

    connect(pw, &MonitorProxyWidget::requestMonitorPress, this, &MonitorsGround::requestMonitorPress);
    connect(pw, &MonitorProxyWidget::requestMonitorPress, this, [this,pw]() {
        m_movingItem = pw;
    });
    connect(pw, &MonitorProxyWidget::requestMonitorRelease, this, &MonitorsGround::onRequestMonitorRelease);
    connect(pw, &MonitorProxyWidget::requestMouseMove, this, &MonitorsGround::onRequestMouseMove);
    connect(pw, &MonitorProxyWidget::requestKeyPress, this, &MonitorsGround::onRequestKeyPress);
}


void MonitorsGround::onGeometryChanged()
{
    for (auto pw : m_monitors.keys()) {
        Monitor *mon = qobject_cast<Monitor*>(sender());
        if (mon->name() == m_monitors[pw]->name()) {
            m_movingItem = pw;
            pw->update();
            break;
        }
    }

    for (auto pw : m_monitors.keys()) {
        pw->setMovedX(m_monitors[pw]->x());
        pw->setMovedY(m_monitors[pw]->y());
        pw->setPos(0,0);
        if(!m_isSingleDisplay)
            pw->moveBy(pw->x(),pw->y());
    }

    onResize();
}

void MonitorsGround::onRotateChanged(const quint16 rotate)
{
    Q_UNUSED(rotate)

    if (m_setMergeMode)
        return;

    Monitor *mon = qobject_cast<Monitor*>(sender());
    for (auto pw : m_monitors.keys()) {
          if (mon->name() == m_monitors[pw]->name()) {
              m_movingItem = pw;
              pw->update();
              break;
          }
      }

    onResize();

    if(!m_isSingleDisplay)
        onRequestMonitorRelease(mon);
}

void MonitorsGround::onRequestMonitorRelease(Monitor *mon)
{
    Q_UNUSED(mon)
    bool isRestore = false;
    multiScreenSortAlgo(isRestore);
    if (isRestore == true)
        return;

    multiScreenAutoAdjust();
    onResize();
    updateConnectedState();

    //显示设置生效倒计时提示框
    Q_EMIT setEffectiveReminderVisible(true, m_nEffectiveTime);
    m_effectiveTimer->start();
}

void MonitorsGround::resetMonitorsView()
{
    if (m_model->displayMode() == MERGE_MODE) {
        adjustAll();
    } else {
        m_isInit = false;
        centeredMonitorsView();
    }
}


void MonitorsGround::adjustAll()
{
    setEnabled(false);

    const double scaleW = double(width()) / (m_model->screenWidth()*1.2);
    const double scaleH = double(height()) / (m_model->screenHeight()*1.2);

    const double scale = std::min(scaleW, scaleH);

    int cnt = 0;
    int offset = static_cast<int>(10/scale);

    for (auto pw : m_monitors.keys()) {
         pw->setZValue(cnt);
         pw->setPos(QPointF(offset * cnt, offset * cnt));
         cnt++;
    }

    QPointF dPos = sceneRect().center() - scene()->itemsBoundingRect().center();
    for (auto pw : m_monitors.keys()) {
        pw->setPos(pw->pos() + dPos);
    }

    resetMatrix();
    this->scale(scale, scale);
}

void MonitorsGround::onResize()
{
    if (!m_isSingleDisplay) {
        if (size().width() < MAX_W) {
            setSceneRect(QRect(0, 0, width(), 240 + size().width() - MIN_W));
            setFixedHeight(240 + size().width() - MIN_W);
        }
        else {
            setSceneRect(QRect(0, 0, width(), 240 + MAX_W - MIN_W));
            setFixedHeight(240 + MAX_W - MIN_W);
        }
        updateScale();
    }

    resetMonitorsView();
}

void MonitorsGround::resizeEvent(QResizeEvent *event)
{
    //解决最大化view高度设置不能及时生效的问题
    QTimer::singleShot(0,this,[=]{
        onResize();
    });

    QGraphicsView::resizeEvent(event);
}

void MonitorsGround::enterEvent(QEvent *)
{
    auto p = parentWidget();
    while (p && !m_scrollArea) {
        m_scrollArea = qobject_cast<QScrollArea *>(p);
        if (m_scrollArea)
            break;
        p = p->parentWidget();
    }
    if (m_scrollArea)
        QScroller::ungrabGesture(m_scrollArea->viewport());
}

void MonitorsGround::leaveEvent(QEvent *)
{
    if (m_scrollArea)
        QScroller::grabGesture(viewport(), QScroller::LeftMouseButtonGesture);
}

void MonitorsGround::singleScreenAdjest()
{
    const double scaleW = double(width()) / (m_monitors.values().last()->w()*1.5);
    const double scaleH = double(height()) / (m_monitors.values().last()->h()*1.5);

    const double scale = std::min(scaleW, scaleH);

    QPointF dPos = sceneRect().center() - scene()->itemsBoundingRect().center();
    m_monitors.keys().last()->setPos(m_monitors.keys().last()->pos() + dPos);
    resetMatrix();
    this->scale(scale, scale);
    setEnabled(false);
}


//应用设置
void MonitorsGround::applySettings()
{
    qRegisterMetaType<QHash<Monitor *, QPair<int, int>>>("QHash<Monitor *, QPair<int, int>>");
    QHash<Monitor *, QPair<int, int>> monitorPosition;

    QList<qreal> lstX, lstY;

    for (auto pw : m_monitors.keys()) {
        pw->update();
    }

    for (auto pw : m_monitors.keys()) {
        lstX.append(pw->pos().x());
        lstY.append(pw->pos().y());
        qDebug() << pw->name() << "pos" << pw->pos();
    }
    std::sort(lstX.begin(),lstX.end(),[=](const qreal item1, const qreal item2) {
       return item1 < item2;
    });
    std::sort(lstY.begin(),lstY.end(),[=](const qreal item1, const qreal item2) {
       return item1 < item2;
    });

    for (auto it(m_monitors.cbegin()); it != m_monitors.cend(); ++it) {
        monitorPosition.insert(it.value(), QPair<int, int>(int(it.key()->pos().x() - lstX.first()), int(it.key()->pos().y() - lstY.first())));
        qWarning() << "applySettings" << it.value()->name() << int(it.key()->pos().x() - lstX.first()) << int(it.key()->pos().y() - lstY.first()) << it.key()->pos();
    }
    Q_EMIT requestApplySettings(monitorPosition);
}

/*1050-5401*/
//多屏排序算法
QPointF MonitorsGround::multiScreenSortAlgo(bool &isRestore, bool isMove)
{
    //排列算法
    //1、寻找位置变化的屏幕块，确定为被移动的块
    //2、判断被移动块和其他块之间的位置关系来进行图形拼接移动
    //*根据中心点判定

    QList<MonitorProxyWidget *> lstActivedItemsX;
    QList<MonitorProxyWidget *> lstActivedItemsY;
    QList<MonitorProxyWidget *> lstNoMovedItems;
    QList<MonitorProxyWidget *> lstShelterItems;

    m_lstMoveingItemToCenterPosLen.clear();

    isRestore = false;
    bool isAutoAdsorption = false;
    bool isIntersect = false;
    qreal g_dx = 0.0;
    qreal g_dy = 0.0;

    qreal intersectedArea = 0.0; //相交的面积

    QRectF moveItemIntersect;

    bool g_bXYTogetherMoved = false; //标志XY方向是否一起移动，其余情况按哪个方向离得近向哪个方向移动


    moveItemIntersect = m_movingItem->mapRectToScene(m_movingItem->justIntersectRect());

    auto mapToSceneIntersectRect = [=](MonitorProxyWidget *item) {
        return item->mapToScene(item->justIntersectRect());
    };

    auto mapToSceneBoundingRect = [=](MonitorProxyWidget *item) {
        return item->mapToScene(item->boundingRect());
    };


    for (int i = 0; i < m_lstSortItems.size(); i++) {

        if (m_movingItem != m_lstSortItems[i]) {
            QRectF rect = moveItemIntersect.intersected(m_lstSortItems[i]->mapRectToScene(m_lstSortItems[i]->boundingRect()));
            intersectedArea += rect.width()*rect.height();

            //移动块完全覆盖一个块
            if (moveItemIntersect.contains(m_lstSortItems[i]->mapRectToScene(m_lstSortItems[i]->boundingRect()))) {
                lstShelterItems.append(m_lstSortItems[i]);
            }

            //要么不相交,要相交就是要点线相交的
            if (!mapToSceneIntersectRect(m_movingItem).intersects(mapToSceneBoundingRect(m_lstSortItems[i]))
               && mapToSceneBoundingRect(m_movingItem).intersects(mapToSceneBoundingRect(m_lstSortItems[i]))) {
                isAutoAdsorption = true;
            }
            //出现相交的情况
            if (mapToSceneIntersectRect(m_movingItem).intersects(mapToSceneBoundingRect(m_lstSortItems[i]))
                    && mapToSceneBoundingRect(m_movingItem).intersects(mapToSceneBoundingRect(m_lstSortItems[i]))) {
                isIntersect = true;
            }

            lstNoMovedItems.append(m_lstSortItems[i]);
        }
    }

    //移动块被完全包含在其他的块
    if (qFuzzyCompare(moveItemIntersect.width() * moveItemIntersect.height(), intersectedArea)) {
        qDebug() << "存在包含关系";
        lstShelterItems.append(m_movingItem);
    }

    //移动块到其他块中心点的距离排序
    QPointF movedPos = m_movingItem->mapToScene(m_movingItem->boundingRect().center());
    for (auto item : lstNoMovedItems) {
        QPointF tempPos = item->mapToScene(item->boundingRect().center());
        qreal len = sqrt(pow(tempPos.x()-movedPos.x(),2)+pow(tempPos.y()-movedPos.y(),2));
        m_lstMoveingItemToCenterPosLen.append(qMakePair(item,len));
    }

    //获取距离最近的块
    std::sort(m_lstMoveingItemToCenterPosLen.begin(),m_lstMoveingItemToCenterPosLen.end(),[=](const QPair<MonitorProxyWidget *, qreal> &item1, const QPair<MonitorProxyWidget *, qreal> &item2) {
       return item1.second < item2.second;
    });

    //自动回弹的触发条件: 1. 一个屏幕完全包含另一个屏的时候 2. 一个屏幕剩下的屏幕集合所包围
    if (lstShelterItems.size() > 0) {
        if (isMove) {
            autoRebound();
            qDebug() << "自动回弹流程触发! == " << lstShelterItems.size();
            isRestore = true;
            return QPointF(0.0,0.0);
        }
    }
    else { //没有完全重合的情况处理逻辑

        std::sort(lstNoMovedItems.begin(),lstNoMovedItems.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                  return item1->mapToScene(item1->boundingRect().bottomLeft()).x() < item2->mapToScene(item2->boundingRect().bottomLeft()).x();
               });

       //将MovedItem放入链表中排序，左排 右排，判定覆盖几个块
       lstNoMovedItems.append(m_movingItem);
       //左排序
       std::sort(lstNoMovedItems.begin(),lstNoMovedItems.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {

           if (item1 == m_movingItem) {
               return item1->mapToScene(item1->boundingRect().bottomRight()).x() < item2->mapToScene(item2->boundingRect().bottomLeft()).x();
           }
           else if (item2 == m_movingItem) {
               return item1->mapToScene(item1->boundingRect().bottomLeft()).x() < item2->mapToScene(item2->boundingRect().bottomRight()).x();
           }
           return item1->mapToScene(item1->boundingRect().bottomLeft()).x() < item2->mapToScene(item2->boundingRect().bottomLeft()).x();
       });
       int nBIndexLeft = lstNoMovedItems.indexOf(m_movingItem);

       //右排序
       std::sort(lstNoMovedItems.begin(),lstNoMovedItems.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {

           if (item1 == m_movingItem) {
               return item1->mapToScene(item1->boundingRect().bottomLeft()).x() < item2->mapToScene(item2->boundingRect().bottomRight()).x();
           }
           else if (item2 == m_movingItem) {
               return item1->mapToScene(item1->boundingRect().bottomRight()).x() < item2->mapToScene(item2->boundingRect().bottomLeft()).x();
           }
           return item1->mapToScene(item1->boundingRect().bottomRight()).x() < item2->mapToScene(item2->boundingRect().bottomRight()).x();
       });
       int nBIndexRight = lstNoMovedItems.indexOf(m_movingItem);

       //上排序
       std::sort(lstNoMovedItems.begin(),lstNoMovedItems.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {

           if (item1 == m_movingItem) {
               return item1->mapToScene(item1->boundingRect().bottomLeft()).y() < item2->mapToScene(item2->boundingRect().topLeft()).y();
           }
           else if (item2 == m_movingItem) {
               return item1->mapToScene(item1->boundingRect().topLeft()).y() < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
           }
           return item1->mapToScene(item1->boundingRect().topLeft()).y() < item2->mapToScene(item2->boundingRect().topLeft()).y();
       });
       int nBIndexTop = lstNoMovedItems.indexOf(m_movingItem);

       //下排序
       std::sort(lstNoMovedItems.begin(),lstNoMovedItems.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {

           if (item1 == m_movingItem) {
               return item1->mapToScene(item1->boundingRect().topLeft()).y() < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
           }
           else if (item2 == m_movingItem) {
               return item1->mapToScene(item1->boundingRect().bottomLeft()).y() < item2->mapToScene(item2->boundingRect().topLeft()).y();
           }
           return item1->mapToScene(item1->boundingRect().bottomLeft()).y() < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
       });
       int nBIndexBottom = lstNoMovedItems.indexOf(m_movingItem);

       int nItemSize = lstNoMovedItems.size() - 1;

       //说明移动块在其他块X坐标有重合
       if (nBIndexLeft != 0 && nBIndexRight != nItemSize) {

           //移动X
           //将MovedItem放入链表中排序，上排 下排，判定在Y轴移动的方向和距离
           //处理
           qreal movex1 = m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).x();
           qreal movex2 = m_movingItem->mapToScene(m_movingItem->boundingRect().topRight()).x();

           for (auto pair : m_lstMoveingItemToCenterPosLen) {
               qreal x1 = pair.first->mapToScene(pair.first->boundingRect().topLeft()).x();
               qreal x2 = pair.first->mapToScene(pair.first->boundingRect().topRight()).x();

               if (!((x1 > movex1 && x1 > movex2) || (x2 < movex1 && x2 < movex2))) {
                   lstActivedItemsX.append(pair.first);
                }
           }

           lstActivedItemsX.append(m_movingItem);
           //上排序
           std::sort(lstActivedItemsX.begin(),lstActivedItemsX.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
              return item1->mapToScene(item1->boundingRect().topLeft()).y() < item2->mapToScene(item2->boundingRect().topLeft()).y();
           });
           int nIndexTop = lstActivedItemsX.indexOf(m_movingItem);

           //下排序
           std::sort(lstActivedItemsX.begin(),lstActivedItemsX.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
              return item1->mapToScene(item1->boundingRect().bottomLeft()).y() < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
           });
           int nIndexBottom = lstActivedItemsX.indexOf(m_movingItem);

           //先做预移动，如果有重合，把重合块加入激活块重新排序
           if (nIndexTop == nIndexBottom) {
               //移动块在最上面
               if (nIndexTop == 0) {
                   qreal dy = lstActivedItemsX.first()->mapToScene(lstActivedItemsX.first()->boundingRect().topLeft()).y() - m_movingItem->mapToScene(m_movingItem->boundingRect().bottomLeft()).y();
                   m_movingItem->moveBy(0, dy);
                   m_graphicsScene.update();

                   QList<MonitorProxyWidget *> lstShelterItemsTemp;
                   lstNoMovedItems.removeOne(m_movingItem);
                   for (int i = 0; i < lstNoMovedItems.size(); i++) {
                       if (m_movingItem->mapToScene(m_movingItem->boundingRect()).intersects(lstNoMovedItems[i]->mapToScene(lstNoMovedItems[i]->boundingRect()))) {
                           lstShelterItemsTemp.append(lstNoMovedItems[i]);
                       }
                   }

                   if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(0, -dy);//先移回去
                        m_graphicsScene.update();

                        //下排序
                        std::sort(lstActivedItemsX.begin(),lstActivedItemsX.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                           return item1->mapToScene(item1->boundingRect().bottomLeft()).y() < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
                        });
                        //int nIndexBottomTemp = lstActivedItemsX.indexOf(m_movingItem);

                        //上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsX.contains(t))
                                lstActivedItemsX.append(t);
                        }
                        std::sort(lstActivedItemsX.begin(),lstActivedItemsX.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                           return item1->mapToScene(item1->boundingRect().topLeft()).y() < item2->mapToScene(item2->boundingRect().topLeft()).y();
                        });
                        int nIndexTopTemp = lstActivedItemsX.indexOf(m_movingItem);

                        lstActivedItemsX.removeOne(m_movingItem);

                        if(!lstActivedItemsX.isEmpty()) {
                            if (nIndexTopTemp == 0) {
                                g_dy = lstActivedItemsX.first()->mapToScene(lstActivedItemsX.first()->boundingRect().topLeft()).y() - m_movingItem->mapToScene(m_movingItem->boundingRect().bottomLeft()).y();
                                qDebug() << "g_dy" << g_dy;
                            }
                            else {
                                g_dy = lstActivedItemsX.last()->mapToScene(lstActivedItemsX.last()->boundingRect().bottomLeft()).y() - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).y();
                                qDebug() << "g_dy" << g_dy;

                            }
                        }
                   }
               }
                   //移动块在最下面
               else {
                   if(!lstActivedItemsX.contains(m_movingItem))
                       lstActivedItemsX.append(m_movingItem);
                   //下排序
                   std::sort(lstActivedItemsX.begin(),lstActivedItemsX.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                      return item1->mapToScene(item1->boundingRect().bottomLeft()).y() < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
                   });

                   qreal dy = lstActivedItemsX.last()->mapToScene(lstActivedItemsX.last()->boundingRect().bottomLeft()).y() - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).y();
                   m_movingItem->moveBy(0, dy);
                   m_graphicsScene.update();

                   lstNoMovedItems.removeOne(m_movingItem);
                   QList<MonitorProxyWidget *> lstShelterItemsTemp;
                   for (int i = 0; i < lstNoMovedItems.size(); i++) {
                       if (m_movingItem->mapToScene(m_movingItem->boundingRect()).intersects(lstNoMovedItems[i]->mapToScene(lstNoMovedItems[i]->boundingRect()))) {
                           lstShelterItemsTemp.append(lstNoMovedItems[i]);
                       }
                   }


                   if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(0, -dy);//先移回去
                        m_graphicsScene.update();

                        //上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsX.contains(t))
                                lstActivedItemsX.append(t);
                        }

                        std::sort(lstActivedItemsX.begin(),lstActivedItemsX.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                           return item1->mapToScene(item1->boundingRect().topLeft()).y() < item2->mapToScene(item2->boundingRect().topLeft()).y();
                        });
                        int nIndexTopTemp = lstActivedItemsX.indexOf(m_movingItem);

                        //下排序
                        std::sort(lstActivedItemsX.begin(),lstActivedItemsX.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                           return item1->mapToScene(item1->boundingRect().bottomLeft()).y() < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
                        });
                        //int nIndexBottomTemp = lstActivedItemsX.indexOf(m_movingItem);

                        lstActivedItemsX.removeOne(m_movingItem);

                        if(!lstActivedItemsX.isEmpty()) {
                            if (nIndexTopTemp == 0) {
                                g_dy = lstActivedItemsX.first()->mapToScene(lstActivedItemsX.first()->boundingRect().topLeft()).y() - m_movingItem->mapToScene(m_movingItem->boundingRect().bottomLeft()).y();
                                qDebug() << "g_dy" << g_dy;
                            }
                            else {
                                g_dy = lstActivedItemsX.last()->mapToScene(lstActivedItemsX.last()->boundingRect().bottomLeft()).y() - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).y();
                                qDebug() << "g_dy" << g_dy;
                            }
                        }
                   }
               }
           }
       }

       //说明移动块在其他块Y坐标有重合
       if (nBIndexTop != 0 && nBIndexBottom != nItemSize) {

           //移动Y
           //处理
           qreal movey1 = m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).y();
           qreal movey2 = m_movingItem->mapToScene(m_movingItem->boundingRect().bottomLeft()).y();

           for (auto pair : m_lstMoveingItemToCenterPosLen) {
               qreal y1 = pair.first->mapToScene(pair.first->boundingRect().topLeft()).y();
               qreal y2 = pair.first->mapToScene(pair.first->boundingRect().bottomLeft()).y();

               if (!((y1 > movey1 && y1 > movey2) || (y2 < movey1 && y2 < movey2))) {
                    lstActivedItemsY.append(pair.first);
                }
           }

           lstActivedItemsY.append(m_movingItem);
           //左排序
           std::sort(lstActivedItemsY.begin(),lstActivedItemsY.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
              return item1->mapToScene(item1->boundingRect().topLeft()).x() < item2->mapToScene(item2->boundingRect().topLeft()).x();
           });
           int nIndexLeft = lstActivedItemsY.indexOf(m_movingItem);

           //右排序
           std::sort(lstActivedItemsY.begin(),lstActivedItemsY.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
              return item1->mapToScene(item1->boundingRect().topRight()).x() < item2->mapToScene(item2->boundingRect().topRight()).x();
           });
           int nIndexRight = lstActivedItemsY.indexOf(m_movingItem);

           if (nIndexLeft == nIndexRight) {
               //移动块在最右侧
               if (nIndexLeft == 0) {
                   qreal dx = lstActivedItemsY.first()->mapToScene(lstActivedItemsY.first()->boundingRect().topLeft()).x() - m_movingItem->mapToScene(m_movingItem->boundingRect().topRight()).x();
                   m_movingItem->moveBy(dx, 0);
                   m_graphicsScene.update();

                   QList<MonitorProxyWidget *> lstShelterItemsTemp;
                   lstNoMovedItems.removeOne(m_movingItem);
                   for (int i = 0; i < lstNoMovedItems.size(); i++) {
                       if (m_movingItem->mapToScene(m_movingItem->boundingRect()).intersects(lstNoMovedItems[i]->mapToScene(lstNoMovedItems[i]->boundingRect()))) {
                           lstShelterItemsTemp.append(lstNoMovedItems[i]);
                       }
                   }


                   if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(-dx, 0);
                        m_graphicsScene.update();

                        //下排序
                        std::sort(lstActivedItemsY.begin(),lstActivedItemsY.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                           return item1->mapToScene(item1->boundingRect().topRight()).x() < item2->mapToScene(item2->boundingRect().topRight()).x();
                        });
                        //int nIndexRightTemp = lstActivedItemsY.indexOf(m_movingItem);

                        //上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsY.contains(t))
                                lstActivedItemsY.append(t);
                        }
                        std::sort(lstActivedItemsY.begin(),lstActivedItemsY.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                           return item1->mapToScene(item1->boundingRect().topLeft()).x() < item2->mapToScene(item2->boundingRect().topLeft()).x();
                        });
                        int nIndexLeftTemp = lstActivedItemsY.indexOf(m_movingItem);

                        lstActivedItemsY.removeOne(m_movingItem);

                        if(!lstActivedItemsY.isEmpty()) {
                            if (nIndexLeftTemp == 0) {
                                g_dx = lstActivedItemsY.first()->mapToScene(lstActivedItemsY.first()->boundingRect().topLeft()).x() - m_movingItem->mapToScene(m_movingItem->boundingRect().topRight()).x();
                                qDebug() << "g_dx" << g_dx;
                            }
                            else {
                                g_dx = lstActivedItemsY.last()->mapToScene(lstActivedItemsY.last()->boundingRect().topRight()).x() - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).x();
                                qDebug() << "g_dx" << g_dx;
                            }
                        }
                   }
               }
                   //移动块在最左侧
               else {
                   if(!lstActivedItemsY.contains(m_movingItem))
                       lstActivedItemsY.append(m_movingItem);
                   //右排序
                   std::sort(lstActivedItemsY.begin(),lstActivedItemsY.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                      return item1->mapToScene(item1->boundingRect().topRight()).x() < item2->mapToScene(item2->boundingRect().topRight()).x();
                   });

                   qreal dx = lstActivedItemsY.last()->mapToScene(lstActivedItemsY.last()->boundingRect().topRight()).x() - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).x();
                   m_movingItem->moveBy(dx, 0);
                   m_graphicsScene.update();

                   lstNoMovedItems.removeOne(m_movingItem);
                   QList<MonitorProxyWidget *> lstShelterItemsTemp;
                   for (int i = 0; i < lstNoMovedItems.size(); i++) {
                       if (m_movingItem->mapToScene(m_movingItem->boundingRect()).intersects(lstNoMovedItems[i]->mapToScene(lstNoMovedItems[i]->boundingRect()))) {
                           lstShelterItemsTemp.append(lstNoMovedItems[i]);
                       }
                   }

                   if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(-dx, 0);
                        m_graphicsScene.update();

                        //上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsY.contains(t))
                                lstActivedItemsY.append(t);
                        }
                        std::sort(lstActivedItemsY.begin(),lstActivedItemsY.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                           return item1->mapToScene(item1->boundingRect().topLeft()).x() < item2->mapToScene(item2->boundingRect().topLeft()).x();
                        });
                        int nIndexLeftTemp = lstActivedItemsY.indexOf(m_movingItem);

                        //下排序
                        std::sort(lstActivedItemsY.begin(),lstActivedItemsY.end(),[=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                           return item1->mapToScene(item1->boundingRect().topRight()).x() < item2->mapToScene(item2->boundingRect().topRight()).x();
                        });
                        //int nIndexRightTemp = lstActivedItemsY.indexOf(m_movingItem);

                        lstActivedItemsY.removeOne(m_movingItem);

                        if(!lstActivedItemsY.isEmpty()) {
                            if (nIndexLeftTemp == 0) {
                                g_dx = lstActivedItemsY.first()->mapToScene(lstActivedItemsY.first()->boundingRect().topLeft()).x() - m_movingItem->mapToScene(m_movingItem->boundingRect().topRight()).x();
                                qDebug() << "g_dx" << g_dx;
                            }
                            else {
                                g_dx = lstActivedItemsY.last()->mapToScene(lstActivedItemsY.last()->boundingRect().topRight()).x() - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).x();
                                qDebug() << "g_dx" << g_dx;
                            }
                        }
                   }
               }
           }
       }

       //说明移动块在其他块的四周
       if ((nBIndexLeft == 0 || nBIndexRight == nItemSize) || (nBIndexTop == 0 || nBIndexBottom == nItemSize)) {
          //表示X方向没有重叠的
          //TODO：
          //找距离他最近的顶点
          //LT 左上
          lstNoMovedItems.removeOne(m_movingItem);
          QRectF movedRectF = m_movingItem->mapRectToScene(m_movingItem->boundingRect());
          QList<QPair<MonitorProxyWidget *, qreal>> lstTempLen;
          if (nBIndexLeft == 0 && nBIndexTop == 0) {
              for (auto item : lstNoMovedItems) {
                  QPointF tempPos = item->mapToScene(item->boundingRect().topLeft());
                  qreal len = sqrt(pow(tempPos.x()-movedRectF.bottomRight().x(),2)+pow(tempPos.y()-movedRectF.bottomRight().y(),2));
                  lstTempLen.append(qMakePair(item,len));
              }
              //获取距离最近的块
              std::sort(lstTempLen.begin(),lstTempLen.end(),[=](const QPair<MonitorProxyWidget *, qreal> &item1, const QPair<MonitorProxyWidget *, qreal> &item2) {
                 return item1.second < item2.second;
              });

              QPointF dPos = lstTempLen.first().first->mapToScene(lstTempLen.first().first->boundingRect().topLeft()) - m_movingItem->mapToScene(m_movingItem->boundingRect().bottomRight());
              g_dx = dPos.x();
              g_dy = dPos.y();
              qDebug() << "g_dx" << g_dx;
              qDebug() << "g_dy" << g_dy;
              g_bXYTogetherMoved = true;
          }

          //LB 左下
          if (nBIndexLeft == 0 && nBIndexBottom == nItemSize) {
              for (auto item : lstNoMovedItems) {
                  QPointF tempPos = item->mapToScene(item->boundingRect().bottomLeft());
                  qreal len = sqrt(pow(tempPos.x()-movedRectF.topRight().x(),2)+pow(tempPos.y()-movedRectF.topRight().y(),2));
                  lstTempLen.append(qMakePair(item,len));
              }
              //获取距离最近的块
              std::sort(lstTempLen.begin(),lstTempLen.end(),[=](const QPair<MonitorProxyWidget *, qreal> &item1, const QPair<MonitorProxyWidget *, qreal> &item2) {
                 return item1.second < item2.second;
              });

              QPointF dPos = lstTempLen.first().first->mapToScene(lstTempLen.first().first->boundingRect().bottomLeft()) - m_movingItem->mapToScene(m_movingItem->boundingRect().topRight());
              g_dx = dPos.x();
              g_dy = dPos.y();
              g_bXYTogetherMoved = true;
              qDebug() << "g_dx" << g_dx;
              qDebug() << "g_dy" << g_dy;
          }

          //RT 右上
          if (nBIndexRight == nItemSize && nBIndexTop == 0) {
              for (auto item : lstNoMovedItems) {
                  QPointF tempPos = item->mapToScene(item->boundingRect().topRight());
                  qreal len = sqrt(pow(tempPos.x()-movedRectF.bottomLeft().x(),2)+pow(tempPos.y()-movedRectF.bottomLeft().y(),2));
                  lstTempLen.append(qMakePair(item,len));
              }
              //获取距离最近的块
              std::sort(lstTempLen.begin(),lstTempLen.end(),[=](const QPair<MonitorProxyWidget *, qreal> &item1, const QPair<MonitorProxyWidget *, qreal> &item2) {
                 return item1.second < item2.second;
              });

              QPointF dPos = lstTempLen.first().first->mapToScene(lstTempLen.first().first->boundingRect().topRight()) - m_movingItem->mapToScene(m_movingItem->boundingRect().bottomLeft());
              g_dx = dPos.x();
              g_dy = dPos.y();
              g_bXYTogetherMoved = true;
              qDebug() << "g_dx" << g_dx;
              qDebug() << "g_dy" << g_dy;
          }

          //RB 右下

           if (nBIndexRight == nItemSize && nBIndexBottom == nItemSize) {
               for (auto item : lstNoMovedItems) {
                   QPointF tempPos = item->mapToScene(item->boundingRect().bottomRight());
                   qreal len = sqrt(pow(tempPos.x()-movedRectF.topLeft().x(),2)+pow(tempPos.y()-movedRectF.topLeft().y(),2));
                   lstTempLen.append(qMakePair(item,len));
               }
               //获取距离最近的块
               std::sort(lstTempLen.begin(),lstTempLen.end(),[=](const QPair<MonitorProxyWidget *, qreal> &item1, const QPair<MonitorProxyWidget *, qreal> &item2) {
                  return item1.second < item2.second;
               });

               QPointF dPos = lstTempLen.first().first->mapToScene(lstTempLen.first().first->boundingRect().bottomRight()) - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft());
               g_dx = dPos.x();
               g_dy = dPos.y();
               qDebug() << "g_dx" << g_dx;
               qDebug() << "g_dy" << g_dy;
               g_bXYTogetherMoved = true;
           }
       }
    }

    qreal dx = g_dx, dy = g_dy;

    //是自动吸附并且没有相交的情况，不需要要移动
    if (isAutoAdsorption&&!isIntersect) {
        if (isMove)
            m_movingItem->moveBy(0, 0);
        else {
            dx = 0.0;
            dy = 0.0;
        }
    }
    //其他情况需要移动
    else {
        //是否XY一起移动
        if (g_bXYTogetherMoved) {
            if (isMove)
                m_movingItem->moveBy(g_dx, g_dy);
            qDebug() << "XY_XY_XY" << g_dx << g_dy;
        }
        else {
            //XY其中一个为0的情况， 执行移动
            if (qFuzzyIsNull(g_dx) || qFuzzyIsNull(g_dy)) {
                if (isMove)
                    m_movingItem->moveBy(g_dx, g_dy);

                 qDebug() << "DBL_MIN" << g_dx << g_dy;
            }
            //X和Y都不为0的情况下，哪个移动的绝对值小移动哪一个
            else{
                if (isMove)
                    fabs(g_dx) < fabs(g_dy) ?  m_movingItem->moveBy(g_dx, 0): m_movingItem->moveBy(0, g_dy);
                else {
                    fabs(g_dx) < fabs(g_dy) ?  dy = 0.0: dx = 0.0;
                }
                qDebug() << "fabs(g_dx) < fabs(g_dy) ?" << g_dx << g_dy;
            }
        }
    }
    for (auto pw : m_monitors.keys()) {
        pw->update();
    }
    m_graphicsScene.update();

    if (!isMove)
        return QPointF(dx,dy);
    else {
        return QPointF(g_dx,g_dy);
    }
}

//多屏自动调整
void MonitorsGround::multiScreenAutoAdjust()
{
    //在自动拼接之后已经达成全连通状态,无效执行自动调整
    updateConnectedState();

    //判断是否为全连通状态
    if (getConnectedDomain(m_movingItem).size() == m_lstItems.size()) {
        updateConnectedState(true);
        return;
    }

    //存储移动块到临时变量
    MonitorProxyWidget * m_movingItemTemp = m_movingItem;
    QList<MonitorProxyWidget *> lstChangedItems = m_lstItems;

    //处理之前变化的块为空则返回
    if (lstChangedItems.isEmpty())
        return;

    QMap<MonitorProxyWidget *,QList<MonitorProxyWidget *>> maplstItems;
    QMap<MonitorProxyWidget *, bool> mapItemsNeedMove;
    QList<MonitorProxyWidget *> itemTemp;

    //这个列表是存放的就是所有移动块相关的块
    //判断改变连通的块与移动块的剩余联通块是否存在连接
    //【这种是在移动块初始连接块是两个及以上的情况下触发的，如果是一个连通块的话不会改变连通状态的】

    //获取屏幕集群
    for (auto iter : m_lstItems) {

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
        qDebug() << "处理之后变化的块:" << item->name();
    }

    for (auto iter : lstChangedItems) {
        maplstItems.insert(iter, getConnectedDomain(iter));
     }

    for (auto iter : maplstItems.keys()) {
        qDebug() << "屏幕集群:" << iter->name();
        for (auto k : maplstItems[iter]) {
            qDebug() << "val" << k->name();
        }
    }


    if (maplstItems.size() > 0) {
        //说明除了移动块剩下的被拆分成了几部分，需要重新向其中一个部分移动了
       //找到中心屏幕集群(包含移动块)
       for (auto iter1 = maplstItems.begin(); iter1 != maplstItems.end(); ++iter1) {
            if (iter1.value().contains(m_movingItemTemp)) {
                m_lstSortItems = iter1.value();
                break;
            }
       }

       //其他屏幕集群依次向中心屏幕集群移动
       for (auto iter = maplstItems.begin(); iter != maplstItems.end(); ++iter) {
            if (iter.value().contains(m_movingItemTemp))
                continue;

            QList<QPointF> lstPos;
            QList<qreal> lstX,lstY;
            for (auto item : iter.value()) {
                m_movingItem = item;
                qDebug() << "将此屏幕设置为移动块" << m_movingItem->name();
                bool isRestore = false;
                QPointF pos = multiScreenSortAlgo(isRestore,false);
                lstPos.append(pos);
                lstX.append(pos.x());
                lstY.append(pos.y());
                qDebug() << "坐标信息" << pos;
            }
            qreal mindX = 0.0;
            qreal mindY = 0.0;
            if (!lstPos.isEmpty()) {
                //排序，根据绝对值排序
                std::sort(lstX.begin(),lstX.end(),[=](qreal item1,  qreal item2) {
                   return fabs(item1) < fabs(item2);
                });

                std::sort(lstY.begin(),lstY.end(),[=](qreal item1,  qreal item2) {
                   return fabs(item1) < fabs(item2);
                });

                mindX = lstX.first();
                mindY = lstY.first();

                if (qFuzzyIsNull(mindX) && qFuzzyIsNull(mindY)) {
                    if(qFuzzyIsNull(lstX.first())) {
                        lstX.takeFirst();
                        if(lstX.size() > 0)
                            mindX = lstX.first();
                    }
                    else {
                        if(qFuzzyIsNull(lstY.first())) {
                            lstY.takeFirst();
                            if(lstY.size() > 0)
                                mindY = lstY.first();
                        }
                    }
                }


                qDebug() << "minX:" << mindX << "minY:" << mindY;

                //执行移动操作
                for (auto item : iter.value()) {
                    m_movingItem = item;
                    m_movingItem->moveBy(mindX, mindY);
                    m_graphicsScene.update();
                    if (!m_lstSortItems.contains(item))
                        m_lstSortItems.append(item);
                }

                for (auto k : m_lstSortItems) {
                    qDebug() << "sortItems" << k->name();
                }
            }
       }
    }
    m_lstSortItems = m_lstItems;
    m_movingItem = m_movingItemTemp;

    //自动调整完毕后,更新连通域
    updateConnectedState(true);
}

//更连通状态
//更新上一次拼接完成的值
void MonitorsGround::updateConnectedState(bool isInit)
{
    QList<MonitorProxyWidget *> m_lstItemsTemp;
    for (int i = 0; i < m_lstItems.size(); i++) {
        m_lstItemsTemp.clear();

        for (int j = 0; j < m_lstItems.size(); j++) {
            if (j != i && m_lstItems[i]->mapToScene(m_lstItems[i]->boundingRectEx()).intersects(m_lstItems[j]->mapToScene(m_lstItems[j]->boundingRect()))
                   && !m_lstItemsTemp.contains(m_lstItems[j])
                   && !m_lstItems[i]->mapToScene(m_lstItems[i]->justIntersectRect()).intersects(m_lstItems[j]->mapToScene(m_lstItems[j]->boundingRect()))) {
                m_lstItemsTemp.append(m_lstItems[j]);
            }
        }

        if (isInit) {
            m_mapInitItemConnectedState.insert(m_lstItems[i],m_lstItemsTemp);
        }

        m_mapItemConnectedState.insert(m_lstItems[i],m_lstItemsTemp);
    }
}

//获取连通域
QList<MonitorProxyWidget *> MonitorsGround::getConnectedDomain(MonitorProxyWidget *item)
{
   QList<MonitorProxyWidget *> lstTemp1Items;

   QList<MonitorProxyWidget *> lstTemp;
   QList<MonitorProxyWidget *> lstItems;

   lstItems.append(item);

   for (auto iter : m_lstItems) /*标记循环次数*/ {
       Q_UNUSED(iter)

       lstTemp.clear();
       //获取所有的value
       for (auto a : lstItems) {
           for (auto t : m_mapItemConnectedState[a]) {
               if (!lstTemp1Items.contains(t))
                   lstTemp.append(t);
           }
       }

       //去重插入
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


//响应键盘调整 脱离的需要拉回来,重叠的需要打回去
//左右边界相交1个像素的情况下,只能像上下运动; 相反如果上下边界相交1个像素的情况下,只能左右运动.
//鼠标巡边移动
void MonitorsGround::onRequestKeyPress(MonitorProxyWidget *pw, int keyValue)
{
    Q_EMIT setEffectiveReminderVisible(false, m_nEffectiveTime);
    m_effectiveTimer->stop();

    //当鼠标移动的时候开始响应并执行自动吸附的逻辑
    //保证 bufferboundingRect 相交且 boundingRect 不相交，保证移动的块item始终在其他item的外边缘移动

    //需要在全连通的情况下开始响应键盘事件
    bool bIntersectsTop = false;
    bool bIntersectsBottom = false;
    bool bIntersectsRight = false;
    bool bIntersectsLeft = false;

    for (auto item : m_lstItems) {
        if (item == pw)
            continue;
        if (pw->mapToScene(pw->boundingRect()).intersects(item->mapToScene(item->boundingRect()))) {
            if (item->mapToScene(item->boundingRect()).intersects(pw->mapToScene(pw->justIntersectRectTop()))) {
                //上相交
                bIntersectsTop = true;
            }
            else if (item->mapToScene(item->boundingRect()).intersects(pw->mapToScene(pw->justIntersectRectBottom()))) {
                //下相交
                bIntersectsBottom = true;
            }
            else if (item->mapToScene(item->boundingRect()).intersects(pw->mapToScene(pw->justIntersectRectLeft()))) {
                //左相交
                bIntersectsLeft = true;
            }
            else if (item->mapToScene(item->boundingRect()).intersects(pw->mapToScene(pw->justIntersectRectRight()))) {
                //右相交
                bIntersectsRight = true;
            }
        }
    }

    if (!bIntersectsTop && !bIntersectsBottom && !bIntersectsRight && !bIntersectsLeft) {
        for (auto item : m_lstItems) {
            if (item == pw)
                continue;
            //达成顶点相交
            if (item->mapToScene(item->boundingRect()).intersects(pw->mapToScene(pw->justIntersectRectLeftTop()))) {
                //左上相交
                bIntersectsBottom = true;
                bIntersectsRight = true;
            }
            else if (item->mapToScene(item->boundingRect()).intersects(pw->mapToScene(pw->justIntersectRectLeftBottom()))) {
                //左下相交
                bIntersectsTop = true;
                bIntersectsRight = true;
            }
            else if (item->mapToScene(item->boundingRect()).intersects(pw->mapToScene(pw->justIntersectRectRightTop()))) {
                //右上相交
                bIntersectsBottom = true;
                bIntersectsLeft = true;
            }
            else if (item->mapToScene(item->boundingRect()).intersects(pw->mapToScene(pw->justIntersectRectRightBottom()))) {
                //右下相交
                bIntersectsTop = true;
                bIntersectsLeft = true;
            }
        }
    }

    int moveStep = 10;
    //根据按键的方向与当前的相接的边和连通域的变化来综合判定是否执行移动操作
    switch (keyValue)
    {
    case  Qt::Key_Left:
        if (!bIntersectsLeft && (bIntersectsTop || bIntersectsBottom)) {
            //执行运动
            pw->moveBy(-moveStep,0);
            updateConnectedState();
            if (getConnectedDomain(m_movingItem).size() != m_lstItems.size()) {
                pw->moveBy(moveStep,0);
            }
        }
        break;
    case  Qt::Key_Right:
        if (!bIntersectsRight && (bIntersectsTop || bIntersectsBottom)) {
            //执行运动
            pw->moveBy(moveStep,0);
            updateConnectedState();
            if (getConnectedDomain(m_movingItem).size() != m_lstItems.size()) {
                pw->moveBy(-moveStep,0);
            }
        }
        break;
    case  Qt::Key_Up:
        if (!bIntersectsTop && (bIntersectsLeft || bIntersectsRight)) {
            //执行运动
            pw->moveBy(0,-moveStep);
            updateConnectedState();
            if (getConnectedDomain(m_movingItem).size() != m_lstItems.size()) {
                pw->moveBy(0,moveStep);
            }
        }
        break;
    case  Qt::Key_Down:
        if (!bIntersectsBottom && (bIntersectsLeft || bIntersectsRight)) {
            //执行运动
            pw->moveBy(0,moveStep);
            updateConnectedState();
            if (getConnectedDomain(m_movingItem).size() != m_lstItems.size()) {
                pw->moveBy(0,-moveStep);
            }
        }
        break;
    default:
        break;
    }

    pw->update();

    Q_EMIT setEffectiveReminderVisible(true, m_nEffectiveTime);
    m_effectiveTimer->start();
}

//自动吸附实现
void MonitorsGround::onRequestMouseMove(MonitorProxyWidget *pw)
{
    Q_EMIT setEffectiveReminderVisible(false, m_nEffectiveTime);
    m_effectiveTimer->stop();

    //当鼠标移动的时候开始响应并执行自动吸附的逻辑
    //保证 bufferboundingRect 相交且 boundingRect 不相交，保证移动的块item始终在其他item的外边缘移动

    qreal top = 0.0;
    qreal bottom = 0.0;
    qreal right = 0.0;
    qreal left = 0.0;

    auto minMoveLen = [=](qreal temp, qreal &len) {
        if (fabs(len) > 0.0) {
            if (fabs(temp) < fabs(len)) len = temp;
        }
        else {
            len = temp;
        }
    };

    for (auto item : m_lstItems) {
        if (item == pw)
            continue;

        if (pw->mapToScene(pw->boundingRect()).intersects(item->mapToScene(item->boundingRect())))
            return;

        //与boundingRect 不相交
        if (pw->mapToScene(pw->boundingRect()).intersects(item->mapToScene(item->bufferboundingRectTop()))) {
            //上相交
            qreal temp =  item->mapToScene(item->boundingRect().topRight()).y() - pw->mapToScene(pw->boundingRect().bottomLeft()).y();
            minMoveLen(temp,top);
        }
        else if (pw->mapToScene(pw->boundingRect()).intersects(item->mapToScene(item->bufferboundingRectBottom()))) {
            //下相交
            qreal temp =  item->mapToScene(item->boundingRect().bottomLeft()).y() - pw->mapToScene(pw->boundingRect().topLeft()).y();
            minMoveLen(temp,bottom);
        }
        else if (pw->mapToScene(pw->boundingRect()).intersects(item->mapToScene(item->bufferboundingRectLeft()))) {
            //左相交
            qreal temp =  item->mapToScene(item->boundingRect().bottomLeft()).x() - pw->mapToScene(pw->boundingRect().bottomRight()).x();
            minMoveLen(temp,left);
        }
        else if (pw->mapToScene(pw->boundingRect()).intersects(item->mapToScene(item->bufferboundingRectRight()))) {
            //右相交
            qreal temp = item->mapToScene(item->boundingRect().bottomRight()).x() - pw->mapToScene(pw->boundingRect().bottomLeft()).x();
            minMoveLen(temp,right);
        }
    }

    if (qFuzzyIsNull(top))
        pw->moveBy(0,bottom);
    else if (qFuzzyIsNull(bottom))
        pw->moveBy(0,top);
    else {
        (fabs(top) < fabs(bottom)) ? pw->moveBy(0,top) : pw->moveBy(0,bottom);
    }

    if (qFuzzyIsNull(left))
        pw->moveBy(right,0);
    else if (qFuzzyIsNull(right))
        pw->moveBy(left,0);
    else {
        (fabs(left) < fabs(right)) ? pw->moveBy(left,0) : pw->moveBy(right,0);
    }
}

//更新缩放比例
void MonitorsGround::updateScale()
{
    QRectF rectView = rect();
    QRectF rect = m_graphicsScene.itemsBoundingRect();

    double screenWidth = rect.width();
    double screenHeight = rect.height();

    //计算缩放比例 1.2，是指活动区域和调节区域
    double hScale = ((rectView.width() > MAX_W)? MAX_W: rectView.width()) / 1.2 / screenWidth;
    double wScale = rectView.height() / 1.2 / screenHeight;
    m_scale = std::min(hScale,wScale);

    resetMatrix();
    scale(m_scale, m_scale);
}

void MonitorsGround::autoRebound()
{

    for (auto pw : m_monitors.keys()) {
        //设置位置
        pw->setPos(pw->getPreCenter());
        pw->update();
    }
}

//屏幕示意图平移居中
void MonitorsGround::centeredMonitorsView()
{
    if (m_monitors.size() == 1) {
        singleScreenAdjest();
    } else {
        setEnabled(true);

        QPointF dPos = sceneRect().center() - scene()->itemsBoundingRect().center();
        for (auto pw : m_monitors.keys()) {
            //保存上一次调整的数据,用与在不满足调整条件的时候自动恢复的功能.
            pw->setPreCenter(pw->pos());
            pw->setPos(pw->pos() + dPos);
        }

        if (!m_isInit)
            updateConnectedState(true);//更新初始化连接数据
    }

    for (auto pw : m_monitors.keys()) {
        pw->update();
    }

    m_isInit = true;
}
