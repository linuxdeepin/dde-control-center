// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
        LockType,
        GreeterType,
        UnknownType,
        LightType,
        DarkType,
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
