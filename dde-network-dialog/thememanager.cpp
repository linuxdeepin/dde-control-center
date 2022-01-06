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
#include "thememanager.h"
#include <DApplication>
#include <DPlatformTheme>
#include <DLineEdit>

DGUI_USE_NAMESPACE

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_themeType(UnknownType)
{
    appThemeTypeChanged(Dtk::Gui::DGuiApplicationHelper::instance()->themeType());
    // 主题发生变化触发的信号
    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged, this, &ThemeManager::appThemeTypeChanged);
}

ThemeManager::~ThemeManager()
{
}

QString ThemeManager::getIcon(QString path)
{
    if (path.startsWith("common")) {
        return ":/" + path;
    }
    switch (m_themeType) {
    case GreeterType:
    case LockType:
    case DarkType:
        return ":/dark/" + path;
    default:
        return ":/light/" + path;
    }
}

void ThemeManager::setThemeType(ThemeType type)
{
    if (type != m_themeType) {
        m_themeType = type;
        switch (m_themeType) {
        case LockType: {
            Dtk::Gui::DGuiApplicationHelper::instance()->setPaletteType(Dtk::Gui::DGuiApplicationHelper::ColorType::LightType);

            DPalette palette = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();

            palette.setColor(DPalette::All, DPalette::BrightText, QColor(255, 255, 255));         // 文本颜色
            palette.setColor(DPalette::All, DPalette::Window, QColor(235, 235, 235, static_cast<int>(0.05 * 255))); // 背景颜色

            Dtk::Gui::DGuiApplicationHelper::instance()->setApplicationPalette(palette);
        } break;
        case GreeterType: {
            Dtk::Gui::DGuiApplicationHelper::instance()->setPaletteType(Dtk::Gui::DGuiApplicationHelper::ColorType::LightType);

            DPalette palette = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();

            palette.setColor(DPalette::All, DPalette::BrightText, QColor(255, 255, 255)); // 文本颜色
            palette.setColor(DPalette::All, DPalette::Window, QColor(210, 210, 210));      // 背景颜色

            Dtk::Gui::DGuiApplicationHelper::instance()->setApplicationPalette(palette);
        } break;
        default: break;
        }
        emit themeChanged(m_themeType);
    }
}

void ThemeManager::updateInputStyle(Dtk::Widget::DLineEdit *inputEdit)
{
    switch (m_themeType) {
    case LockType: {
        QPalette editPalette = inputEdit->palette();
        editPalette.setColor(QPalette::Button, QColor(255, 255, 255, static_cast<int>(0.2 * 255)));
        inputEdit->setPalette(editPalette);
    } break;
    case GreeterType: {
        QPalette editPalette = inputEdit->palette();
        editPalette.setColor(QPalette::Button, QColor(55, 55, 55, static_cast<int>(0.2 * 255)));
        inputEdit->setPalette(editPalette);
    } break;
    case DarkType: {
        QPalette editPalette = inputEdit->palette();
        editPalette.setColor(QPalette::Button, QColor(255, 255, 255, static_cast<int>(0.2 * 255)));
        inputEdit->setPalette(editPalette);
    } break;
    case LightType: {
        QPalette editPalette = inputEdit->palette();
        editPalette.setColor(QPalette::Button, QColor(0, 0, 0, static_cast<int>(0.2 * 255)));
        inputEdit->setPalette(editPalette);
    } break;
    default:
        break;
    }
}

void ThemeManager::appThemeTypeChanged(Dtk::Gui::DGuiApplicationHelper::ColorType themeType)
{
    if (m_themeType >= UnknownType) {
        switch (themeType) {
        case Dtk::Gui::DGuiApplicationHelper::ColorType::DarkType:
            setThemeType(DarkType);
            break;
        case Dtk::Gui::DGuiApplicationHelper::ColorType::LightType:
            setThemeType(LightType);
            break;
        default:
            break;
        }
    }
}

QColor ThemeManager::backgroundColor()
{
    switch (m_themeType) {
    case DarkType:
        return QColor(0, 0, 0, static_cast<int>(0.03 * 255));
    case LockType:
        return QColor(235, 235, 235, static_cast<int>(0.05 * 255));
    case GreeterType:
        return QColor(105, 105, 105);
    default:
        return QColor(255, 255, 255, static_cast<int>(0.03 * 255));
    }
    Q_UNREACHABLE();
    return QColor(255, 255, 255, static_cast<int>(0.03 * 255));
}

QColor ThemeManager::lineColor()
{
    switch (m_themeType) {
    case DarkType:
        return QColor(255, 255, 255, static_cast<int>(0.05 * 255));
    case LockType:
        return QColor(0, 0, 0, static_cast<int>(0.1 * 255));
    default:
        return QColor(0, 0, 0, static_cast<int>(0.1 * 255));
    }
    Q_UNREACHABLE();
    return QColor(255, 255, 255, static_cast<int>(0.03 * 255));
}

QColor ThemeManager::itemBackgroundColor()
{
    switch (m_themeType) {
    case DarkType:
        return QColor(255, 255, 255, static_cast<int>(255 * 0.10));
    case LockType:
        return QColor(255, 255, 255, static_cast<int>(255 * 0.10));
    case GreeterType:
        return QColor(155, 155, 155);
    default:
        return QColor(0, 0, 0, static_cast<int>(255 * 0.10));
    }
    Q_UNREACHABLE();
    return QColor(255, 255, 255, static_cast<int>(0.03 * 255));
}
