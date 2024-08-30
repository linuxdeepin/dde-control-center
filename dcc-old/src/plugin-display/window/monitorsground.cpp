// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "monitorsground.h"

#include "monitorproxywidget.h"
#include "operation/displaymodel.h"

#include <DPaletteHelper>

#include <QGraphicsScene>
#include <QScrollArea>
#include <QScroller>
#include <QTimer>
#include <QWidget>

#include <float.h>
#include <math.h>

using namespace DCC_NAMESPACE;
constexpr int MIN_W = 484; // 窗口的最小宽度
constexpr int MAX_W = 726; // 窗口的最大宽度

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

    // 自动填色和无边框
    setAutoFillBackground(true);
    setStyleSheet("border:0px");
    DPalette pa = DPaletteHelper::instance()->palette(parentWidget());
    QColor curThemeColor = pa.base().color();
    curThemeColor.setAlphaF(0.001); // 设置透明度
    pa.setBrush(QPalette::Base, QBrush(curThemeColor));
    DPaletteHelper::instance()->setPalette(this, pa);

    // 添加场景
    setScene(&m_graphicsScene);

    Q_EMIT setEffectiveReminderVisible(false, m_nEffectiveTime);

    m_effectiveTimer->setInterval(m_nEffectiveTime * 1000);
    m_effectiveTimer->setSingleShot(true);
    connect(m_effectiveTimer, &QTimer::timeout, this, [=]() {
        Q_EMIT setEffectiveReminderVisible(false, m_nEffectiveTime);
        for (auto mon : m_model->monitorList()) {
            disconnect(mon, &Monitor::geometryChanged, this, &MonitorsGround::onGeometryChanged);
        }

        applySettings();
        Q_EMIT requestMonitorRelease(m_monitors[m_movingItem]);
        m_effectiveTimer->setInterval(m_nEffectiveTime * 1000);
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
    m_mapItemConnectedState.clear();
    m_mapInitItemConnectedState.clear();
    m_lstSortItems.clear();
    m_graphicsScene.clear();
    m_model = model;

    if (!moni) {
        m_isSingleDisplay = false;
        for (auto mon : model->monitorList()) {
            initMonitorProxyWidget(mon);
        }

        for (auto pw : m_monitors.keys()) {
            if (!m_isSingleDisplay)
                pw->moveBy(pw->x(), pw->y());
        }

        for (auto mon : model->monitorList()) {
            connect(mon, &Monitor::rotateChanged, this, &MonitorsGround::onRotateChanged);
            connect(mon, &Monitor::currentModeChanged, this, &MonitorsGround::onCurrentModeChanged);
            connect(mon, &Monitor::geometryChanged, this, &MonitorsGround::onGeometryChanged);
        }

        connect(m_model,
                &DisplayModel::primaryScreenChanged,
                this,
                static_cast<void (MonitorsGround::*)()>(&MonitorsGround::update),
                Qt::QueuedConnection);
    } else {
        initMonitorProxyWidget(moni);
        connect(moni, &Monitor::geometryChanged, this, &MonitorsGround::onGeometryChanged);
        connect(moni, &Monitor::currentModeChanged, this, &MonitorsGround::onCurrentModeChanged);
        connect(moni, &Monitor::rotateChanged, this, &MonitorsGround::onRotateChanged);
        m_isSingleDisplay = true;
    }
    setEnabled(!m_isSingleDisplay && m_model->displayMode() == EXTEND_MODE);
    QTimer::singleShot(1, this, [=]() {
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

    connect(pw,
            &MonitorProxyWidget::requestMonitorPress,
            this,
            &MonitorsGround::requestMonitorPress);
    connect(pw, &MonitorProxyWidget::requestMonitorPress, this, [this, pw]() {
        for (auto item : m_monitors.keys()) {
            item->setZValue(0);
        }
        m_movingItem = pw;
        m_movingItem->setZValue(1);
    });
    connect(pw,
            &MonitorProxyWidget::requestMonitorRelease,
            this,
            &MonitorsGround::onRequestMonitorRelease);
    connect(pw, &MonitorProxyWidget::requestMouseMove, this, &MonitorsGround::onRequestMouseMove);
    connect(pw, &MonitorProxyWidget::requestKeyPress, this, &MonitorsGround::onRequestKeyPress);
}

void MonitorsGround::onGeometryChanged()
{
    for (auto pw : m_monitors.keys()) {
        Monitor *mon = qobject_cast<Monitor *>(sender());
        if (mon->name() == m_monitors[pw]->name()) {
            m_movingItem = pw;
            pw->update();
            break;
        }
    }

    for (auto pw : m_monitors.keys()) {
        pw->setMovedX(m_monitors[pw]->x());
        pw->setMovedY(m_monitors[pw]->y());
        pw->setPos(0, 0);
        if (!m_isSingleDisplay)
            pw->moveBy(pw->x(), pw->y());
    }

    onResize();
}

void MonitorsGround::onRotateChanged()
{
    onCurrentModeChanged();
}

// 当方向改变时 当分辨率改变时
void MonitorsGround::onCurrentModeChanged()
{
    m_nEffectiveTime = 0;
    m_effectiveTimer->setInterval(m_nEffectiveTime);
    onResize();
    if (m_setMergeMode || m_isSingleDisplay)
        return;

    Monitor *mon = qobject_cast<Monitor *>(sender());
    for (auto pw : m_monitors.keys()) {
        if (mon->name() == m_monitors[pw]->name()) {
            m_movingItem = pw;
            pw->update();
            break;
        }
    }

    executemultiScreenAlgo(false);
}

void MonitorsGround::executemultiScreenAlgo(const bool isRebound)
{
    if (m_isSingleDisplay)
        return;

    bool isRestore = false;
    multiScreenSortAlgo(isRestore, isRebound);

    // 需要重新拼接
    if (m_movingItem->getReSplicing()) {
        m_movingItem->setReSplicing(false);
        multiScreenSortAlgo(isRestore, true);
    }

    if (isRestore == true)
        return;

    multiScreenAutoAdjust();
    onResize();
    updateConnectedState();

    // 显示设置生效倒计时提示框
    Q_EMIT setEffectiveReminderVisible(true, m_nEffectiveTime);
    m_effectiveTimer->start();
}

void MonitorsGround::onRequestMonitorRelease()
{
    executemultiScreenAlgo(true);
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

    const double scaleW = double(width()) / (m_monitors.values().first()->w() * 1.2);
    const double scaleH = double(height()) / (m_monitors.values().first()->h() * 1.2);

    const double scale = std::min(scaleW, scaleH);

    int cnt = 0;
    int offset = static_cast<int>(10 / scale);

    for (auto pw : m_monitors.keys()) {
        pw->setZValue(cnt);
        pw->setPos(QPointF(offset * cnt, offset * cnt));
        cnt++;
    }

    QPointF dPos = sceneRect().center() - scene()->itemsBoundingRect().center();
    for (auto pw : m_monitors.keys()) {
        pw->setPos(pw->pos() + dPos);
    }

    resetTransform();
    this->scale(scale, scale);
}

void MonitorsGround::onResize()
{
    if (!m_isSingleDisplay) {
        if (size().width() < MAX_W) {
            setSceneRect(QRect(0, 0, width(), 240 + size().width() - MIN_W));
            setFixedHeight(240 + size().width() - MIN_W);
        } else {
            setSceneRect(QRect(0, 0, width(), 240 + MAX_W - MIN_W));
            setFixedHeight(240 + MAX_W - MIN_W);
        }
        updateScale();
    } else {
        setSceneRect(QRect(0, 0, width(), height()));
    }

    resetMonitorsView();
}

void MonitorsGround::resizeEvent(QResizeEvent *event)
{
    // 解决最大化view高度设置不能及时生效的问题
    QTimer::singleShot(0, this, [=] {
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
        QScroller::grabGesture(m_scrollArea->viewport(), QScroller::LeftMouseButtonGesture);
}

void MonitorsGround::paintEvent(QPaintEvent *event)
{
    if (m_isSingleDisplay || m_model->displayMode() != EXTEND_MODE)
        this->setEnabled(false);

    QGraphicsView::paintEvent(event);
}

void MonitorsGround::singleScreenAdjest()
{
    const double scaleW = double(width()) / (m_monitors.keys().last()->w() * 1.5);
    const double scaleH = double(height()) / (m_monitors.keys().last()->h() * 1.5);

    const double scale = std::min(scaleW, scaleH);

    QPointF dPos = sceneRect().center() - scene()->itemsBoundingRect().center();
    m_monitors.keys().last()->setPos(m_monitors.keys().last()->pos() + dPos);
    resetTransform();
    this->scale(scale, scale);
    setEnabled(false);
}

// 应用设置
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
    std::sort(lstX.begin(), lstX.end(), [=](const qreal item1, const qreal item2) {
        return item1 < item2;
    });
    std::sort(lstY.begin(), lstY.end(), [=](const qreal item1, const qreal item2) {
        return item1 < item2;
    });

    for (auto it(m_monitors.cbegin()); it != m_monitors.cend(); ++it) {
        monitorPosition.insert(it.value(),
                               QPair<int, int>(int(it.key()->pos().x() - lstX.first()),
                                               int(it.key()->pos().y() - lstY.first())));
        qWarning() << "applySettings" << it.value()->name() << monitorPosition[it.value()];
    }

    // 在计算之后再做一次过滤
    // 消除一个像素的重叠
    // 首先获取所以的拼接点到list
    QList<QList<Monitor *>> lstMonitor;
    for (auto iter = m_mapItemConnectedState.begin(); iter != m_mapItemConnectedState.end();
         ++iter) {
        for (auto item : iter.value()) {
            QList<Monitor *> lstMonitorTemp;
            lstMonitorTemp << m_monitors[iter.key()] << m_monitors[item];
            lstMonitor.append(lstMonitorTemp);
        }
    }

    // 依据字符串进行排序
    QMap<QString, QList<Monitor *>> mapMonitor;
    for (auto m : lstMonitor) {
        std::sort(m.begin(), m.end(), [=](const Monitor *m1, const Monitor *m2) {
            return m1->name() < m2->name();
        });
        // 去重
        mapMonitor.insert(m.first()->name() + m.last()->name(), m);
        qDebug() << "mapMonitor" << m.first()->name() + m.last()->name();
    }

    // 获取到实际相交点的信息
    for (auto m : mapMonitor.values()) {
        // 左右坐标
        // 上下坐标 对比
        QRect m1(monitorPosition[m.first()].first,
                 monitorPosition[m.first()].second,
                 m.first()->w(),
                 m.first()->h());
        QRect m2(monitorPosition[m.last()].first,
                 monitorPosition[m.last()].second,
                 m.last()->w(),
                 m.last()->h());

        qDebug() << "获取实际交点" << mapMonitor.key(m) << "1:" << m1 << "2:" << m2;

        // 左右相接
        if (m1.left() < m2.left() && m1.right() == m2.left()) {
            monitorPosition[m.last()] = (QPair<int, int>(m1.right() + 1, m2.y()));
        } else if (m1.left() > m2.left() && m2.right() == m1.left()) {
            monitorPosition[m.first()] = (QPair<int, int>(m2.right() + 1, m1.y()));
        }

        // 上下相接
        if (m1.top() < m2.top() && m1.bottom() == m2.top()) {
            monitorPosition[m.last()] = (QPair<int, int>(m2.x(), m1.bottom() + 1));
        } else if (m1.top() > m2.top() && m2.bottom() == m1.top()) {
            monitorPosition[m.first()] = (QPair<int, int>(m1.x(), m2.bottom() + 1));
        }
    }

    for (auto it(m_monitors.cbegin()); it != m_monitors.cend(); ++it) {
        qWarning() << "applySettings 处理之后:" << it.value()->name()
                   << monitorPosition[it.value()];
    }

    Q_EMIT requestApplySettings(monitorPosition);
}

/*1050-5401*/
// 多屏排序算法
QPointF MonitorsGround::multiScreenSortAlgo(bool &isRestore, const bool isRebound)
{
    // 排列算法
    // 1、寻找位置变化的屏幕块，确定为被移动的块
    // 2、判断被移动块和其他块之间的位置关系来进行图形拼接移动
    //*根据中心点判定

    QList<MonitorProxyWidget *> lstActivedItemsX;
    QList<MonitorProxyWidget *> lstActivedItemsY;
    QList<MonitorProxyWidget *> lstNoMovedItems;
    QList<MonitorProxyWidget *> lstShelterItems;

    m_lstMoveingItemToCenterPosLen.clear();

    bool isMove = true;
    isRestore = false;
    bool isAutoAdsorption = false;
    bool isIntersect = false;
    qreal g_dx = 0.0;
    qreal g_dy = 0.0;

    qreal intersectedArea = 0.0; // 相交的面积

    QRectF moveItemIntersect, moveItemRect;

    bool g_bXYTogetherMoved =
            false; // 标志XY方向是否一起移动，其余情况按哪个方向离得近向哪个方向移动

    moveItemIntersect = m_movingItem->mapRectToScene(m_movingItem->justIntersectRect());
    moveItemRect = m_movingItem->mapRectToScene(m_movingItem->boundingRect());

    auto mapToSceneIntersectRect = [=](MonitorProxyWidget *item) {
        return item->mapToScene(item->justIntersectRect());
    };

    auto mapToSceneBoundingRect = [=](MonitorProxyWidget *item) {
        return item->mapToScene(item->boundingRect());
    };

    for (int i = 0; i < m_lstSortItems.size(); i++) {

        if (m_movingItem != m_lstSortItems[i]) {
            QRectF rect = moveItemIntersect.intersected(
                    m_lstSortItems[i]->mapRectToScene(m_lstSortItems[i]->boundingRect()));
            intersectedArea += rect.width() * rect.height();

            // 移动块完全覆盖一个块
            if (moveItemRect.contains(
                        m_lstSortItems[i]->mapRectToScene(m_lstSortItems[i]->boundingRect()))) {
                lstShelterItems.append(m_lstSortItems[i]);
            }

            // 要么不相交,要相交就是要点线相交的
            if (!mapToSceneIntersectRect(m_movingItem)
                         .intersects(mapToSceneBoundingRect(m_lstSortItems[i]))
                && mapToSceneBoundingRect(m_movingItem)
                           .intersects(mapToSceneBoundingRect(m_lstSortItems[i]))) {
                isAutoAdsorption = true;
            }
            // 出现相交的情况
            if (mapToSceneIntersectRect(m_movingItem)
                        .intersects(mapToSceneBoundingRect(m_lstSortItems[i]))
                && mapToSceneBoundingRect(m_movingItem)
                           .intersects(mapToSceneBoundingRect(m_lstSortItems[i]))) {
                isIntersect = true;
            }

            lstNoMovedItems.append(m_lstSortItems[i]);
        }
    }

    // 移动块被完全包含在其他的块
    if (qFuzzyCompare(moveItemIntersect.width() * moveItemIntersect.height(), intersectedArea)) {
        qDebug() << "存在包含关系";
        lstShelterItems.append(m_movingItem);
    }

    // 移动块到其他块中心点的距离排序
    QPointF movedPos = m_movingItem->mapToScene(m_movingItem->boundingRect().center());
    for (auto item : lstNoMovedItems) {
        QPointF tempPos = item->mapToScene(item->boundingRect().center());
        qreal len = sqrt(pow(tempPos.x() - movedPos.x(), 2) + pow(tempPos.y() - movedPos.y(), 2));
        m_lstMoveingItemToCenterPosLen.append(qMakePair(item, len));
    }

    // 获取距离最近的块
    std::sort(m_lstMoveingItemToCenterPosLen.begin(),
              m_lstMoveingItemToCenterPosLen.end(),
              [=](const QPair<MonitorProxyWidget *, qreal> &item1,
                  const QPair<MonitorProxyWidget *, qreal> &item2) {
                  return item1.second < item2.second;
              });

    // 自动回弹的触发条件: 1. 一个屏幕完全包含另一个屏的时候 2. 一个屏幕剩下的屏幕集合所包围
    if (lstShelterItems.size() > 0) {
        if (isMove) {

            if (isRebound) {
                autoRebound();
                qDebug() << "自动回弹流程触发! == " << lstShelterItems.size();
                isRestore = true;
                return QPointF(0.0, 0.0);
            } else {
                // 当改变方向时出现覆盖多个块的情况，会触发自动会弹流程，但是改变方向的上一个操作状态不存在或者说回弹到上一个状态没有意义，会导致屏幕重叠现象
                // 如果是由于方向改变导致的重叠，那就将此块移动到items外接矩形的左下角再重新执行拼接算法。
                m_movingItem->moveBy(
                        scene()->itemsBoundingRect().right()
                                - m_movingItem
                                          ->mapToScene(m_movingItem->boundingRect().bottomLeft())
                                          .x(),
                        scene()->itemsBoundingRect().bottom()
                                - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft())
                                          .y());
                m_movingItem->setReSplicing(true);
                return QPointF(0.0, 0.0);
            }
        }
    } else { // 没有完全重合的情况处理逻辑

        std::sort(lstNoMovedItems.begin(),
                  lstNoMovedItems.end(),
                  [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                      return item1->mapToScene(item1->boundingRect().bottomLeft()).x()
                              < item2->mapToScene(item2->boundingRect().bottomLeft()).x();
                  });

        // 将MovedItem放入链表中排序，左排 右排，判定覆盖几个块
        lstNoMovedItems.append(m_movingItem);
        // 左排序
        std::sort(lstNoMovedItems.begin(),
                  lstNoMovedItems.end(),
                  [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                      if (item1 == m_movingItem) {
                          return item1->mapToScene(item1->boundingRect().bottomRight()).x()
                                  < item2->mapToScene(item2->boundingRect().bottomLeft()).x();
                      } else if (item2 == m_movingItem) {
                          return item1->mapToScene(item1->boundingRect().bottomLeft()).x()
                                  < item2->mapToScene(item2->boundingRect().bottomRight()).x();
                      }
                      return item1->mapToScene(item1->boundingRect().bottomLeft()).x()
                              < item2->mapToScene(item2->boundingRect().bottomLeft()).x();
                  });
        int nBIndexLeft = lstNoMovedItems.indexOf(m_movingItem);

        // 右排序
        std::sort(lstNoMovedItems.begin(),
                  lstNoMovedItems.end(),
                  [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                      if (item1 == m_movingItem) {
                          return item1->mapToScene(item1->boundingRect().bottomLeft()).x()
                                  < item2->mapToScene(item2->boundingRect().bottomRight()).x();
                      } else if (item2 == m_movingItem) {
                          return item1->mapToScene(item1->boundingRect().bottomRight()).x()
                                  < item2->mapToScene(item2->boundingRect().bottomLeft()).x();
                      }
                      return item1->mapToScene(item1->boundingRect().bottomRight()).x()
                              < item2->mapToScene(item2->boundingRect().bottomRight()).x();
                  });
        int nBIndexRight = lstNoMovedItems.indexOf(m_movingItem);

        // 上排序
        std::sort(lstNoMovedItems.begin(),
                  lstNoMovedItems.end(),
                  [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                      if (item1 == m_movingItem) {
                          return item1->mapToScene(item1->boundingRect().bottomLeft()).y()
                                  < item2->mapToScene(item2->boundingRect().topLeft()).y();
                      } else if (item2 == m_movingItem) {
                          return item1->mapToScene(item1->boundingRect().topLeft()).y()
                                  < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
                      }
                      return item1->mapToScene(item1->boundingRect().topLeft()).y()
                              < item2->mapToScene(item2->boundingRect().topLeft()).y();
                  });
        int nBIndexTop = lstNoMovedItems.indexOf(m_movingItem);

        // 下排序
        std::sort(lstNoMovedItems.begin(),
                  lstNoMovedItems.end(),
                  [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                      if (item1 == m_movingItem) {
                          return item1->mapToScene(item1->boundingRect().topLeft()).y()
                                  < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
                      } else if (item2 == m_movingItem) {
                          return item1->mapToScene(item1->boundingRect().bottomLeft()).y()
                                  < item2->mapToScene(item2->boundingRect().topLeft()).y();
                      }
                      return item1->mapToScene(item1->boundingRect().bottomLeft()).y()
                              < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
                  });
        int nBIndexBottom = lstNoMovedItems.indexOf(m_movingItem);

        int nItemSize = lstNoMovedItems.size() - 1;

        // 说明移动块在其他块X坐标有重合
        if (nBIndexLeft != 0 && nBIndexRight != nItemSize) {

            // 移动X
            // 将MovedItem放入链表中排序，上排 下排，判定在Y轴移动的方向和距离
            // 处理
            qreal movex1 = m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).x();
            qreal movex2 = m_movingItem->mapToScene(m_movingItem->boundingRect().topRight()).x();

            for (auto pair : m_lstMoveingItemToCenterPosLen) {
                qreal x1 = pair.first->mapToScene(pair.first->boundingRect().topLeft()).x();
                qreal x2 = pair.first->mapToScene(pair.first->boundingRect().topRight()).x();

                if (!((x1 >= movex1 && x1 >= movex2) || (x2 <= movex1 && x2 <= movex2))) {
                    lstActivedItemsX.append(pair.first);
                }
            }

            lstActivedItemsX.append(m_movingItem);
            // 上排序
            std::sort(lstActivedItemsX.begin(),
                      lstActivedItemsX.end(),
                      [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                          return item1->mapToScene(item1->boundingRect().topLeft()).y()
                                  < item2->mapToScene(item2->boundingRect().topLeft()).y();
                      });
            int nIndexTop = lstActivedItemsX.indexOf(m_movingItem);

            // 下排序
            std::sort(lstActivedItemsX.begin(),
                      lstActivedItemsX.end(),
                      [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                          return item1->mapToScene(item1->boundingRect().bottomLeft()).y()
                                  < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
                      });
            int nIndexBottom = lstActivedItemsX.indexOf(m_movingItem);

            // 先做预移动，如果有重合，把重合块加入激活块重新排序
            if (nIndexTop == nIndexBottom) {
                // 移动块在最上面
                if (nIndexTop == 0) {
                    qreal dy =
                            lstActivedItemsX.first()
                                    ->mapToScene(lstActivedItemsX.first()->boundingRect().topLeft())
                                    .y()
                            - m_movingItem->mapToScene(m_movingItem->boundingRect().bottomLeft())
                                      .y();
                    m_movingItem->moveBy(0, dy);
                    m_graphicsScene.update();

                    QList<MonitorProxyWidget *> lstShelterItemsTemp;
                    lstNoMovedItems.removeOne(m_movingItem);
                    for (int i = 0; i < lstNoMovedItems.size(); i++) {
                        if (m_movingItem->mapToScene(m_movingItem->boundingRect())
                                    .intersects(lstNoMovedItems[i]->mapToScene(
                                            lstNoMovedItems[i]->boundingRect()))) {
                            lstShelterItemsTemp.append(lstNoMovedItems[i]);
                        }
                    }

                    if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(0, -dy); // 先移回去
                        m_graphicsScene.update();

                        // 下排序
                        std::sort(
                                lstActivedItemsX.begin(),
                                lstActivedItemsX.end(),
                                [=](const MonitorProxyWidget *item1,
                                    const MonitorProxyWidget *item2) {
                                    return item1->mapToScene(item1->boundingRect().bottomLeft()).y()
                                            < item2->mapToScene(item2->boundingRect().bottomLeft())
                                                      .y();
                                });
                        // int nIndexBottomTemp = lstActivedItemsX.indexOf(m_movingItem);

                        // 上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsX.contains(t))
                                lstActivedItemsX.append(t);
                        }
                        std::sort(lstActivedItemsX.begin(),
                                  lstActivedItemsX.end(),
                                  [=](const MonitorProxyWidget *item1,
                                      const MonitorProxyWidget *item2) {
                                      return item1->mapToScene(item1->boundingRect().topLeft()).y()
                                              < item2->mapToScene(item2->boundingRect().topLeft())
                                                        .y();
                                  });
                        int nIndexTopTemp = lstActivedItemsX.indexOf(m_movingItem);

                        lstActivedItemsX.removeOne(m_movingItem);

                        if (!lstActivedItemsX.isEmpty()) {
                            if (nIndexTopTemp == 0) {
                                g_dy = lstActivedItemsX.first()
                                                ->mapToScene(lstActivedItemsX.first()
                                                                     ->boundingRect()
                                                                     .topLeft())
                                                .y()
                                        - m_movingItem
                                                  ->mapToScene(
                                                          m_movingItem->boundingRect().bottomLeft())
                                                  .y();
                                qDebug() << "g_dy" << g_dy;
                            } else {
                                g_dy = lstActivedItemsX.last()
                                                ->mapToScene(lstActivedItemsX.last()
                                                                     ->boundingRect()
                                                                     .bottomLeft())
                                                .y()
                                        - m_movingItem
                                                  ->mapToScene(
                                                          m_movingItem->boundingRect().topLeft())
                                                  .y();
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
                    std::sort(
                            lstActivedItemsX.begin(),
                            lstActivedItemsX.end(),
                            [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                                return item1->mapToScene(item1->boundingRect().bottomLeft()).y()
                                        < item2->mapToScene(item2->boundingRect().bottomLeft()).y();
                            });

                    qreal dy = lstActivedItemsX.last()
                                       ->mapToScene(
                                               lstActivedItemsX.last()->boundingRect().bottomLeft())
                                       .y()
                            - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).y();
                    m_movingItem->moveBy(0, dy);
                    m_graphicsScene.update();

                    lstNoMovedItems.removeOne(m_movingItem);
                    QList<MonitorProxyWidget *> lstShelterItemsTemp;
                    for (int i = 0; i < lstNoMovedItems.size(); i++) {
                        if (m_movingItem->mapToScene(m_movingItem->boundingRect())
                                    .intersects(lstNoMovedItems[i]->mapToScene(
                                            lstNoMovedItems[i]->boundingRect()))) {
                            lstShelterItemsTemp.append(lstNoMovedItems[i]);
                        }
                    }

                    if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(0, -dy); // 先移回去
                        m_graphicsScene.update();

                        // 上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsX.contains(t))
                                lstActivedItemsX.append(t);
                        }

                        std::sort(lstActivedItemsX.begin(),
                                  lstActivedItemsX.end(),
                                  [=](const MonitorProxyWidget *item1,
                                      const MonitorProxyWidget *item2) {
                                      return item1->mapToScene(item1->boundingRect().topLeft()).y()
                                              < item2->mapToScene(item2->boundingRect().topLeft())
                                                        .y();
                                  });
                        int nIndexTopTemp = lstActivedItemsX.indexOf(m_movingItem);

                        // 下排序
                        std::sort(
                                lstActivedItemsX.begin(),
                                lstActivedItemsX.end(),
                                [=](const MonitorProxyWidget *item1,
                                    const MonitorProxyWidget *item2) {
                                    return item1->mapToScene(item1->boundingRect().bottomLeft()).y()
                                            < item2->mapToScene(item2->boundingRect().bottomLeft())
                                                      .y();
                                });
                        // int nIndexBottomTemp = lstActivedItemsX.indexOf(m_movingItem);

                        lstActivedItemsX.removeOne(m_movingItem);

                        if (!lstActivedItemsX.isEmpty()) {
                            if (nIndexTopTemp == 0) {
                                g_dy = lstActivedItemsX.first()
                                                ->mapToScene(lstActivedItemsX.first()
                                                                     ->boundingRect()
                                                                     .topLeft())
                                                .y()
                                        - m_movingItem
                                                  ->mapToScene(
                                                          m_movingItem->boundingRect().bottomLeft())
                                                  .y();
                                qDebug() << "g_dy" << g_dy;
                            } else {
                                g_dy = lstActivedItemsX.last()
                                                ->mapToScene(lstActivedItemsX.last()
                                                                     ->boundingRect()
                                                                     .bottomLeft())
                                                .y()
                                        - m_movingItem
                                                  ->mapToScene(
                                                          m_movingItem->boundingRect().topLeft())
                                                  .y();
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
            qreal movey1 = m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).y();
            qreal movey2 = m_movingItem->mapToScene(m_movingItem->boundingRect().bottomLeft()).y();

            for (auto pair : m_lstMoveingItemToCenterPosLen) {
                qreal y1 = pair.first->mapToScene(pair.first->boundingRect().topLeft()).y();
                qreal y2 = pair.first->mapToScene(pair.first->boundingRect().bottomLeft()).y();

                if (!((y1 >= movey1 && y1 >= movey2) || (y2 <= movey1 && y2 <= movey2))) {
                    lstActivedItemsY.append(pair.first);
                }
            }

            lstActivedItemsY.append(m_movingItem);
            // 左排序
            std::sort(lstActivedItemsY.begin(),
                      lstActivedItemsY.end(),
                      [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                          return item1->mapToScene(item1->boundingRect().topLeft()).x()
                                  < item2->mapToScene(item2->boundingRect().topLeft()).x();
                      });
            int nIndexLeft = lstActivedItemsY.indexOf(m_movingItem);

            // 右排序
            std::sort(lstActivedItemsY.begin(),
                      lstActivedItemsY.end(),
                      [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                          return item1->mapToScene(item1->boundingRect().topRight()).x()
                                  < item2->mapToScene(item2->boundingRect().topRight()).x();
                      });
            int nIndexRight = lstActivedItemsY.indexOf(m_movingItem);

            if (nIndexLeft == nIndexRight) {
                // 移动块在最右侧
                if (nIndexLeft == 0) {
                    qreal dx =
                            lstActivedItemsY.first()
                                    ->mapToScene(lstActivedItemsY.first()->boundingRect().topLeft())
                                    .x()
                            - m_movingItem->mapToScene(m_movingItem->boundingRect().topRight()).x();
                    m_movingItem->moveBy(dx, 0);
                    m_graphicsScene.update();

                    QList<MonitorProxyWidget *> lstShelterItemsTemp;
                    lstNoMovedItems.removeOne(m_movingItem);
                    for (int i = 0; i < lstNoMovedItems.size(); i++) {
                        if (m_movingItem->mapToScene(m_movingItem->boundingRect())
                                    .intersects(lstNoMovedItems[i]->mapToScene(
                                            lstNoMovedItems[i]->boundingRect()))) {
                            lstShelterItemsTemp.append(lstNoMovedItems[i]);
                        }
                    }

                    if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(-dx, 0);
                        m_graphicsScene.update();

                        // 下排序
                        std::sort(lstActivedItemsY.begin(),
                                  lstActivedItemsY.end(),
                                  [=](const MonitorProxyWidget *item1,
                                      const MonitorProxyWidget *item2) {
                                      return item1->mapToScene(item1->boundingRect().topRight()).x()
                                              < item2->mapToScene(item2->boundingRect().topRight())
                                                        .x();
                                  });
                        // int nIndexRightTemp = lstActivedItemsY.indexOf(m_movingItem);

                        // 上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsY.contains(t))
                                lstActivedItemsY.append(t);
                        }
                        std::sort(lstActivedItemsY.begin(),
                                  lstActivedItemsY.end(),
                                  [=](const MonitorProxyWidget *item1,
                                      const MonitorProxyWidget *item2) {
                                      return item1->mapToScene(item1->boundingRect().topLeft()).x()
                                              < item2->mapToScene(item2->boundingRect().topLeft())
                                                        .x();
                                  });
                        int nIndexLeftTemp = lstActivedItemsY.indexOf(m_movingItem);

                        lstActivedItemsY.removeOne(m_movingItem);

                        if (!lstActivedItemsY.isEmpty()) {
                            if (nIndexLeftTemp == 0) {
                                g_dx = lstActivedItemsY.first()
                                                ->mapToScene(lstActivedItemsY.first()
                                                                     ->boundingRect()
                                                                     .topLeft())
                                                .x()
                                        - m_movingItem
                                                  ->mapToScene(
                                                          m_movingItem->boundingRect().topRight())
                                                  .x();
                                qDebug() << "g_dx" << g_dx;
                            } else {
                                g_dx = lstActivedItemsY.last()
                                                ->mapToScene(lstActivedItemsY.last()
                                                                     ->boundingRect()
                                                                     .topRight())
                                                .x()
                                        - m_movingItem
                                                  ->mapToScene(
                                                          m_movingItem->boundingRect().topLeft())
                                                  .x();
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
                    std::sort(
                            lstActivedItemsY.begin(),
                            lstActivedItemsY.end(),
                            [=](const MonitorProxyWidget *item1, const MonitorProxyWidget *item2) {
                                return item1->mapToScene(item1->boundingRect().topRight()).x()
                                        < item2->mapToScene(item2->boundingRect().topRight()).x();
                            });

                    qreal dx =
                            lstActivedItemsY.last()
                                    ->mapToScene(lstActivedItemsY.last()->boundingRect().topRight())
                                    .x()
                            - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft()).x();
                    m_movingItem->moveBy(dx, 0);
                    m_graphicsScene.update();

                    lstNoMovedItems.removeOne(m_movingItem);
                    QList<MonitorProxyWidget *> lstShelterItemsTemp;
                    for (int i = 0; i < lstNoMovedItems.size(); i++) {
                        if (m_movingItem->mapToScene(m_movingItem->boundingRect())
                                    .intersects(lstNoMovedItems[i]->mapToScene(
                                            lstNoMovedItems[i]->boundingRect()))) {
                            lstShelterItemsTemp.append(lstNoMovedItems[i]);
                        }
                    }

                    if (lstShelterItemsTemp.size() >= 0) {
                        m_movingItem->moveBy(-dx, 0);
                        m_graphicsScene.update();

                        // 上排序
                        for (auto t : lstShelterItemsTemp) {
                            if (!lstActivedItemsY.contains(t))
                                lstActivedItemsY.append(t);
                        }
                        std::sort(lstActivedItemsY.begin(),
                                  lstActivedItemsY.end(),
                                  [=](const MonitorProxyWidget *item1,
                                      const MonitorProxyWidget *item2) {
                                      return item1->mapToScene(item1->boundingRect().topLeft()).x()
                                              < item2->mapToScene(item2->boundingRect().topLeft())
                                                        .x();
                                  });
                        int nIndexLeftTemp = lstActivedItemsY.indexOf(m_movingItem);

                        // 下排序
                        std::sort(lstActivedItemsY.begin(),
                                  lstActivedItemsY.end(),
                                  [=](const MonitorProxyWidget *item1,
                                      const MonitorProxyWidget *item2) {
                                      return item1->mapToScene(item1->boundingRect().topRight()).x()
                                              < item2->mapToScene(item2->boundingRect().topRight())
                                                        .x();
                                  });
                        // int nIndexRightTemp = lstActivedItemsY.indexOf(m_movingItem);

                        lstActivedItemsY.removeOne(m_movingItem);

                        if (!lstActivedItemsY.isEmpty()) {
                            if (nIndexLeftTemp == 0) {
                                g_dx = lstActivedItemsY.first()
                                                ->mapToScene(lstActivedItemsY.first()
                                                                     ->boundingRect()
                                                                     .topLeft())
                                                .x()
                                        - m_movingItem
                                                  ->mapToScene(
                                                          m_movingItem->boundingRect().topRight())
                                                  .x();
                                qDebug() << "g_dx" << g_dx;
                            } else {
                                g_dx = lstActivedItemsY.last()
                                                ->mapToScene(lstActivedItemsY.last()
                                                                     ->boundingRect()
                                                                     .topRight())
                                                .x()
                                        - m_movingItem
                                                  ->mapToScene(
                                                          m_movingItem->boundingRect().topLeft())
                                                  .x();
                                qDebug() << "g_dx" << g_dx;
                            }
                        }
                    }
                }
            }
        }

        // 说明移动块在其他块的四周
        if ((nBIndexLeft == 0 || nBIndexRight == nItemSize)
            || (nBIndexTop == 0 || nBIndexBottom == nItemSize)) {
            // 表示X方向没有重叠的
            // TODO：
            // 找距离他最近的顶点
            // LT 左上
            lstNoMovedItems.removeOne(m_movingItem);
            QRectF movedRectF = m_movingItem->mapRectToScene(m_movingItem->boundingRect());
            QList<QPair<MonitorProxyWidget *, qreal>> lstTempLen;
            if (nBIndexLeft == 0 && nBIndexTop == 0) {
                for (auto item : lstNoMovedItems) {
                    QPointF tempPos = item->mapToScene(item->boundingRect().topLeft());
                    qreal len = sqrt(pow(tempPos.x() - movedRectF.bottomRight().x(), 2)
                                     + pow(tempPos.y() - movedRectF.bottomRight().y(), 2));
                    lstTempLen.append(qMakePair(item, len));
                }
                // 获取距离最近的块
                std::sort(lstTempLen.begin(),
                          lstTempLen.end(),
                          [=](const QPair<MonitorProxyWidget *, qreal> &item1,
                              const QPair<MonitorProxyWidget *, qreal> &item2) {
                              return item1.second < item2.second;
                          });

                QPointF dPos = lstTempLen.first().first->mapToScene(
                                       lstTempLen.first().first->boundingRect().topLeft())
                        - m_movingItem->mapToScene(m_movingItem->boundingRect().bottomRight());
                g_dx = dPos.x();
                g_dy = dPos.y();
                qDebug() << "g_dx" << g_dx;
                qDebug() << "g_dy" << g_dy;
                g_bXYTogetherMoved = true;
            }

            // LB 左下
            if (nBIndexLeft == 0 && nBIndexBottom == nItemSize) {
                for (auto item : lstNoMovedItems) {
                    QPointF tempPos = item->mapToScene(item->boundingRect().bottomLeft());
                    qreal len = sqrt(pow(tempPos.x() - movedRectF.topRight().x(), 2)
                                     + pow(tempPos.y() - movedRectF.topRight().y(), 2));
                    lstTempLen.append(qMakePair(item, len));
                }
                // 获取距离最近的块
                std::sort(lstTempLen.begin(),
                          lstTempLen.end(),
                          [=](const QPair<MonitorProxyWidget *, qreal> &item1,
                              const QPair<MonitorProxyWidget *, qreal> &item2) {
                              return item1.second < item2.second;
                          });

                QPointF dPos = lstTempLen.first().first->mapToScene(
                                       lstTempLen.first().first->boundingRect().bottomLeft())
                        - m_movingItem->mapToScene(m_movingItem->boundingRect().topRight());
                g_dx = dPos.x();
                g_dy = dPos.y();
                g_bXYTogetherMoved = true;
                qDebug() << "g_dx" << g_dx;
                qDebug() << "g_dy" << g_dy;
            }

            // RT 右上
            if (nBIndexRight == nItemSize && nBIndexTop == 0) {
                for (auto item : lstNoMovedItems) {
                    QPointF tempPos = item->mapToScene(item->boundingRect().topRight());
                    qreal len = sqrt(pow(tempPos.x() - movedRectF.bottomLeft().x(), 2)
                                     + pow(tempPos.y() - movedRectF.bottomLeft().y(), 2));
                    lstTempLen.append(qMakePair(item, len));
                }
                // 获取距离最近的块
                std::sort(lstTempLen.begin(),
                          lstTempLen.end(),
                          [=](const QPair<MonitorProxyWidget *, qreal> &item1,
                              const QPair<MonitorProxyWidget *, qreal> &item2) {
                              return item1.second < item2.second;
                          });

                QPointF dPos = lstTempLen.first().first->mapToScene(
                                       lstTempLen.first().first->boundingRect().topRight())
                        - m_movingItem->mapToScene(m_movingItem->boundingRect().bottomLeft());
                g_dx = dPos.x();
                g_dy = dPos.y();
                g_bXYTogetherMoved = true;
                qDebug() << "g_dx" << g_dx;
                qDebug() << "g_dy" << g_dy;
            }

            // RB 右下

            if (nBIndexRight == nItemSize && nBIndexBottom == nItemSize) {
                for (auto item : lstNoMovedItems) {
                    QPointF tempPos = item->mapToScene(item->boundingRect().bottomRight());
                    qreal len = sqrt(pow(tempPos.x() - movedRectF.topLeft().x(), 2)
                                     + pow(tempPos.y() - movedRectF.topLeft().y(), 2));
                    lstTempLen.append(qMakePair(item, len));
                }
                // 获取距离最近的块
                std::sort(lstTempLen.begin(),
                          lstTempLen.end(),
                          [=](const QPair<MonitorProxyWidget *, qreal> &item1,
                              const QPair<MonitorProxyWidget *, qreal> &item2) {
                              return item1.second < item2.second;
                          });

                QPointF dPos = lstTempLen.first().first->mapToScene(
                                       lstTempLen.first().first->boundingRect().bottomRight())
                        - m_movingItem->mapToScene(m_movingItem->boundingRect().topLeft());
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
                    fabs(g_dx) < fabs(g_dy) ? m_movingItem->moveBy(g_dx, 0)
                                            : m_movingItem->moveBy(0, g_dy);
                else {
                    fabs(g_dx) < fabs(g_dy) ? dy = 0.0 : dx = 0.0;
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
        return QPointF(dx, dy);
    else {
        return QPointF(g_dx, g_dy);
    }
}

// 多屏自动调整
void MonitorsGround::multiScreenAutoAdjust()
{
    // 在自动拼接之后已经达成全连通状态,无效执行自动调整
    updateConnectedState();

    // 判断是否为全连通状态
    if (getConnectedDomain(m_movingItem).size() == m_lstItems.size()) {
        updateConnectedState(true);
        return;
    }

    // 存储移动块到临时变量
    MonitorProxyWidget *m_movingItemTemp = m_movingItem;
    QList<MonitorProxyWidget *> lstChangedItems = m_lstItems;

    // 处理之前变化的块为空则返回
    if (lstChangedItems.isEmpty())
        return;

    QMap<MonitorProxyWidget *, QList<MonitorProxyWidget *>> maplstItems;
    QMap<MonitorProxyWidget *, bool> mapItemsNeedMove;
    QList<MonitorProxyWidget *> itemTemp;

    // 这个列表是存放的就是所有移动块相关的块
    // 判断改变连通的块与移动块的剩余联通块是否存在连接
    // 【这种是在移动块初始连接块是两个及以上的情况下触发的，如果是一个连通块的话不会改变连通状态的】

    // 获取屏幕集群
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
        // 说明除了移动块剩下的被拆分成了几部分，需要重新向其中一个部分移动了
        // 找到中心屏幕集群(包含移动块)
        for (auto iter1 = maplstItems.begin(); iter1 != maplstItems.end(); ++iter1) {
            if (iter1.value().contains(m_movingItemTemp)) {
                m_lstSortItems = iter1.value();
                break;
            }
        }

        // 其他屏幕集群依次向中心屏幕集群移动
        for (auto iter = maplstItems.begin(); iter != maplstItems.end(); ++iter) {
            if (iter.value().contains(m_movingItemTemp))
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
    m_lstSortItems = m_lstItems;
    m_movingItem = m_movingItemTemp;

    // 自动调整完毕后,更新连通域
    updateConnectedState(true);
}

// 更连通状态
// 更新上一次拼接完成的值
bool MonitorsGround::updateConnectedState(bool isInit)
{
    bool isIntersect = false;
    QList<MonitorProxyWidget *> m_lstItemsTemp;
    for (int i = 0; i < m_lstItems.size(); i++) {
        m_lstItemsTemp.clear();

        for (int j = 0; j < m_lstItems.size(); j++) {
            if (j != i
                && m_lstItems[i]
                           ->mapRectToScene(m_lstItems[i]->boundingRectEx())
                           .intersects(m_lstItems[j]->mapRectToScene(m_lstItems[j]->boundingRect()))
                && !m_lstItemsTemp.contains(m_lstItems[j])
                && !m_lstItems[i]
                            ->mapRectToScene(m_lstItems[i]->justIntersectRect())
                            .intersects(
                                    m_lstItems[j]->mapRectToScene(m_lstItems[j]->boundingRect()))) {
                m_lstItemsTemp.append(m_lstItems[j]);
            }
            if (j != i
                && m_lstItems[i]
                           ->mapRectToScene(m_lstItems[i]->justIntersectRect())
                           .intersects(
                                   m_lstItems[j]->mapRectToScene(m_lstItems[j]->boundingRect()))) {
                isIntersect = true;
            }
        }

        if (isInit) {
            m_mapInitItemConnectedState.insert(m_lstItems[i], m_lstItemsTemp);
        }

        m_mapItemConnectedState.insert(m_lstItems[i], m_lstItemsTemp);
    }

    return isIntersect;
}

// 获取连通域
QList<MonitorProxyWidget *> MonitorsGround::getConnectedDomain(MonitorProxyWidget *item)
{
    QList<MonitorProxyWidget *> lstTemp1Items;

    QList<MonitorProxyWidget *> lstTemp;
    QList<MonitorProxyWidget *> lstItems;

    lstItems.append(item);

    for (auto iter : m_lstItems) /*标记循环次数*/ {
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

// 响应键盘调整 脱离的需要拉回来,重叠的需要打回去
// 左右边界相交1个像素的情况下,只能像上下运动; 相反如果上下边界相交1个像素的情况下,只能左右运动.
// 鼠标巡边移动
void MonitorsGround::onRequestKeyPress(MonitorProxyWidget *pw, int keyValue)
{
    Q_EMIT setEffectiveReminderVisible(false, m_nEffectiveTime);
    m_effectiveTimer->stop();

    // 当鼠标移动的时候开始响应并执行自动吸附的逻辑
    // 保证 bufferboundingRect 相交且 boundingRect
    // 不相交，保证移动的块item始终在其他item的外边缘移动

    // 需要在全连通的情况下开始响应键盘事件
    bool bIntersectsTop = false;
    bool bIntersectsBottom = false;
    bool bIntersectsRight = false;
    bool bIntersectsLeft = false;

    for (auto item : m_lstItems) {
        if (item == pw)
            continue;

        if (item->mapRectToScene(item->boundingRect())
                    .intersects(pw->mapRectToScene(pw->justIntersectRectTop()))) {
            // 上相交
            bIntersectsTop = true;
        } else if (item->mapRectToScene(item->boundingRect())
                           .intersects(pw->mapRectToScene(pw->justIntersectRectBottom()))) {
            // 下相交
            bIntersectsBottom = true;
        } else if (item->mapRectToScene(item->boundingRect())
                           .intersects(pw->mapRectToScene(pw->justIntersectRectLeft()))) {
            // 左相交
            bIntersectsLeft = true;
        } else if (item->mapRectToScene(item->boundingRect())
                           .intersects(pw->mapRectToScene(pw->justIntersectRectRight()))) {
            // 右相交
            bIntersectsRight = true;
        }
    }

    if (!bIntersectsTop && !bIntersectsBottom && !bIntersectsRight && !bIntersectsLeft) {
        for (auto item : m_lstItems) {
            if (item == pw)
                continue;
            // 达成顶点相交
            if (item->mapRectToScene(item->boundingRect())
                        .intersects(pw->mapRectToScene(pw->justIntersectRectLeftTop()))) {
                // 左上相交
                bIntersectsBottom = true;
                bIntersectsRight = true;
            } else if (item->mapRectToScene(item->boundingRect())
                               .intersects(pw->mapRectToScene(pw->justIntersectRectLeftBottom()))) {
                // 左下相交
                bIntersectsTop = true;
                bIntersectsRight = true;
            } else if (item->mapRectToScene(item->boundingRect())
                               .intersects(pw->mapRectToScene(pw->justIntersectRectRightTop()))) {
                // 右上相交
                bIntersectsBottom = true;
                bIntersectsLeft = true;
            } else if (item->mapRectToScene(item->boundingRect())
                               .intersects(
                                       pw->mapRectToScene(pw->justIntersectRectRightBottom()))) {
                // 右下相交
                bIntersectsTop = true;
                bIntersectsLeft = true;
            }
        }
    }

    int moveStep = 10;
    qreal recision = 0.1;
    // 根据按键的方向与当前的相接的边和连通域的变化来综合判定是否执行移动操作
    switch (keyValue) {
    case Qt::Key_Left:
        if (!bIntersectsLeft && (bIntersectsTop || bIntersectsBottom)) {
            // 执行运动
            for (int i = 0; i < moveStep * 10; i++) {
                pw->moveBy(-recision, 0);
                pw->update();
                // 相交
                if (updateConnectedState()) {
                    pw->moveBy(1, 0); // 此处为：判定为重叠时，回退距离为内缩的距离
                    break;
                }
                if (getConnectedDomain(m_movingItem).size() != m_lstItems.size()) {
                    pw->moveBy(recision, 0);
                    break;
                }
            }
        }
        break;
    case Qt::Key_Right:
        if (!bIntersectsRight && (bIntersectsTop || bIntersectsBottom)) {
            // 执行运动
            for (int i = 0; i < moveStep * 10; i++) {
                pw->moveBy(recision, 0);
                pw->update();
                if (updateConnectedState()) {
                    pw->moveBy(-1, 0);
                    break;
                }
                if (getConnectedDomain(m_movingItem).size() != m_lstItems.size()) {
                    pw->moveBy(-recision, 0);
                    break;
                }
            }
        }
        break;
    case Qt::Key_Up:
        if (!bIntersectsTop && (bIntersectsLeft || bIntersectsRight)) {
            // 执行运动
            for (int i = 0; i < moveStep * 10; i++) {
                pw->moveBy(0, -recision);
                pw->update();
                if (updateConnectedState()) {
                    pw->moveBy(0, 1);
                    break;
                }
                if (getConnectedDomain(m_movingItem).size() != m_lstItems.size()) {
                    pw->moveBy(0, recision);
                    break;
                }
            }
        }
        break;
    case Qt::Key_Down:
        if (!bIntersectsBottom && (bIntersectsLeft || bIntersectsRight)) {
            // 执行运动
            for (int i = 0; i < moveStep * 10; i++) {
                pw->moveBy(0, recision);
                pw->update();
                if (updateConnectedState()) {
                    pw->moveBy(0, -1);
                    break;
                }
                if (getConnectedDomain(m_movingItem).size() != m_lstItems.size()) {
                    pw->moveBy(0, -recision);
                    break;
                }
            }
        }
        break;
    default:
        break;
    }

    pw->update();
    updateConnectedState();
    onResize();

    Q_EMIT setEffectiveReminderVisible(true, m_nEffectiveTime);
    m_effectiveTimer->start();
}

// 自动吸附实现
void MonitorsGround::onRequestMouseMove(MonitorProxyWidget *pw)
{
    Q_EMIT setEffectiveReminderVisible(false, m_nEffectiveTime);
    m_effectiveTimer->stop();

    // 当鼠标移动的时候开始响应并执行自动吸附的逻辑
    // 保证 bufferboundingRect 相交且 boundingRect
    // 不相交，保证移动的块item始终在其他item的外边缘移动

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

    for (auto item : m_lstItems) {
        if (item == pw)
            continue;

        if (pw->mapToScene(pw->boundingRect()).intersects(item->mapToScene(item->boundingRect())))
            return;

        // 与boundingRect 不相交
        if (pw->mapToScene(pw->boundingRect())
                    .intersects(item->mapToScene(item->bufferboundingRectTop()))) {
            // 上相交
            qreal temp = item->mapToScene(item->boundingRect().topRight()).y()
                    - pw->mapToScene(pw->boundingRect().bottomLeft()).y();
            minMoveLen(temp, top);

            // 判断边对齐
            temp = item->mapToScene(item->boundingRect().topRight()).x()
                    - pw->mapToScene(pw->boundingRect().bottomRight()).x();
            minMoveLen(temp, topLeft);
            temp = item->mapToScene(item->boundingRect().topLeft()).x()
                    - pw->mapToScene(pw->boundingRect().bottomLeft()).x();
            minMoveLen(temp, topRight);
        } else if (pw->mapToScene(pw->boundingRect())
                           .intersects(item->mapToScene(item->bufferboundingRectBottom()))) {
            // 下相交
            qreal temp = item->mapToScene(item->boundingRect().bottomLeft()).y()
                    - pw->mapToScene(pw->boundingRect().topLeft()).y();
            minMoveLen(temp, bottom);

            // 判断边对齐的可能
            temp = item->mapToScene(item->boundingRect().topRight()).x()
                    - pw->mapToScene(pw->boundingRect().bottomRight()).x();
            minMoveLen(temp, bottomLeft);
            temp = item->mapToScene(item->boundingRect().topLeft()).x()
                    - pw->mapToScene(pw->boundingRect().bottomLeft()).x();
            minMoveLen(temp, bottomRight);
        } else if (pw->mapToScene(pw->boundingRect())
                           .intersects(item->mapToScene(item->bufferboundingRectLeft()))) {
            // 左相交
            qreal temp = item->mapToScene(item->boundingRect().bottomLeft()).x()
                    - pw->mapToScene(pw->boundingRect().bottomRight()).x();
            minMoveLen(temp, left);

            // 判断边对齐的可能
            temp = item->mapToScene(item->boundingRect().topRight()).y()
                    - pw->mapToScene(pw->boundingRect().topRight()).y();
            minMoveLen(temp, leftTop);
            temp = item->mapToScene(item->boundingRect().bottomLeft()).y()
                    - pw->mapToScene(pw->boundingRect().bottomLeft()).y();
            minMoveLen(temp, leftBottom);
        } else if (pw->mapToScene(pw->boundingRect())
                           .intersects(item->mapToScene(item->bufferboundingRectRight()))) {
            // 右相交
            qreal temp = item->mapToScene(item->boundingRect().bottomRight()).x()
                    - pw->mapToScene(pw->boundingRect().bottomLeft()).x();
            minMoveLen(temp, right);

            // 判断边对齐的可能
            temp = item->mapToScene(item->boundingRect().topRight()).y()
                    - pw->mapToScene(pw->boundingRect().topRight()).y();
            minMoveLen(temp, rightTop);
            temp = item->mapToScene(item->boundingRect().bottomLeft()).y()
                    - pw->mapToScene(pw->boundingRect().bottomLeft()).y();
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
        edgeAlignmentPos.setX((fabs(top) < fabs(bottom)) ? edgeAlignment(topRight, topLeft)
                                                         : edgeAlignment(bottomRight, bottomLeft));
    }

    if (qFuzzyIsNull(left)) {
        autoAdsorptionPos.setX(right);
        edgeAlignmentPos.setY(edgeAlignment(rightTop, rightBottom));
    } else if (qFuzzyIsNull(right)) {
        autoAdsorptionPos.setX(left);
        edgeAlignmentPos.setY(edgeAlignment(leftTop, leftBottom));
    } else {
        autoAdsorptionPos.setX((fabs(left) < fabs(right)) ? left : right);
        edgeAlignmentPos.setY((fabs(left) < fabs(right)) ? edgeAlignment(leftTop, leftBottom)
                                                         : edgeAlignment(rightTop, rightBottom));
    }

    if (!qFuzzyIsNull(autoAdsorptionPos.x())) {
        edgeAlignmentPos.setX(0.0);
    }

    if (!qFuzzyIsNull(autoAdsorptionPos.y())) {
        edgeAlignmentPos.setY(0.0);
    }

    pw->moveBy(edgeAlignmentPos.x() + autoAdsorptionPos.x(),
               edgeAlignmentPos.y() + autoAdsorptionPos.y());
}

// 更新缩放比例
void MonitorsGround::updateScale()
{
    QRectF rectView = rect();
    QRectF rect = m_graphicsScene.itemsBoundingRect();

    double screenWidth = rect.width();
    double screenHeight = rect.height();

    // 计算缩放比例 1.2，是指活动区域和调节区域
    double hScale = ((rectView.width() > MAX_W) ? MAX_W : rectView.width()) / 1.2 / screenWidth;
    double wScale = rectView.height() / 1.2 / screenHeight;
    m_scale = std::min(hScale, wScale);

    resetTransform();
    scale(m_scale, m_scale);
}

void MonitorsGround::autoRebound()
{

    for (auto pw : m_monitors.keys()) {
        // 设置位置
        pw->setPos(pw->getPreCenter());
        pw->update();
    }
}

// 屏幕示意图平移居中
void MonitorsGround::centeredMonitorsView()
{
    if (m_monitors.size() == 1) {
        singleScreenAdjest();
    } else {
        QPointF dPos = sceneRect().center() - scene()->itemsBoundingRect().center();
        for (auto pw : m_monitors.keys()) {
            // 保存上一次调整的数据,用与在不满足调整条件的时候自动恢复的功能.
            pw->setPreCenter(pw->pos());
            pw->setPos(pw->pos() + dPos);
        }

        if (!m_isInit)
            updateConnectedState(true); // 更新初始化连接数据
    }

    for (auto pw : m_monitors.keys()) {
        pw->update();
    }

    m_isInit = true;
}
