// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
    AccessPoints *getConnectedAp() const;
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
