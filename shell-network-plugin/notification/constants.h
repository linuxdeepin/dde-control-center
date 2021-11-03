/*
 * Copyright (C) 2015 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     fanpengcheng <fanpengcheng_cm@deepin.com>
 *
 * Maintainer: fanpengcheng <fanpengcheng_cm@deepin.com>
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
#ifndef DEFINE_H
#define DEFINE_H

#include "notificationentity.h"

#include <QSize>
#include <QStringList>
#include <QStandardPaths>
#include <QTextDocument>
#include <QSettings>
#include <QDebug>
#include <QApplication>

#define MIN(a,b) ((a)>(b)?(b):(a))
#define ABS(a) (a)>0?(a):(-(a))

#define  DEFAULT_TIME_SLOT true
#define  DEFAULT_DO_NOT_DISTURB false
#define  DEFAULT_APP_IN_FULLSCREEN false
#define  DEFAULT_CONNECTED_PROJECTOR false
#define  DEFAULT_SCREEN_LOCKED false
#define  DEFAULT_SHOW_ICON_ON_DOCK true

#define  DEFAULT_ALLOW_NOTIFY true
#define  DEFAULT_ONLY_IN_NOTIFY true
#define  DEFAULT_LOCK_SHOW_NOTIFY true
#define  DEFAULT_SHOW_NOTIFY_PREVIEW true
#define  DEFAULT_NOTIFY_SOUND true

typedef std::shared_ptr<NotificationEntity> EntityPtr;

static const int MenuPadding = 4;
static const int MenuWidth = 14;
static const int MenuHeight = 7;
static const int ScreenPadding = 20;            //最上方通知距屏幕上方间隔
static const int BubbleMargin = 12;             //桌面消息通知间隔
static const int BubblePadding = 10;            //消息通知内部Padding
static const int BubbleSpacing = 10;            //消息通知内部Space
static const int BubbleTimeout = 5000;          //通知默认超时时间(毫秒)
static const int BubbleEntities = 3;
static const int BubbleOverLap = 2;             //层叠的气泡数量
static const int BubbleOverLapHeight = 12;      //通知中心层叠层高度
static const QString NoReplaceId = "0";         //为0 返回一个计数值给程序
static const int AnimationTime = 300;           //动画时间，单位：毫秒
static const int ExpandAnimationTime = 100;
static const int BubbleWindowHeight = 60;       // 窗口模式下气泡的高度
static const int MaxBubbleButtonWidth = 180;    // 窗口模式下气泡按钮的最大宽度
static const int BubbleStartPos = -(BubbleWindowHeight + ScreenPadding);  // 窗口模式下气泡起始Y位置
static const QStringList Directory = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
static const QString CachePath = Directory.first() + "/.cache/deepin/deepin-notifications/";

static const QString DeletedAction = "deleted";
static const QString CreatedAction = "created";

static const QString AllowNotifyStr = "AllowNotify";
static const QString ShowInNotifyCenterStr = "ShowInNotifyCenter";
static const QString LockShowNotifyStr = "LockShowNotify";
static const QString ShowNotifyPreviewStr = "ShowNotifyPreview";
static const QString NotificationSoundStr = "NotificationSound";
static const QString AppIconStr = "Icon";
static const QString AppNameStr = "Name";

static const QString SystemNotifySettingStr = "SystemNotify";
static const QString DoNotDisturbStr = "DoNotDisturb";
static const QString TimeSlotStr = "TimeSlot";
static const QString StartTimeStr = "StartTime";
static const QString DefaultStartTime = "22:00";
static const QString EndTimeStr = "EndTime";
static const QString DefaultEndTime = "07:00";
static const QString AppsInFullscreenStr = "AppsInFullscreen";
static const QString ConnectedProjectorStr = "ConnectedProjector";
static const QString ScreenLockedStr = "ScreenLocked";
static const QString ShowIconOnDockStr = "ShowIconOnDock";
static const QString WhiteBoardStr = "WhiteBoard";
static const QStringList IgnoreList = {
    "dde-control-center"
};

namespace Notify {
    static const int CenterWidth = 400;
    static const int CenterMargin = 10;
    static const int CenterTitleHeight = 36;

    static const int GroupTitleHeight = 32;
    static const int BubbleCloseButtonSize = 24;
    static const int GroupButtonSize = 28;
    static const int GroupMargin = 30;
    static const int BubbleDefaultAlpha = 20;       //bubble控件透明度
    static const qreal BubbleOpacity = 0.6;         //bubble内容的透明度
}

class OSD
{
public:
    static const int DockMargin = 5;

    typedef enum {
        BUBBLEWINDOW,
        BUBBLEWIDGET
    } ShowStyle;

    enum DockPosition {
        Top = 0,
        Right = 1,
        Bottom = 2,
        Left = 3
    };

    enum DockModel {
        Fashion = 0,
        Efficient = 1
    };

    static QSize BubbleSize(ShowStyle style)
    {
        QSize size;
        if (style == BUBBLEWINDOW) {
            size = QSize(600, BubbleWindowHeight);
        } else if (style == BUBBLEWIDGET) {
            size = QSize(380, 90);
        }
        return size;
    }

    static int BubbleWidth(ShowStyle style)
    {
        return BubbleSize(style).width();
    }

    static int BubbleHeight(ShowStyle style)
    {
        return BubbleSize(style).height();
    }

    static QSize ButtonSize(ShowStyle style)
    {
        QSize size;
        if (style == BUBBLEWINDOW) {
            size = QSize(70, 40);
        } else if (style == BUBBLEWIDGET) {
            size = QSize(60, 36);
        }
        return size;
    }

    static QSize IconButtonSize(ShowStyle style)
    {
        QSize size;
        if (style == BUBBLEWINDOW) {
            size = QSize(36, 36);
        } else if (style == BUBBLEWIDGET) {
            size = QSize(36, 36);
        }
        return size;
    }

    static QSize IconSize(ShowStyle style)
    {
        QSize size;
        if (style == BUBBLEWINDOW) {
            size = QSize(40, 40);
        } else if (style == BUBBLEWIDGET) {
            size = QSize(24, 24);
        }
        return size;
    }

    static QSize CloseButtonSize(ShowStyle style)
    {
        QSize size;
        if (style == BUBBLEWINDOW) {
            size = QSize(30, 30);
        } else if (style == BUBBLEWIDGET) {
            size = QSize(24, 24);
        }
        return size;
    }

    static QString removeHTML(const QString &source)
    {
        QTextDocument document;
        document.setHtml(source);
        return document.toPlainText();
    }
};
#endif // DEFINE_H
