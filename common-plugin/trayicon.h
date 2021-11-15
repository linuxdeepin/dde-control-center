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

#include <QWidget>
#include <QVariantMap>

class NetworkPanel;

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
    explicit TrayIcon(NetworkPanel *panel);

public:
    void updateIcon();

protected:
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);

public Q_SLOTS:
    void showNetworkDialog();

protected:
    NetworkPanel *m_panel;
    QPixmap m_iconPixmap;
};

#endif // TRAYICON_H
