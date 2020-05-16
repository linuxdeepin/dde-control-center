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

#include "dtkwidget_global.h"

#include <QWidget>

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

Q_SIGNALS:
    void showThemeWidget();
    void showFontsWidget();
    void requestSwitchWM();
    void requestSetOpacity(int value);
    void requestSetMiniEffect(int effect);
    void requestSetActiveColor(const QString &color);

private:
    void onOpacityChanged(std::pair<int, double> value);
    void onMiniEffectChanged(int index);
    void onActiveColorChanged(const QString &newColor);
    void onActiveColorClicked();
    void onCompositingAllowSwitchChanged(bool value);

private:
    QVBoxLayout *m_centralLayout;             //主布局
    DTK_WIDGET_NAMESPACE::DSwitchButton *m_wmSwitch;  //是否开启特效
    dcc::widgets::TitledSliderItem *m_transparentSlider;  //透明度调节
    dcc::widgets::ComboxWidget *m_cmbMiniEffect;    //最小化效果
    dcc::personalization::PersonalizationModel *m_model;
    PerssonalizationThemeWidget *m_Themes;
    QList<RoundColorWidget *> m_activeColorsList;
    QWidget *m_bgWidget;
    QWidget *m_switchWidget;
    bool m_bSystemIsServer;
};
}
}
