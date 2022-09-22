// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BUBBLETOOL_H
#define BUBBLETOOL_H

#include <QRgb>
#include <QImage>

#include "constants.h"

class ActionButton;
class AppIcon;
class NotificationEntity;
class QDBusArgument;
/*!
 * \~chinese \class BubbleTool
 * \~chinese \brief 气泡的工具类
 */
class BubbleTool
{
public:
    static QString processActions(ActionButton *action, QStringList action_list); //设置功能列表
    static void processIconData(AppIcon *icon, EntityPtr entity); //从entity提取出图标信息设置到icon上
    static void actionInvoke(const QString &actionId, EntityPtr entity);//从entity提取出命令信息,执行命令产生相应动作
    static const QString getDeepinAppName(const QString &name);//获取应用名称

private:
    /*!
     * \~chinese \name saveImg
     * \~chinese \brief 在路径/.cache/deepin/deepin-notifications/保存一张图片,命名方式为路径名加通知id
     * \~chinese \param image:要保存的图片 id:通知id
     */
    static void saveImg(const QImage &image, uint id);
    /*!
     * \~chinese \name converToPixmap
     * \~chinese \brief 根据value参数得到一张图片并保存,将图片转换为一张保持长宽比,并保持清晰度的位图返回
     * \~chinese \param icon:根据此参数来获取图片的长和宽 value:根据此参数来获取图片 id:通知id
     * ~chinese \return 返回一张位图
     */
    static QPixmap converToPixmap(AppIcon *icon, const QDBusArgument &value, uint id);
    static inline void copyLineRGB32(QRgb *dst, const char *src, int width);
    static inline void copyLineARGB32(QRgb *dst, const char *src, int width);
    /*!
     * \~chinese \name decodeNotificationSpecImageHint
     * \~chinese \brief 根据Dbus返回的参数来得到一张图片
     * \~chinese \param 返回一张图片
     */
    static QImage decodeNotificationSpecImageHint(const QDBusArgument &arg);
};

#endif // BUBBLETOOL_H
