// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "displaymodule.h"

#include "WayQtUtils.hpp"
#include "dccfactory.h"
#include "operation/dccscreen.h"
#include "private/dccscreen_p.h"
#include "private/displaymodel.h"
#include "private/displaymodule_p.h"
#include "private/displayworker.h"

#include <QMetaObject>
#include <QMetaType>
#include <QRect>
#include <QThread>

namespace dccV25 {
class Rect : public QRect
{
public:
    using QRect::QRect;

    bool operator<(const Rect &r) const
    {
        if (x() < r.x()) {
            return true;
        }
        if (x() > r.x()) {
            return false;
        }
        if (y() < r.y()) {
            return true;
        }
        if (y() > r.y()) {
            return false;
        }
        if (width() < r.width()) {
            return true;
        }
        if (width() > r.width()) {
            return false;
        }
        if (height() < r.height()) {
            return true;
        }
        return false;
    }
};

DisplayModulePrivate::DisplayModulePrivate(DisplayModule *parent)
    : q_ptr(parent)
    , m_primary(nullptr)
{
    QMetaObject::invokeMethod(
            q_ptr,
            [this]() {
                init();
            },
            Qt::QueuedConnection);
}

void DisplayModulePrivate::init()
{
    m_model = new DisplayModel(q_ptr);
    m_worker = new DisplayWorker(m_model, q_ptr);
    m_worker->active();
    q_ptr->connect(m_model, &DisplayModel::monitorListChanged, [this]() {
        updateMonitorList();
    });
    q_ptr->connect(m_model, &DisplayModel::primaryScreenChanged, q_ptr, [this]() {
        updatePrimary();
        Q_EMIT q_ptr->primaryScreenChanged();
    });
    q_ptr->connect(m_model, &DisplayModel::displayModeChanged, q_ptr, [this]() {
        updateDisplayMode();
        updateVirtualScreens();
        Q_EMIT q_ptr->displayModeChanged();
    });
    updateMonitorList();
    updatePrimary();
    updateDisplayMode();
}

void DisplayModulePrivate::updateVirtualScreens()
{
    bool changed = false;
    QMap<Rect, QList<Monitor *>> addScreenMap;
    for (auto srcScreen : m_screens) {
        if (!srcScreen->enable()) {
            continue;
        }
        DccScreenPrivate *screenPrivate = DccScreenPrivate::Private(srcScreen);
        Rect rect(QPoint(srcScreen->x(), srcScreen->y()), srcScreen->currentResolution());
        if (addScreenMap.contains(rect)) {
            addScreenMap[rect].append(screenPrivate->monitors());
        } else {
            addScreenMap.insert(rect, screenPrivate->monitors());
        }
    }
    for (auto it = m_virtualScreens.cbegin(); it != m_virtualScreens.cend();) {
        DccScreenPrivate *screenPrivate = DccScreenPrivate::Private(*it);
        Rect rect(QPoint((*it)->x(), (*it)->y()), (*it)->currentResolution());
        bool isSame = addScreenMap.contains(rect);
        if (isSame) {
            auto monitors = addScreenMap[rect];
            auto screenMonitors = screenPrivate->monitors();
            isSame = monitors.size() == screenMonitors.size();
            if (isSame) {
                for (auto monitor : monitors) {
                    if (!screenMonitors.contains(monitor)) {
                        isSame = false;
                        break;
                    }
                }
            }
        }
        if (isSame) {
            addScreenMap.remove(rect);
            it++;
        } else {
            changed = true;
            (*it)->deleteLater();
            it = m_virtualScreens.erase(it);
        }
    }
    for (auto monitors : addScreenMap) {
        changed = true;
        m_virtualScreens << DccScreenPrivate::New(monitors, m_worker, q_ptr);
    }
    if (changed) {
        std::sort(m_virtualScreens.begin(), m_virtualScreens.end(), [](const DccScreen *screen1, const DccScreen *screen2) {
            return screen1->name() < screen2->name();
        });
        updatePrimary();
        Q_EMIT q_ptr->virtualScreensChanged();
    }
}

void DisplayModulePrivate::updateMonitorList()
{
    bool changed = false;
    QList<Monitor *> addMonitorList = m_model->monitorList();
    for (auto it = m_screens.cbegin(); it != m_screens.cend();) {
        Monitor *monitor = DccScreenPrivate::Private(*it)->monitor();
        int index = addMonitorList.indexOf(monitor);
        if (index >= 0) {
            addMonitorList.remove(index);
            it++;
        } else {
            changed = true;
            (*it)->deleteLater();
            it = m_screens.erase(it);
        }
    }
    for (auto monitor : addMonitorList) {
        changed = true;
        m_screens << DccScreenPrivate::New({ monitor }, m_worker, q_ptr);
        auto updateVirtualScreensFun = [this]() {
            updateVirtualScreens();
        };
        q_ptr->connect(monitor, &Monitor::xChanged, q_ptr, updateVirtualScreensFun);
        q_ptr->connect(monitor, &Monitor::yChanged, q_ptr, updateVirtualScreensFun);
        q_ptr->connect(monitor, &Monitor::wChanged, q_ptr, updateVirtualScreensFun);
        q_ptr->connect(monitor, &Monitor::hChanged, q_ptr, updateVirtualScreensFun);
        q_ptr->connect(monitor, &Monitor::enableChanged, q_ptr, updateVirtualScreensFun);
    }
    if (changed) {
        std::sort(m_screens.begin(), m_screens.end(), [](const DccScreen *screen1, const DccScreen *screen2) {
            return screen1->name() < screen2->name();
        });
        updateVirtualScreens();
        Q_EMIT q_ptr->screensChanged();
    }
}

void DisplayModulePrivate::updatePrimary()
{
    DccScreen *primary = nullptr;
    for (auto screen : m_virtualScreens) {
        if (m_model->primary() == screen->name()) {
            primary = screen;
            break;
        }
    }
    if (m_primary != primary) {
        m_primary = primary;
        Q_EMIT q_ptr->primaryScreenChanged();
    }
}

void DisplayModulePrivate::updateDisplayMode()
{
    switch (m_model->displayMode()) {
    case MERGE_MODE:
        m_displayMode = "MERGE";
        break;
    case EXTEND_MODE:
        m_displayMode = "EXTEND";
        break;
    case SINGLE_MODE:
        for (auto screen : m_screens) {
            if (screen->enable()) {
                m_displayMode = screen->name();
                break;
            }
        }
        break;
    default:
        break;
    }
}

DccScreen *DisplayModulePrivate::primary() const
{
    return m_primary;
}

QString DisplayModulePrivate::displayMode() const
{
    return m_displayMode;
}

DisplayModule::DisplayModule(QObject *parent)
    : QObject(parent)
    , d_ptrDisplayModule(new DisplayModulePrivate(this))
{
}

DisplayModule::~DisplayModule() { }

QList<DccScreen *> DisplayModule::virtualScreens() const
{
    Q_D(const DisplayModule);
    return d->m_virtualScreens;
}

QList<DccScreen *> DisplayModule::screens() const
{
    Q_D(const DisplayModule);
    return d->m_screens;
}

DccScreen *DisplayModule::primaryScreen() const
{
    Q_D(const DisplayModule);
    return d->primary();
}

void DisplayModule::setPrimaryScreen(DccScreen *primary)
{
    Q_D(DisplayModule);
    d->m_worker->setPrimary(primary->name());
}

QString DisplayModule::displayMode() const
{
    Q_D(const DisplayModule);
    return d->displayMode();
}

void DisplayModule::setDisplayMode(const QString &mode)
{
    int modeType = SINGLE_MODE;
    QString name;
    if (mode == "MERGE") {
        modeType = MERGE_MODE;
    } else if (mode == "EXTEND") {
        modeType = EXTEND_MODE;
    } else {
        modeType = SINGLE_MODE;
        name = mode;
    }
    Q_D(DisplayModule);
    d->m_worker->switchMode(modeType, name);
}

bool DisplayModule::isX11() const
{
    return !WQt::Utils::isTreeland();
}

void DisplayModule::saveChanges()
{
    Q_D(DisplayModule);
    d->m_worker->saveChanges();
}

void DisplayModule::resetBackup()
{
    Q_D(DisplayModule);
    d->m_worker->resetBackup();
}
DCC_FACTORY_CLASS(DisplayModule)

} // namespace dccV25

#include "displaymodule.moc"
