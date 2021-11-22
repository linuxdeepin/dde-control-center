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
#include <QWidget>
#include <QWindow>

static const QString NetworkDialogApp = "dde-network-dialog"; //网络列表执行文件

NetworkDialog::NetworkDialog(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
    , m_focusWidget(nullptr)
    , m_runReason(Dock)
    , m_saveMode(false)
{
    connect(m_process, static_cast<void (QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &NetworkDialog::finished);
}

NetworkDialog::~NetworkDialog()
{
    m_process->close();
}

void NetworkDialog::finished(int, QProcess::ExitStatus)
{
    freeFocus();
}

void NetworkDialog::saveConfig(int x, int y, Dock::Position position)
{
    m_saveMode = true;
    runProcess(x, y, position);
}

void NetworkDialog::show(int x, int y, Dock::Position position)
{
    m_process->blockSignals(true);
    m_process->close();
    m_process->blockSignals(false);
    requestFocus();
    runProcess(x, y, position);
}

void NetworkDialog::requestFocus()
{
    for (QWidget *w : qApp->topLevelWidgets()) {
        if (QString("FullscreenBackground") == w->metaObject()->superClass()->className()) {
            w->installEventFilter(this);
            if (w->window() && w->window()->windowHandle()->setKeyboardGrabEnabled(false)) {
                qInfo() << "requestFocus true";
            }
            m_focusWidget = w;
        }
    }
}

void NetworkDialog::freeFocus()
{
    if (m_focusWidget) {
        m_focusWidget->removeEventFilter(this);
        if (m_focusWidget->window() && m_focusWidget->window()->windowHandle()->setKeyboardGrabEnabled(true)) {
            qInfo() << "freeFocus true";
        }
        m_focusWidget = nullptr;
    }
}

bool NetworkDialog::eventFilter(QObject *watched, QEvent *e)
{
    if (e->type() == QEvent::WindowDeactivate) {
        return true;
    }
    return QObject::eventFilter(watched, e);
}

void NetworkDialog::runProcess(int x, int y, Dock::Position position)
{
    QStringList argList;
    if (m_saveMode) {
        argList << "-s";
        m_saveMode = false;
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
    if (!m_connectSsid.isEmpty()) {
        argList << "-c" << m_connectSsid;
        m_connectSsid.clear();
    }
    if (!m_connectDev.isEmpty()) {
        argList << "-n" << m_connectDev;
        m_connectDev.clear();
    }
    switch (m_runReason) {
    case Lock:
        argList << "-r"
                << "Lock";
        break;
    case Greeter:
        argList << "-r"
                << "Greeter";
        break;
    case Dock:
        argList << "-r"
                << "Dock";
        break;
    case Password:
        argList << "-r"
                << "Password";
        break;
    default:
        break;
    }

    m_process->start(NetworkDialogApp, argList);
}

void NetworkDialog::setConnectWireless(const QString &dev, const QString &ssid)
{
    m_connectDev = dev;
    m_connectSsid = ssid;
}

void NetworkDialog::setRunReason(RunReason reason)
{
    m_runReason = reason;
}

void NetworkDialog::setSaveMode(bool isSave)
{
    m_saveMode = isSave;
}
