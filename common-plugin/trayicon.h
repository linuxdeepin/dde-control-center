/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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

#ifndef TRAYICON_H
#define TRAYICON_H

#include "utils.h"

#include <QWidget>
#include <QVariantMap>

namespace dde {
namespace network {
class AccessPoints;
}
}

NETWORKPLUGIN_BEGIN_NAMESPACE
using namespace dde::network;

class NetworkPluginHelper;

/**
 * @brief The TrayIcon class
 * 登录界面多屏时需要提供多个托盘按钮
 * 固将界面部分与功能实现分开
 */
class TrayIcon : public QWidget
{
    Q_OBJECT

Q_SIGNALS:
    void signalShowNetworkDialog(QWidget *w);

public:
    explicit TrayIcon(NetworkPluginHelper *networkHelper);
    void setGreeterStyle(bool greeterStyle);

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    QString getStrengthStateString(int strength) const;
    AccessPoints *getStrongestAp();
    bool isDarkIcon() const;

public Q_SLOTS:
    void showNetworkDialog();
    void refreshIcon();

protected:
    NetworkPluginHelper *m_networkHelper;
    QPixmap m_iconPixmap;

    bool m_greeterStyle;                  // 登录界面样式
    QTimer *m_refreshIconTimer;
};
NETWORKPLUGIN_END_NAMESPACE
#endif // TRAYICON_H
