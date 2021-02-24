/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#pragma once

#include "interface/namespace.h"
#include "modules/modulewidget.h"
#include "modules/personalization/personalizationmodel.h"
#include "widgets/settingsitem.h"

#include "dtkwidget_global.h"

#include <DGuiApplicationHelper>

#include <QWidget>

using namespace dcc::widgets;

namespace dcc {
namespace personalization {
class PersonalizationModel;
}
}

namespace dcc {
namespace widgets {
class TitledSliderItem;
class ComboxWidget;
}
}

DWIDGET_BEGIN_NAMESPACE
class DSwitchButton;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace personalization {
class PerssonalizationThemeWidget;
class RoundColorWidget;

class RingColorWidget : public SettingsItem
{
    Q_OBJECT
public:
    explicit RingColorWidget(SettingsItem *parent = nullptr);
    virtual ~RingColorWidget() {}

    void setSelectedItem(RoundColorWidget* item)
    {
        m_selectedItem = item;
    }

    static const int EXTRA = 2; //2px extra space to avoid line cutted off

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    RoundColorWidget* m_selectedItem;
};

class PersonalizationGeneral : public QWidget
{
    Q_OBJECT
public:
    explicit PersonalizationGeneral(QWidget *parent = nullptr);
    void setModel(dcc::personalization::PersonalizationModel *model);
    inline PerssonalizationThemeWidget *getThemeWidget() { return m_Themes; }

protected:
    void paintEvent(QPaintEvent *event);

private:
    void updateActiveColors(RoundColorWidget *selectedWidget);
    void updateWMSwitcher(bool checked);
    void updateThemeColors(Dtk::Gui::DGuiApplicationHelper::ColorType type);

Q_SIGNALS:
    void showThemeWidget();
    void showFontsWidget();
    void requestSwitchWM();
    void requestWindowSwitchWM(bool value);
    void requestSetOpacity(int value);
    void requestSetMiniEffect(int effect);
    void requestSetActiveColor(const QString &color);
    void windowRadiusChanged(int radius);

private:
    void onOpacityChanged(std::pair<int, double> value);
    void onMiniEffectChanged(int index);
    void onActiveColorChanged(const QString &newColor);
    void onActiveColorClicked();
    void onCompositingAllowSwitchChanged(bool value);
    void onWindowRadiusChanged(int radius);

private:
    QVBoxLayout *m_centralLayout;             //主布局
    DTK_WIDGET_NAMESPACE::DSwitchButton *m_wmSwitch;  //是否开启特效
    dcc::widgets::TitledSliderItem *m_transparentSlider;  //透明度调节
    dcc::widgets::ComboxWidget *m_cmbMiniEffect;    //最小化效果
    dcc::personalization::PersonalizationModel *m_model;
    dcc::widgets:: TitledSliderItem *m_winRoundSlider;   // 自定义圆角(社区版功能)
    PerssonalizationThemeWidget *m_Themes;
    QList<RoundColorWidget *> m_activeColorsList;
    RingColorWidget *m_bgWidget;
    QWidget *m_switchWidget;
    bool m_bSystemIsServer;
    int m_windowRadius;
    Dtk::Gui::DGuiApplicationHelper::ColorType m_themeType;
};
}
}
