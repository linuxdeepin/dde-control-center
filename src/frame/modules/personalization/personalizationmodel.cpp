// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "personalizationmodel.h"
#include "model/thememodel.h"
#include "model/fontmodel.h"
#include "model/fontsizemodel.h"

using namespace dcc;
using namespace dcc::personalization;

PersonalizationModel::PersonalizationModel(QObject *parent)
    : QObject(parent)
    , m_opacity(std::pair<int, double>(2, 0.4f))
    , m_allowSwitch(false)
{
    m_windowModel    = new ThemeModel(this);
    m_iconModel      = new ThemeModel(this);
    m_mouseModel     = new ThemeModel(this);
    m_standFontModel = new FontModel(this);
    m_monoFontModel  = new FontModel(this);
    m_fontSizeModel  = new FontSizeModel(this);
    m_is3DWm = true;
    m_isMoveWindow = false;
    m_miniEffect = 0;
}

PersonalizationModel::~PersonalizationModel()
{

}

void PersonalizationModel::setIs3DWm(const bool is3d)
{
    if (is3d != m_is3DWm) {
        m_is3DWm = is3d;
        Q_EMIT wmChanged(is3d);
    }
}

bool PersonalizationModel::is3DWm() const
{
    return m_is3DWm;
}

void PersonalizationModel::setIsMoveWindow(const bool isMoveWindow)
{
    if (isMoveWindow != m_isMoveWindow) {
        m_isMoveWindow = isMoveWindow;
        Q_EMIT moveWindowChanged(isMoveWindow);
    }
}

bool PersonalizationModel::isMoveWindow() const
{
    return m_isMoveWindow;
}

void PersonalizationModel::setWindowRadius(int radius)
{
    if (m_windowRadius != radius)
        m_windowRadius = radius;

    Q_EMIT  onWindowRadiusChanged(radius);
}

int PersonalizationModel::windowRadius()
{
    return m_windowRadius;
}

void PersonalizationModel::setOpacity(std::pair<int, double> opacity)
{
    if (m_opacity == opacity) return;

    m_opacity = opacity;

    Q_EMIT onOpacityChanged(opacity);
}

void PersonalizationModel::setMiniEffect(const int &effect)
{
    if(m_miniEffect == effect) return;

    m_miniEffect=effect;

    Q_EMIT onMiniEffectChanged(effect);
}

void PersonalizationModel::setActiveColor(const QString &color)
{
    if (m_activeColor == color)
        return;

    m_activeColor = color;

    Q_EMIT onActiveColorChanged(color);
}

void PersonalizationModel::setCompositingAllowSwitch(bool value)
{
    if (m_allowSwitch == value)
        return;
    m_allowSwitch = value;

    Q_EMIT onCompositingAllowSwitch(value);
}
