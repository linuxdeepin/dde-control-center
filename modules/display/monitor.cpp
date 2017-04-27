#include "monitor.h"

using namespace dcc::display;

Monitor::Monitor(QObject *parent)
    : QObject(parent),
      m_brightness(1.)
{

}

void Monitor::setX(const int x)
{
    if (m_x == x)
        return;

    m_x = x;

    emit xChanged(m_x);
    emit geometryChanged();
}

void Monitor::setY(const int y)
{
    if (m_y == y)
        return;

    m_y = y;

    emit yChanged(m_y);
    emit geometryChanged();
}

void Monitor::setW(const int w)
{
    if (m_w == w)
        return;

    m_w = w;

    emit wChanged(m_w);
    emit geometryChanged();
}

void Monitor::setH(const int h)
{
    if (m_h == h)
        return;

    m_h = h;

    emit hChanged(m_h);
    emit geometryChanged();
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

    emit rotateChanged(m_rotate);
}

void Monitor::setBrightness(const double brightness)
{
    if (m_brightness == brightness)
        return;

    m_brightness = brightness;

    emit brightnessChanged(m_brightness);
}

void Monitor::setName(const QString &name)
{
    m_name = name;
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
    if (m_currentMode == resolution)
        return;

    m_currentMode = resolution;

    emit currentModeChanged(m_currentMode);
}

void Monitor::setModeList(const ResolutionList &modeList)
{
    m_modeList.clear();

    // NOTE: ignore resolution less than 1024x768
    for (auto m : modeList)
        if (m.width() >= 1024 && m.height() >= 768)
            m_modeList.append(m);
}
