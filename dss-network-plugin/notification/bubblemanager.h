/*
 * Copyright (C) 2014 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 *             listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#ifndef BUBBLEMANAGER_H
#define BUBBLEMANAGER_H

#include "bubble.h"
#include "constants.h"

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include <QQueue>
#include <QDesktopWidget>
#include <QApplication>
#include <QGuiApplication>
#include <QTimer>

#include <com_deepin_daemon_gesture.h>
#include <com_deepin_dde_daemon_launcherd.h>
#include <com_deepin_daemon_appearance.h>

using Appearance = com::deepin::daemon::Appearance;
using LauncherInter = com::deepin::dde::daemon::Launcher;
using GestureInter = com::deepin::daemon::Gesture;

static const QString NotificationsDBusService = "org.freedesktop.Notifications";
static const QString NotificationsDBusPath = "/org/freedesktop/Notifications";
static const QString DDENotifyDBusServer = "com.deepin.dde.Notification";
static const QString DDENotifyDBusPath = "/com/deepin/dde/Notification";
static const QString Login1DBusService = "org.freedesktop.login1";
static const QString Login1DBusPath = "/org/freedesktop/login1";
static const QString DisplayDaemonDBusServie = "com.deepin.daemon.Display";
static const QString DisplayDaemonDBusPath = "/com/deepin/daemon/Display";
static const QString LauncherDaemonDBusServie = "com.deepin.dde.daemon.Launcher";
static const QString LauncherDaemonDBusPath = "/com/deepin/dde/daemon/Launcher";
static const QString SessionDBusServie = "com.deepin.SessionManager";
static const QString SessionDaemonDBusPath = "/com/deepin/SessionManager";

class AbstractPersistence;
class AbstractNotifySetting;

class BubbleManager : public QObject, public QDBusContext
{
    Q_OBJECT

public:
    explicit BubbleManager(QObject *parent = nullptr);
    ~BubbleManager();

    enum ClosedReason {
        Expired = 1,
        Dismissed = 2,
        Closed = 3,
        Unknown = 4
    };

    enum AnimationPath {
        Down = 0,
        Up = 1
    };

Q_SIGNALS:
    // Standard Notifications dbus implementation
    void ActionInvoked(uint, const QString &);
    void NotificationClosed(uint, uint);

    // Extra DBus APIs
    void RecordAdded(const QString &);
    void AppInfoChanged(const QString &id, uint item, QDBusVariant var);
    void SystemInfoChanged(uint item, QDBusVariant var);
    void AppAddedSignal(const QString &id);
    void AppRemovedSignal(const QString &id);

    // 旧接口之后废弃
    void appAdded(QString appName);
    void appRemoved(QString appName);
    void appSettingChanged(QString Settings);
    void systemSettingChanged(QString Settings);

public Q_SLOTS:
    // Standard Notifications dbus implementation
    /*!
     * \~chinese \name CloseNotification
     * \~chinese \brief 根据通知id关闭通知气泡
     * \~chinese \param 根据Notify函数处理后返回的ID来关闭气泡
     */
    void CloseNotification(uint);
    QStringList GetCapabilities();
    /*!
     * \~chinese \name GetServerInformation
     * \~chinese \brief 获取服务信息
     */
    QString GetServerInformation(QString &, QString &, QString &);
    // new notify will be received by this slot

    /*!
     * \~chinese \name Notify
     * \~chinese \brief 返回一个根据通知内容生成的通知ID
     * \~chinese \param in0:App名称; in1:ID; in2:App图标名称; in3:通知信息概要; in4:通知信息主体
     * \~chinese \param in5:行为信息; in6:提示信息 in7:多长时间超时过期,值为-1时不会超时
     * \~chinese \return 返回一个处理后的replacesId
     */
    uint Notify(const QString &, uint replacesId, const QString &, const QString &, const QString &);

    /*!
     * \~chinese \name RemoveRecord
     * \~chinese \brief 根据ID删除通知记录
     * \~chinese \param id
     */
    void RemoveRecord(const QString &id);
    /*!
     * \~chinese \name ClearRecords
     * \~chinese \brief 删除所有通知记录
     */
    void ClearRecords();
    /*!
     * \~chinese \name Toggle
     * \~chinese \brief 控制通知中心的显示和隐藏
     */
    void Toggle();
    /*!
     * \~chinese \name recordCount
     * \~chinese \brief 返回通知中心中通知的数量
     * \~chinese \return 通知中心中通知的数量
     */
    void Show();
    void Hide();

private Q_SLOTS:
    /*!
     * \~chinese \name geometryChanged
     * \~chinese \brief 当屏幕或DOCK栏发生几何变化时,该函数被执行,更新通知中心的几何位置和大小
     */
    void geometryChanged();
    void onPrepareForSleep(bool);
    /*!
     * \~chinese \name bubbleExpired
     * \~chinese \brief 当气泡超时时发出信号,执行该函数,产生移除动画,动画结束删除通知气泡
     * \~chinese \param 超时的通知气泡
     */
    void bubbleExpired(Bubble *);
    /*!
     * \~chinese \name bubbleDismissed
     * \~chinese \brief 鼠标点击气泡,或点击关闭按钮,执行该函数,产生移除动画,动画结束删除通知气泡
     * \~chinese \param 需要关闭的通知
     */
    void bubbleDismissed(Bubble *);
    void bubbleReplacedByOther(Bubble *);
    /*!
     * \~chinese \name bubbleActionInvoked
     * \~chinese \brief 鼠标点击气泡,点击该通知后执行的动作
     * \~chinese \param in1:当前气泡 in:气泡执行动作的id
     */
    void bubbleActionInvoked(Bubble *, QString);
    /*!
     * \~chinese \name updateGeometry
     * \~chinese \brief 当主屏幕发生改变或几何大小发送改变,更新所有通知气泡的几何位置
     */
    void updateGeometry();

private:
    void initConnections();                 //初始化信号槽连接
    /*!
     * \~chinese \name calcReplaceId
     * \~chinese \brief 计算ReplaceId,并查找当前通知中是否有相同的ReplaceId,如果有相同的
     * \~chinese ReplaceId,将该ReplaceId标记的通知中的内容替换掉
     * \~chinese \return 有相同的ReplaceId返回true,没有返回false
     */
    bool calcReplaceId(EntityPtr notify);

    bool checkControlCenterExistence();

    Bubble *createBubble(EntityPtr notify, int index = 0);  //创建一个通知气泡
    void pushBubble(EntityPtr notify);                      //推入一个气泡
    void popBubble(Bubble *);                               //推出一个气泡
    void refreshBubble();

    void pushAnimation(Bubble *bubble);                     //推入一个气泡的动画
    void popAnimation(Bubble *bubble);                      //推出一个气泡的动画

    QRect getBubbleGeometry(int index);                     //根据索引获取气泡的矩形大小
    // Get the last unanimated bubble rect
    QRect getLastStableRect(int index);                     //得到最后一个没有动画的矩形气泡
    QRect calcDisplayRect();
    /**
     * @brief getBubbleHeightBefore 获取序号小于index的气泡的高度之和
     * @param index 当前的气泡序号
     * @return 气泡高度之和
     */
    int getBubbleHeightBefore(const int index);
    QWidget *parentWidget();

private:
    int m_replaceCount = 0;
    QString m_configFile;
    QRect m_currentDisplayRect;
    QRect m_currentDockRect;
    OSD::DockPosition m_dockPos;
    int m_dockMode;

    QList<EntityPtr> m_oldEntities;
    QList<QPointer<Bubble>> m_bubbleList;

    // 手指划入距离，任务栏在右侧时，需大于任务栏最大宽度100，其它情况没有设限大于0即可
    int m_slideWidth;
    GestureInter *m_gestureInter;
    QTimer* m_trickTimer; // 防止300ms内重复按键
};

#endif // BUBBLEMANAGER_H
