//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "monitor.h"

#include <math.h>
#include <QGuiApplication>


using namespace dccV25;
const double DoubleZero = 0.000001;

Monitor::Monitor(QObject *parent)
    : QObject(parent)
    , m_x(0)
    , m_y(0)
    , m_w(0)
    , m_h(0)
    , m_mmWidth(0)
    , m_mmHeight(0)
    , m_scale(-1.0)
    , m_rotate(0)
    , m_brightness(1.0)
    , m_enable(false)
    , m_canBrightness(true)
    , m_screenSensingMode(RotateMode::Normal)
{
}

void Monitor::setX(const int x)
{
    if (m_x == x)
        return;

    m_x = x;

    Q_EMIT xChanged(m_x);
    Q_EMIT geometryChanged();
}

void Monitor::setY(const int y)
{
    if (m_y == y)
        return;

    m_y = y;

    Q_EMIT yChanged(m_y);
    Q_EMIT geometryChanged();
}

void Monitor::setW(const int w)
{
    if (m_w == w)
        return;

    m_w = w;

    Q_EMIT wChanged(m_w);
}

void Monitor::setH(const int h)
{
    if (m_h == h)
        return;

    m_h = h;

    Q_EMIT hChanged(m_h);
}

void Monitor::setMmWidth(const uint mmWidth)
{
    m_mmWidth = mmWidth;
}

void Monitor::setMmHeight(const uint mmHeight)
{
    m_mmHeight = mmHeight;
}

void Monitor::setScale(const double scale)
{
    if (fabs(m_scale - scale) < DoubleZero)
        return;

    m_scale = scale;

    Q_EMIT scaleChanged(m_scale);
}

void Monitor::setPrimary(const QString &primaryName)
{
    m_primary = primaryName;
}

void Monitor::setRotate(const quint16 rotate)
{
    if (m_rotate == rotate)
        return;

    m_rotate = rotate;

    Q_EMIT rotateChanged(m_rotate);
}

void Monitor::setBrightness(const double brightness)
{
    if (fabs(m_brightness - brightness) < DoubleZero)
        return;

    m_brightness = brightness;

    Q_EMIT brightnessChanged(m_brightness);
}

void Monitor::setName(const QString &name)
{
    m_name = name;
}

void Monitor::setManufacturer(const QString &manufacturer)
{
    m_manufacturer = manufacturer;
}

void Monitor::setModel(const QString &model)
{
    m_model = model;
}

void Monitor::setCanBrightness(bool canBrightness)
{
    m_canBrightness = canBrightness;
}

void Monitor::setPath(const QString &path)
{
    m_path = path;
}

void Monitor::setRotateList(const QList<quint16> &rotateList)
{
    m_rotateList = rotateList;
}

void Monitor::setCurrentMode(const Resolution &resolution)
{
    m_currentMode = resolution;

    Q_EMIT currentModeChanged(m_currentMode);
}

bool compareResolution(const Resolution &first, const Resolution &second)
{
    long firstSum = long(first.width()) * first.height();
    long secondSum = long(second.width()) * second.height();
    if (firstSum > secondSum
        || (firstSum == secondSum && (first.rate() - second.rate()) > 0.000001)) {
        return true;
    }

    return false;
}

void Monitor::setModeList(const ResolutionList &modeList)
{
    m_modeList.clear();

    QList<int> miniMode;
    miniMode << 1024 << 768;

    for (auto m : modeList) {
        if (m.width() >= miniMode.at(0) && m.height() >= miniMode.at(1)) {
            m_modeList.append(m);
        }
    }
    std::sort(m_modeList.begin(), m_modeList.end(), compareResolution);

    Q_EMIT modelListChanged(m_modeList);
}


void Monitor::setAvailableFillModes(const QStringList &fillModeList)
{
    if (m_fillModeList == fillModeList)
        return;

    m_fillModeList = fillModeList;
    Q_EMIT availableFillModesChanged(m_fillModeList);
}

void Monitor::setCurrentFillMode(const QString currentFillMode)
{
    if (m_currentFillMode == currentFillMode)
        return;

    m_currentFillMode = currentFillMode;
    Q_EMIT currentFillModeChanged(currentFillMode);
}

void Monitor::setMonitorEnable(bool enable)
{
    if (m_enable == enable)
        return;

    m_enable = enable;
    Q_EMIT enableChanged(enable);
}

void Monitor::setBestMode(const Resolution &mode)
{
    if (m_bestMode == mode)
        return;
    m_bestMode = mode;

    Q_EMIT bestModeChanged();
}

void Monitor::setCurrentRotateMode(const unsigned char mode)
{
    RotateMode screenDate = static_cast<RotateMode>(mode);
    if (screenDate == RotateMode::Gravity)
        Q_EMIT currentRotateModeChanged();

    if (m_screenSensingMode != screenDate) {
        m_screenSensingMode = screenDate;
    }
}

bool Monitor::isSameResolution(const Resolution &r1, const Resolution &r2)
{
    return r1.width() == r2.width() && r1.height() == r2.height();
}

bool Monitor::isSameRatefresh(const Resolution &r1, const Resolution &r2)
{
    return fabs(r1.rate() - r2.rate()) < 0.000001;
}

bool Monitor::hasResolution(const Resolution &r)
{
    for (auto m : m_modeList) {
        if (isSameResolution(m, r)) {
            return true;
        }
    }

    return false;
}

bool Monitor::hasResolutionAndRate(const Resolution &r)
{
    for (auto m : m_modeList) {
        if (fabs(m.rate() - r.rate()) < 0.000001 && m.width() == r.width() && m.height() == r.height()) {
            return true;
        }
    }

    return false;
}

bool Monitor::hasRatefresh(const double r)
{
    for (auto m : m_modeList) {
        if (fabs(m.rate() - r) < 0.000001) {
            return true;
        }
    }

    return false;
}

QScreen *Monitor::getQScreen()
{
    auto screens = QGuiApplication::screens();

    for(auto screen : screens) {
        //x11下，qt获取的名字和后端给的名字一致 wayland下，qt获取的序列号中包含名称
        if(screen->name() == name() || screen->model().contains(name()))
            return screen;
    }

    return nullptr;
}

std::optional<Resolution> Monitor::getResolutionById(quint32 id)
{
    for (auto res : m_modeList) {
        if (res.id() == id)
            return res;
    }
    return std::nullopt;
}

void Monitor::setWallpaper(const QString &wallpaper)
{
    if (m_wallpaper == wallpaper)
        return;
    m_wallpaper = wallpaper;

    Q_EMIT wallpaperChanged();
}
