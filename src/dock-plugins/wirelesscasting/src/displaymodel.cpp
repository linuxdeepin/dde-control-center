// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "displaymodel.h"

const QString DisplayDBusServer = "org.deepin.dde.Display1";
const QString DisplayDBusPath = "/org/deepin/dde/Display1";

DisplayModel::DisplayModel(QObject *parent)
    : QObject(parent)
    , m_displayInter(new DisplayInter(DisplayDBusServer,
                                 DisplayDBusPath,
                                 QDBusConnection::sessionBus(), this))
{
    m_displayInter->setSync(false, false);
    connect(m_displayInter, &DisplayInter::DisplayModeChanged, this, &DisplayModel::setDisplayMode);
    connect(m_displayInter, &DisplayInter::MonitorsChanged, this, &DisplayModel::setMonitors);
    connect(m_displayInter, &DisplayInter::PrimaryChanged, this, &DisplayModel::setPrimaryScreen);

    setPrimaryScreen(m_displayInter->primary());
    setDisplayMode(m_displayInter->displayMode());
    setMonitors(m_displayInter->monitors());
}

DisplayModel::~DisplayModel()
{
}

void DisplayModel::setPrimaryScreen(const QString &screen)
{
    if (m_primaryScreen != screen) {
        m_primaryScreen = screen;
        Q_EMIT primaryScreenChanged(m_primaryScreen);
    }
}

void DisplayModel::setDisplayMode(const uchar &mode)
{
    if (m_displayMode != mode) {
        m_displayMode = mode;
        Q_EMIT displayModeChanged(m_displayMode);
    }
}

void DisplayModel::setMonitors(const QList<QDBusObjectPath> &monitors)
{
    Q_UNUSED(monitors)
    QDBusPendingCall call = m_displayInter->ListOutputNames();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        if (watcher->isError()) {
            qWarning() << "Failed to get all output names: " << watcher->error().message();
        } else {
            QDBusReply<QStringList> reply = watcher->reply();
            m_displayMode = m_displayInter->displayMode();
            const QStringList &monitorsTemp = reply.value();
            for (auto name : monitorsTemp) {
                if (!m_monitors.contains(name)) {
                    m_monitors << name;
                    Q_EMIT screenAdded(name);
                }
            }
            for (auto name : m_monitors) {
                if (!monitorsTemp.contains(name)) {
                    m_monitors.removeOne(name);
                    Q_EMIT screenRemoved(name);
                }
            }
            // 按照名称排序，显示的时候VGA在前，HDMI在后
            std::sort(m_monitors.begin(), m_monitors.end(), std::greater<QString>());
            emit monitorsChanged(m_monitors);
        }
    });
}

void DisplayModel::switchMode(uchar displayMode, QString screen)
{
    m_displayInter->SwitchMode(displayMode, screen);
    setDisplayMode(displayMode);
}

void DisplayModel::setCurrentMode(const QString &currentMode)
{
    if (m_currentMode != currentMode) {
        m_currentMode = currentMode;
        Q_EMIT currentModeChanged(currentMode);
    }
}