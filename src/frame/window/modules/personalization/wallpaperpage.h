/*
   * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
   *
   * Author:     xiechuan <xiechuan@uniontech.com>
   *
   * Maintainer: xiechaun <xiechaun@uniontech.com>
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

#ifndef WALLPAPERPAGE_H
#define WALLPAPERPAGE_H

#include "interface/namespace.h"
#include "modules/modulewidget.h"
#include "modules/personalization/personalizationmodel.h"

#include "dtkwidget_global.h"

#include <DGuiApplicationHelper>

#include <QWidget>

namespace dcc {
namespace personalization {
class PersonalizationModel;
}
} // namespace dcc

namespace dcc {
namespace widgets {
class TitledSliderItem;
class ComboxWidget;
} // namespace widgets
} // namespace dcc

DWIDGET_BEGIN_NAMESPACE
class DSwitchButton;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace personalization {
class PerssonalizationThemeWidget;
class PersonalizationFontsWidget;
class RoundColorWidget;
class RingColorWidget;
class Wallpaper;
class WallpaperPage : public QWidget
{
    Q_OBJECT
public:
    explicit WallpaperPage(QWidget *parent = nullptr);
    void setModel(dcc::personalization::PersonalizationModel *model);
    inline PerssonalizationThemeWidget *getThemeWidget() const { return m_themes; }
    inline PersonalizationFontsWidget *getFontWidget() const { return m_fonts; }

protected:
    void paintEvent(QPaintEvent *event);

private:
    void updateActiveColors(RoundColorWidget *selectedWidget);
    void updateThemeColors(Dtk::Gui::DGuiApplicationHelper::ColorType type);

Q_SIGNALS:
    void showThemeWidget();
    void showFontsWidget();
    void requestSetActiveColor(const QString &color);

private:
    void onActiveColorChanged(const QString &newColor);
    void onActiveColorClicked();

private:
    dcc::personalization::PersonalizationModel *m_model;
    PerssonalizationThemeWidget *m_themes;
    PersonalizationFontsWidget *m_fonts;
    QList<RoundColorWidget *> m_activeColorsList;
    RingColorWidget *m_bgWidget;
    Dtk::Gui::DGuiApplicationHelper::ColorType m_themeType;
    Wallpaper *m_wallpaper;
};
} // namespace personalization
} // namespace DCC_NAMESPACE

#endif // WALLPAPERPAGE_H
