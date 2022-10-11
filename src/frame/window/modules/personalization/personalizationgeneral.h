// SPDX-FileCopyrightText: 2017 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "modules/modulewidget.h"
#include "modules/personalization/personalizationmodel.h"

#include "dtkwidget_global.h"

#include <QWidget>
#include <DGuiApplicationHelper>

namespace dcc {
namespace personalization {
class PersonalizationModel;
}
}

namespace dcc {
namespace widgets {
class TitledSliderItem;
class ComboxWidget;
class SettingsItem;
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

class RingColorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RingColorWidget(QWidget *parent = nullptr) : m_selectedItem(nullptr)
    {
        setAccessibleName("RingColorWidget");
    }
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
    virtual ~PersonalizationGeneral();
    void setModel(dcc::personalization::PersonalizationModel *model);
    inline PerssonalizationThemeWidget *getThemeWidget() { return m_Themes; }

    void onShowMiniEffect(const QString &option);
    void onShowWindowMovedSwitch();

protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateActiveColors(RoundColorWidget *selectedWidget);
    void updateWMSwitcher(bool checked);
    void updateThemeColors(Dtk::Gui::DGuiApplicationHelper::ColorType type);

Q_SIGNALS:
    void showThemeWidget();
    void showFontsWidget();
    void requestSwitchWM();
    void requestWindowSwitchWM(bool value);
    void requestMovedWindowSwitchWM(bool value);
    void windowMovedVisibleChanged(bool value);
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
    DTK_WIDGET_NAMESPACE::DSwitchButton *m_windowMovedSwitch;  //是否开启“窗口移动时启用透明特效”
    QLabel *m_windowMovedLabel;
    QString m_displayData;
    dcc::personalization::PersonalizationModel *m_model;
    dcc::widgets::TitledSliderItem *m_winRoundSlider;   // 自定义圆角(社区版功能)
    PerssonalizationThemeWidget *m_Themes;
    QList<RoundColorWidget *> m_activeColorsList;
    RingColorWidget *m_bgWidget;
    QWidget *m_switchWidget;
    bool m_bSystemIsServer;
    int m_windowRadius;
    Dtk::Gui::DGuiApplicationHelper::ColorType m_themeType;
    bool m_isWayland;
    dcc::widgets::SettingsItem *m_movedWinSwitchItem;
    QVBoxLayout *m_winEffectVLayout;
    bool m_isShowWinEffect;
    QStringList m_options;
};
}
}
