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

#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H
#include <DSingleton>
#include <DGuiApplicationHelper>

namespace Dtk {
    namespace Widget {
        class DLineEdit;
    }
}

class ThemeManager : public QObject, public Dtk::Core::DSingleton<ThemeManager>
{
    Q_OBJECT

    friend Dtk::Core::DSingleton<ThemeManager>;

public:
    enum ThemeType {
        UnknownType,
        LightType,
        DarkType,
        LoginType
    };

Q_SIGNALS:
    void themeChanged(ThemeType type);

public:
    ~ThemeManager();
    QString getIcon(QString path);
    QColor backgroundColor();
    QColor lineColor();
    QColor itemBackgroundColor();
    void setThemeType(ThemeType type);
    void updateInputStyle(Dtk::Widget::DLineEdit *inputEdit);

protected Q_SLOT:
    void appThemeTypeChanged(Dtk::Gui::DGuiApplicationHelper::ColorType themeType);

protected:
    ThemeManager(QObject *parent = nullptr);

private:
    ThemeType m_themeType;
};

#endif // THEMEMANAGER_H
