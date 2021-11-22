/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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

#ifndef NETWORKDIALOG_H
#define NETWORKDIALOG_H

#include "constants.h"

#include <QProcess>

class QWindow;

class NetworkDialog : public QObject
{
    Q_OBJECT

public:
    enum RunReason {
        Lock,      // 锁屏插件唤起
        Greeter,   // greeter插件唤起
        Dock,      // 任务栏插件唤起
        Password,  // 密码错误唤起
    };

    explicit NetworkDialog(QObject *parent = Q_NULLPTR);
    ~NetworkDialog();

    void saveConfig(int x, int y, Dock::Position position = Dock::Position::Bottom);
    void show(int x, int y, Dock::Position position = Dock::Position::Bottom);
    void setConnectWireless(const QString &dev, const QString &ssid);
    void setRunReason(RunReason reason);
    void setSaveMode(bool isSave);

private:
    void runProcess(int x, int y, Dock::Position position = Dock::Position::Bottom);
    bool eventFilter(QObject *watched, QEvent *e) override;

private Q_SLOTS:
    void finished(int, QProcess::ExitStatus);
    void requestFocus();
    void freeFocus();

private:
    QProcess *m_process;
    QWidget *m_focusWidget;
    QString m_connectDev;
    QString m_connectSsid;
    RunReason m_runReason;
    bool m_saveMode;
};

#endif // NETWORKDIALOG_H
