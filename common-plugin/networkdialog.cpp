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

#include "networkdialog.h"

#include <QApplication>
#include <QDebug>
#include <QWindow>

static const QString NetworkDialogApp = "dde-network-dialog"; //网络列表执行文件

NetworkDialog::NetworkDialog(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_focusWindow(nullptr)
{
    connect(m_process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &NetworkDialog::finished);
}

NetworkDialog::~NetworkDialog()
{
    m_process->close();
}

void NetworkDialog::finished(int, QProcess::ExitStatus)
{
    if (m_focusWindow) {
        m_focusWindow->setKeyboardGrabEnabled(true);
    }
}

void NetworkDialog::saveConfig(int x, int y, Dock::Position position)
{
    runProcess(x, y, position, true, false);
}

void NetworkDialog::show(int x, int y, Dock::Position position, bool isShell)
{
    m_process->blockSignals(true);
    m_process->close();
    m_process->blockSignals(false);
    QWindow *window = qApp->focusWindow();
    runProcess(x, y, position, false, isShell);
    if(window) {
        m_focusWindow = window;
    }
    if (m_focusWindow) {
        m_focusWindow->setKeyboardGrabEnabled(false);
    }
}

void NetworkDialog::runProcess(int x, int y, Dock::Position position, bool isSave, bool isShell)
{
    QStringList argList;
    if (isSave) {
        argList << "-s";
    }
    argList << "-p" << QString("%1x%2").arg(x).arg(y);
    QString pos = "bottom";
    switch (position) {
    case Dock::Position::Top:
        pos = "top";
        break;
    case Dock::Position::Bottom:
        pos = "bottom";
        break;
    case Dock::Position::Left:
        pos = "left";
        break;
    case Dock::Position::Right:
        pos = "right";
        break;
    }
    argList << "-d" << pos;
    if (!m_connectPath.isEmpty()) {
        argList << "-c" << m_connectPath;
        m_connectPath.clear();
    }
    if (isShell) {
        argList << "-l";
    }
    m_process->start(NetworkDialogApp, argList);
}

void NetworkDialog::setConnectWireless(const QString &path)
{
    m_connectPath = path;
}
