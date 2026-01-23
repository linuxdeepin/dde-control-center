// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "displaymodule.h"

#include "WayQtUtils.h"
#include "dccfactory.h"
#include "dccscreen.h"
#include "private/concatscreen.h"
#include "private/dccscreen_p.h"
#include "private/displaymodel.h"
#include "private/displaymodule_p.h"
#include "private/displayworker.h"

#include <QDateTime>
#include <QJSValue>
#include <QMetaObject>
#include <QMetaType>
#include <QQuickItem>
#include <QRect>
#include <QRegularExpression>
#include <QThread>

namespace dccV25 {
static const QString DEFAULT_TIME_FORMAT = "hh:mm";

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
    , m_maxGlobalScale(1.0)
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
    });
    q_ptr->connect(m_model, &DisplayModel::displayModeChanged, q_ptr, [this]() {
        updateVirtualScreens();
        updateDisplayMode();
    });
    q_ptr->connect(m_model, &DisplayModel::colorTemperatureEnabledChanged, q_ptr, &DisplayModule::colorTemperatureEnabledChanged);
    q_ptr->connect(m_model, &DisplayModel::colorTemperatureChanged, q_ptr, &DisplayModule::colorTemperatureChanged);
    q_ptr->connect(m_model, &DisplayModel::customColorTempTimePeriodChanged, q_ptr, &DisplayModule::customColorTempTimePeriodChanged);
    q_ptr->connect(m_model, &DisplayModel::adjustCCTmodeChanged, q_ptr, &DisplayModule::colorTemperatureModeChanged);
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
        DccScreen *screen = DccScreenPrivate::New(monitors, m_worker, q_ptr);
        q_ptr->connect(screen, &DccScreen::rotateChanged, q_ptr, &DisplayModule::applyChanged, Qt::QueuedConnection);
        q_ptr->connect(screen, &DccScreen::currentModeChanged, q_ptr, &DisplayModule::applyChanged, Qt::QueuedConnection);
        q_ptr->connect(screen, &DccScreen::wallpaperChanged, q_ptr, &DisplayModule::wallpaperChanged);
        m_virtualScreens << screen;
    }
    if (changed) {
        // 屏幕按类型排序，同类型按名称排序,类型顺序：eDP-DP-DisplayPort-HDMI-DVI-VGA
        static const QStringList c_screenSort({ "vga", "dvi", "hdmi", "displayport", "dp", "edp" });
        std::sort(m_virtualScreens.begin(), m_virtualScreens.end(), [](const DccScreen *screen1, const DccScreen *screen2) {
            int index1 = c_screenSort.indexOf(screen1->name().split('-').first().toLower());
            int index2 = c_screenSort.indexOf(screen2->name().split('-').first().toLower());
            if (index1 != index2) {
                return index1 > index2;
            }
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
    auto updateVirtualScreensFun = [this]() {
        updateVirtualScreens();
    };
    auto updateMaxGlobalScaleFun = [this]() {
        updateMaxGlobalScale();
    };
    for (auto monitor : addMonitorList) {
        changed = true;
        m_screens << DccScreenPrivate::New({ monitor }, m_worker, q_ptr);
        q_ptr->connect(monitor, &Monitor::xChanged, q_ptr, updateVirtualScreensFun);
        q_ptr->connect(monitor, &Monitor::yChanged, q_ptr, updateVirtualScreensFun);
        q_ptr->connect(monitor, &Monitor::wChanged, q_ptr, updateVirtualScreensFun);
        q_ptr->connect(monitor, &Monitor::hChanged, q_ptr, updateVirtualScreensFun);
        q_ptr->connect(monitor, &Monitor::enableChanged, q_ptr, updateVirtualScreensFun);
        q_ptr->connect(monitor, &Monitor::enableChanged, q_ptr, [this]() {
            updateDisplayMode();
        });
        q_ptr->connect(monitor, &Monitor::currentModeChanged, q_ptr, updateMaxGlobalScaleFun);
        q_ptr->connect(monitor, &Monitor::enableChanged, q_ptr, updateMaxGlobalScaleFun);
    }
    if (changed) {
        std::sort(m_screens.begin(), m_screens.end(), [](const DccScreen *screen1, const DccScreen *screen2) {
            return screen1->name() < screen2->name();
        });
        updateVirtualScreens();
        updateMaxGlobalScale();
        updateDisplayMode();
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
    QString displayMode = m_displayMode;
    switch (m_model->displayMode()) {
    case MERGE_MODE:
        displayMode = "MERGE";
        break;
    case EXTEND_MODE:
        displayMode = "EXTEND";
        break;
    case SINGLE_MODE:
        for (auto screen : m_screens) {
            if (screen->enable()) {
                displayMode = screen->name();
                break;
            }
        }
        break;
    default:
        break;
    }
    if (displayMode != m_displayMode) {
        m_displayMode = displayMode;
        Q_EMIT q_ptr->displayModeChanged();
    }
}

void DisplayModulePrivate::updateMaxGlobalScale()
{
    qreal maxScale = 3.0;
    for (auto monitor : m_model->monitorList()) {
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
    if (m_maxGlobalScale != maxScale) {
        m_maxGlobalScale = maxScale;
        Q_EMIT q_ptr->maxGlobalScaleChanged();
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

void DisplayModulePrivate::setScreenPosition(QList<ScreenData *> screensData)
{
    qRegisterMetaType<QHash<Monitor *, QPair<int, int>>>("QHash<Monitor *, QPair<int, int>>");
    QHash<Monitor *, QPair<int, int>> monitorPosition;
    int lstX = 1000000, lstY = 1000000;
    for (auto item : screensData) {
        if (lstX > qRound(item->rect().x())) {
            lstX = qRound(item->rect().x());
        }
        if (lstY > qRound(item->rect().y())) {
            lstY = qRound(item->rect().y());
        }
        DccScreenPrivate *screenPrivate = DccScreenPrivate::Private(item->screen());
        for (auto monitor : screenPrivate->monitors()) {
            monitorPosition.insert(monitor, QPair<int, int>(qRound(item->rect().x()), qRound(item->rect().y())));
        }
    }

    for (auto &&pos : monitorPosition) {
        pos = QPair<int, int>(pos.first - lstX, pos.second - lstY);
    }
    for (auto it = monitorPosition.cbegin(); it != monitorPosition.cend(); ++it) {
        qDebug() << "applySettings 处理之后:" << it.key()->name() << it.value() << it.key()->w() << it.key()->h();
    }
    m_worker->setMonitorPosition(monitorPosition);
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

qreal DisplayModule::globalScale() const
{
    Q_D(const DisplayModule);
    return d->m_model->uiScale();
}

void DisplayModule::setGlobalScale(qreal scale)
{
    Q_D(DisplayModule);
    d->m_worker->setUiScale(scale);
}

qreal DisplayModule::maxGlobalScale() const
{
    Q_D(const DisplayModule);
    return d->m_maxGlobalScale;
}

bool DisplayModule::colorTemperatureEnabled() const
{
    Q_D(const DisplayModule);
    return d->m_model->colorTemperatureEnabled();
}

void DisplayModule::setColorTemperatureEnabled(bool enabled)
{
    Q_D(DisplayModule);
    if (colorTemperatureEnabled() != enabled)
        d->m_worker->setColorTemperatureEnabled(enabled);
}

int DisplayModule::colorTemperatureMode() const
{
    Q_D(const DisplayModule);
    switch (d->m_model->adjustCCTMode()) { // 0 禁用 1 日落到日出 2 全天 3 自定义
    case 1:
        return 1; // 日落到日出
    case 3:
        return 2; // 自定义
    default:
        break;
    }
    return 0; // 全天
}

void DisplayModule::setColorTemperatureMode(int mode)
{
    int ccMode = 2;
    switch (mode) {
    case 1:
        ccMode = 1;
        break;
    case 2:
        ccMode = 3;
    default:
        break;
    }
    Q_D(DisplayModule);
    d->m_worker->SetMethodAdjustCCT(ccMode);
}

int DisplayModule::colorTemperature() const
{
    Q_D(const DisplayModule);
    int kelvin = d->m_model->colorTemperature();

    if (kelvin >= 6500)
        return 50 - (kelvin - 6500) / 300;
    else if (kelvin < 6500 && kelvin >= 1000)
        return 50 - (kelvin - 6500) / 100;
    else
        return 0;
}

void DisplayModule::setColorTemperature(int pos)
{
    int kelvin = pos > 50 ? (6500 - (pos - 50) * 100) : (6500 + (50 - pos) * 300);
    Q_D(DisplayModule);
    if (d->m_model->colorTemperature() != kelvin) {
        d->m_worker->setColorTemperature(kelvin);
    }
}

QString DisplayModule::customColorTempTimePeriod() const
{
    Q_D(const DisplayModule);
    QString timePeriod = d->m_model->customColorTempTimePeriod();

    QRegularExpression re("^((2[0-3]|[01][0-9]):[0-5][0-9])-((2[0-3]|[01][0-9]):[0-5][0-9])$");
    if (!re.match(timePeriod).hasMatch()) {
        timePeriod = "22:00-07:00";
    }
    return timePeriod;
}

void DisplayModule::setCustomColorTempTimePeriod(const QString &timePeriod)
{
    QString oldTimePeriod = customColorTempTimePeriod();
    if (oldTimePeriod == timePeriod) {
        return;
    }
    const QRegularExpression re("^((2[0-3]|[01][0-9]):[0-5][0-9])-((2[0-3]|[01][0-9]):[0-5][0-9])$");
    QRegularExpressionMatch match = re.match(timePeriod);
    if (!match.hasMatch()) {
        return;
    }
    QTime startTime = QTime::fromString(match.captured(1), "hh:mm");
    QTime endTime = QTime::fromString(match.captured(3), "hh:mm");
    // 计算时间差（秒），自动处理跨天情况
    int dSecs = startTime.secsTo(endTime);
    if (dSecs < 0) {
        dSecs += 86400; // 加上一天的秒数
    }
    // 确保最小时间差为5分钟
    const int MIN_DURATION = 300;
    if (dSecs == 0) {
        QRegularExpressionMatch oldMatch = re.match(oldTimePeriod);
        if (!oldMatch.hasMatch()) {
            return;
        }
        dSecs = (oldMatch.captured(1) <= oldMatch.captured(3)) ? MIN_DURATION : -MIN_DURATION;
    } else if (dSecs < MIN_DURATION) {
        dSecs = MIN_DURATION;
    } else {
        // 反向计算时间差
        dSecs = endTime.secsTo(startTime);
        if (dSecs < 0) {
            dSecs += 86400;
        }
        // 处理反向时间差
        dSecs = (dSecs < MIN_DURATION) ? -MIN_DURATION : 0;
    }
    if (dSecs != 0) {
        QRegularExpressionMatch oldMatch = re.match(oldTimePeriod);
        if (!oldMatch.hasMatch()) {
            return;
        }
        QTime oldStartTime = QTime::fromString(oldMatch.captured(1), "hh:mm");
        if (oldStartTime == startTime) {
            startTime = endTime.addSecs(-dSecs);
        } else {
            endTime = startTime.addSecs(dSecs);
        }
    }

    Q_D(DisplayModule);
    d->m_worker->setCustomColorTempTimePeriod(startTime.toString(DEFAULT_TIME_FORMAT) + "-" + endTime.toString(DEFAULT_TIME_FORMAT));
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

void DisplayModule::adsorptionScreen(QList<QObject *> listItems, QObject *pw, qreal scale)
{
    QQuickItem *tmpPw = dynamic_cast<QQuickItem *>(pw);
    if (!tmpPw) {
        return;
    }
    QList<ScreenData *> tmpListItems;
    ScreenData *pwItem = nullptr;
    for (auto obj : listItems) {
        QQuickItem *tmpObj = dynamic_cast<QQuickItem *>(obj);
        if (tmpObj) {
            ScreenData *item = new ScreenData(tmpObj, scale);
            tmpListItems.append(item);
            if (tmpObj == pw) {
                pwItem = item;
            }
        }
    }
    if (tmpListItems.isEmpty()) {
        return;
    }
    ConcatScreen *concatScreen = new ConcatScreen(tmpListItems, pwItem);
    concatScreen->adsorption();
    delete concatScreen;
    qDeleteAll(tmpListItems);
}

void DisplayModule::executemultiScreenAlgo(QList<QObject *> listItems, QObject *pw, qreal scale)
{
    QQuickItem *tmpPw = dynamic_cast<QQuickItem *>(pw);
    if (!tmpPw) {
        return;
    }
    QList<ScreenData *> tmpListItems;
    ScreenData *pwItem = nullptr;
    for (auto obj : listItems) {
        QQuickItem *tmpObj = dynamic_cast<QQuickItem *>(obj);
        if (tmpObj) {
            ScreenData *item = new ScreenData(tmpObj, scale);
            tmpListItems.append(item);
            if (tmpObj == pw) {
                pwItem = item;
            }
        }
    }
    if (tmpListItems.size() < 2 || !pwItem) {
        return;
    }
    ConcatScreen *concatScreen = new ConcatScreen(tmpListItems, pwItem);
    concatScreen->executemultiScreenAlgo(true);
    delete concatScreen;
    qDeleteAll(tmpListItems);
}

void DisplayModule::applySettings(QList<QObject *> listItems, qreal scale)
{
    QList<ScreenData *> tmpListItems;
    for (auto obj : listItems) {
        QQuickItem *tmpObj = dynamic_cast<QQuickItem *>(obj);
        if (tmpObj) {
            ScreenData *item = new ScreenData(tmpObj, scale);
            tmpListItems.append(item);
        }
    }
    Q_D(DisplayModule);
    d->setScreenPosition(tmpListItems);
    qDeleteAll(tmpListItems);
}

void DisplayModule::applyChanged()
{
    Q_D(DisplayModule);
    if (!d->m_model->monitorModeChanging()) {
        return;
    }
    d->m_model->setmodeChanging(false);
    DccScreen *tmpPw = qobject_cast<DccScreen *>(sender());
    if (!tmpPw || d_ptrDisplayModule->m_model->displayMode() != EXTEND_MODE) {
        return;
    }
    QList<ScreenData *> tmpListItems;
    ScreenData *pwItem = nullptr;
    for (auto obj : virtualScreens()) {
        if (obj) {
            ScreenData *item = new ScreenData(obj);
            tmpListItems.append(item);
            if (obj == tmpPw) {
                pwItem = item;
            }
        }
    }
    if (tmpListItems.size() < 2 || !pwItem) {
        return;
    }
    ConcatScreen *concatScreen = new ConcatScreen(tmpListItems, pwItem);
    concatScreen->executemultiScreenAlgo(false);
    d->setScreenPosition(tmpListItems);
    delete concatScreen;
    qDeleteAll(tmpListItems);
}

DCC_FACTORY_CLASS(DisplayModule)

} // namespace dccV25

#include "displaymodule.moc"
