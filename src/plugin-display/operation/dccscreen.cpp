// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccscreen.h"

#include "private/dccscreen_p.h"
#include "private/displayworker.h"

#include <QGuiApplication>

namespace dccV25 {
DccScreen *DccScreenPrivate::New(QList<Monitor *> monitors, DisplayWorker *worker, QObject *parent)
{
    DccScreen *screen = new DccScreen(parent);
    screen->d_ptrDccScreen->m_worker = worker;
    screen->d_ptrDccScreen->setMonitors(monitors);
    return screen;
}

DccScreenPrivate *DccScreenPrivate::Private(DccScreen *screen)
{
    return screen->d_ptrDccScreen.get();
}

DccScreenPrivate::DccScreenPrivate(DccScreen *screen)
    : q_ptr(screen)
    , m_screen(nullptr)
    , m_maxScale(1.0)
{
}

DccScreenPrivate::~DccScreenPrivate() { }

void DccScreenPrivate::setMonitors(QList<Monitor *> monitors)
{
    m_monitors = monitors;
    std::sort(m_monitors.begin(), m_monitors.end(), [](const Monitor *monitor1, const Monitor *monitor2) {
        return monitor1->name() < monitor2->name();
    });
    QStringList name;
    auto updateMaxScaleFun = [this]() {
        updateMaxScale();
    };
    for (auto monitor : m_monitors) {
        name << monitor->name();
        q_ptr->connect(monitor, &Monitor::currentModeChanged, q_ptr, updateMaxScaleFun);
        q_ptr->connect(monitor, &Monitor::enableChanged, q_ptr, updateMaxScaleFun);
    }
    m_name = name.join(" = ");
    updateResolutionList();
    updateRateList();
    updateScreen();
    updateMaxScale();
    q_ptr->connect(monitor(), &Monitor::modelListChanged, q_ptr, [this]() {
        updateResolutionList();
        updateRateList();
    });
    q_ptr->connect(monitor(), &Monitor::currentModeChanged, q_ptr, [this]() {
        updateResolutionList();
        updateRateList();
        q_ptr->currentResolutionChanged();
        q_ptr->currentRateChanged();
    });
    q_ptr->connect(monitor(), &Monitor::availableFillModesChanged, q_ptr, &DccScreen::availableFillModesChanged);
    q_ptr->connect(monitor(), &Monitor::currentFillModeChanged, q_ptr, &DccScreen::currentFillModeChanged);
    q_ptr->connect(monitor(), &Monitor::enableChanged, q_ptr, &DccScreen::enableChanged);
    auto updateScreenFun = [this]() {
        updateScreen();
    };
    q_ptr->connect(qApp, &QGuiApplication::screenAdded, q_ptr, updateScreenFun);
    q_ptr->connect(qApp, &QGuiApplication::screenRemoved, q_ptr, updateScreenFun);
}

Monitor *DccScreenPrivate::monitor()
{
    return m_monitors.first();
}

QList<Monitor *> DccScreenPrivate::monitors()
{
    return m_monitors;
}

void DccScreenPrivate::setMode(QSize resolution, double rate)
{
    m_worker->backupConfig();
    for (auto monitor : m_monitors) {
        quint32 id = 0;
        for (auto mode : monitor->modeList()) {
            if (mode.width() == resolution.width() && mode.height() == resolution.height()) {
                if (mode.rate() == rate) {
                    id = mode.id();
                    break;
                } else if (id == 0) {
                    id = mode.id();
                }
            }
        }
        if (id != 0) {
            m_worker->setMonitorResolution(monitor, id);
        } else {
            m_worker->setMonitorResolutionBySize(monitor, resolution.width(), resolution.height());
        }
    }
    m_worker->applyChanges();
}

void DccScreenPrivate::setRotate(uint rotate)
{
    m_worker->backupConfig();
    m_worker->setMonitorRotate(monitor(), rotate);
    m_worker->applyChanges();
}

void DccScreenPrivate::setFillMode(const QString &fileMode)
{
    m_worker->backupConfig();
    for (auto monitor : m_monitors) {
        m_worker->setCurrentFillMode(monitor, fileMode);
    }
}

void DccScreenPrivate::setScale(qreal scale)
{
    for (auto monitor : m_monitors) {
        m_worker->setIndividualScaling(monitor, scale);
    }
}

void DccScreenPrivate::updateResolutionList()
{
    QList<QSize> resolutionList;
    for (auto monitor = m_monitors.cbegin(); monitor != m_monitors.cend(); monitor++) {
        QList<QSize> tmpResolutionList;
        for (auto mode : (*monitor)->modeList()) {
            QSize tmpMode(mode.width(), mode.height());
            if (!tmpResolutionList.contains(tmpMode)) {
                tmpResolutionList.append(tmpMode);
            }
        }
        if (monitor == m_monitors.cbegin()) {
            resolutionList = tmpResolutionList;
        } else {
            for (auto it = resolutionList.begin(); it != resolutionList.end();) {
                if (tmpResolutionList.contains(*it)) {
                    it++;
                } else {
                    it = resolutionList.erase(it);
                }
            }
        }
    }
    m_resolutionList = resolutionList;
    if (m_resolutionList != resolutionList) {
        m_resolutionList = resolutionList;
        Q_EMIT q_ptr->resolutionListChanged();
    }
}

void DccScreenPrivate::updateRateList()
{
    QList<double> rateList;
    Resolution currentMode = monitor()->currentMode();
    for (auto mode : monitor()->modeList()) {
        if (mode.width() == currentMode.width() && mode.height() == currentMode.height()) {
            rateList.append(mode.rate());
        }
    }
    if (m_rateList != rateList) {
        m_rateList = rateList;
        Q_EMIT q_ptr->rateListChanged();
    }
}

void DccScreenPrivate::updateScreen()
{
    QString name = monitor()->name();
    QRect rect = monitor()->rect();
    QScreen *tmpScreen = nullptr;
    for (auto screen : QGuiApplication::screens()) {
        if (rect == screen->geometry()) {
            if (name == screen->name()) {
                tmpScreen = screen;
            }
            if (!tmpScreen) {
                tmpScreen = screen;
            }
        }
    }
    if (tmpScreen != m_screen) {
        m_screen = tmpScreen;
        Q_EMIT q_ptr->screenChanged();
    }
}

void DccScreenPrivate::updateMaxScale()
{
    qreal maxScale = 3.0;
    for (auto monitor : m_monitors) {
        if (!monitor->enable()) {
            continue;
        }
        auto tmode = monitor->currentMode();
        if (tmode.width() == 0 || tmode.height() == 0) {
            maxScale = 1.0;
            break;
        }
        qreal maxWScale = tmode.width() / MinScreenWidth;
        qreal maxHScale = tmode.height() / MinScreenHeight;
        maxScale = std::min(maxScale, std::min(maxWScale, maxHScale));
    }
    if (maxScale < 1.0) {
        maxScale = 1.0;
    }
    if (m_maxScale != maxScale) {
        m_maxScale = maxScale;
        Q_EMIT q_ptr->maxScaleChanged();
    }
}

DccScreen::DccScreen(QObject *parent)
    : QObject(parent)
    , d_ptrDccScreen(new DccScreenPrivate(this))
{
}

DccScreen::~DccScreen() { }

QString DccScreen::name() const
{
    return d_ptrDccScreen->m_name;
}

bool DccScreen::enable() const
{
    return d_ptrDccScreen->monitor()->enable();
}

int DccScreen::x() const
{
    return d_ptrDccScreen->monitor()->x();
}

int DccScreen::y() const
{
    return d_ptrDccScreen->monitor()->y();
}

QSize DccScreen::bestResolution() const
{
    Resolution resolution = d_ptrDccScreen->monitor()->bestMode();
    return QSize(resolution.width(), resolution.height());
}

QSize DccScreen::currentResolution() const
{
    auto mode = d_ptrDccScreen->monitor()->currentMode();
    return QSize(mode.width(), mode.height());
}

void DccScreen::setCurrentResolution(const QSize &resolution)
{
    d_ptrDccScreen->setMode(resolution, currentRate());
}

double DccScreen::bestRate() const
{
    return d_ptrDccScreen->monitor()->bestMode().rate();
}

double DccScreen::currentRate() const
{
    return d_ptrDccScreen->monitor()->currentMode().rate();
}

void DccScreen::setCurrentRate(const double &rate)
{
    d_ptrDccScreen->setMode(currentResolution(), rate);
}

QList<QSize> DccScreen::resolutionList() const
{
    return d_ptrDccScreen->m_resolutionList;
}

QList<double> DccScreen::rateList() const
{
    return d_ptrDccScreen->m_rateList;
}

uint DccScreen::rotate() const
{
    return d_ptrDccScreen->monitor()->rotate();
}

void DccScreen::setRotate(uint rotate)
{
    d_ptrDccScreen->setRotate(rotate);
}

double DccScreen::brightness() const
{
    return d_ptrDccScreen->monitor()->brightness();
}

QString DccScreen::currentFillMode() const
{
    QString fillMode = d_ptrDccScreen->monitor()->currentFillMode();
    return fillMode.isEmpty() ? "None" : fillMode;
}

void DccScreen::setCurrentFillMode(const QString &fill)
{
    d_ptrDccScreen->setFillMode(fill);
}

QStringList DccScreen::availableFillModes() const
{
    return d_ptrDccScreen->monitor()->availableFillModes();
}

QScreen *DccScreen::screen() const
{
    return d_ptrDccScreen->m_screen;
}

qreal DccScreen::scale() const
{
    return d_ptrDccScreen->monitor()->scale();
}

void DccScreen::setScale(qreal scale)
{
    d_ptrDccScreen->setScale(scale);
}

qreal DccScreen::maxScale() const
{
    return d_ptrDccScreen->m_maxScale;
}

} // namespace dccV25
