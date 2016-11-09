#include "mousemodel.h"

MouseModel::MouseModel(QObject *parent)
{
    Q_UNUSED(parent);
    m_baseSettings  = new MouseModelBaseSettings;
    m_mouseSettings = new MouseModelMouseSettings;
    m_touchSettings = new MouseModelMouseSettings;
    m_trackSettings = new MouseModelThinkpadSettings;
}

MouseModel::~MouseModel()
{
    m_baseSettings->deleteLater();
    m_mouseSettings->deleteLater();
    m_touchSettings->deleteLater();
    m_trackSettings->deleteLater();
}

MouseModelBaseSettings *MouseModel::getBaseSettings()
{
    return m_baseSettings;
}

MouseModelMouseSettings *MouseModel::getMouseSettings()
{
    return m_mouseSettings;
}

MouseModelMouseSettings *MouseModel::getTouchSettings()
{
    return m_touchSettings;
}

MouseModelThinkpadSettings *MouseModel::getTrackSettings()
{
    return m_trackSettings;
}
