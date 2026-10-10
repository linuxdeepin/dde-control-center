// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef EYE_COMFORT_MODE_ITEM_H
#define EYE_COMFORT_MODE_ITEM_H

#include "commoniconbutton.h"
#include "eyecomfortmodeapplet.h"

#include <QWidget>

namespace Dock {
class TipsWidget;
}

class QuickPanelWidget;

class EyeComfortModeItem : public QWidget
{
    Q_OBJECT

public:

    enum ThemeType {
        Light,
        Dark,
        Auto
    };

    explicit EyeComfortModeItem(QWidget *parent = nullptr);

    QWidget *tipsWidget();
    QWidget *popupApplet();
    QWidget *quickPanel();
    const QString contextMenu() const;
    void invokeMenuItem(const QString menuId, const bool checked);
    void refreshIcon();
    void updateTips();
    QString displayName() const;
    bool airplaneEnable();
    ThemeType getThemeType(const QString &globalTheme);
    void setAppletHeight(int height);

signals:
    void requestHideApplet();
    void requestExpand();

public slots:
    void updateDescription();

protected:
    void resizeEvent(QResizeEvent *e);

private:
    void init();

private:
    Dock::TipsWidget *m_tipsLabel;
    EyeComfortmodeApplet *m_applet;
    CommonIconButton *m_icon;
    QPixmap m_iconPixmap;
    QuickPanelWidget *m_quickPanel;
    bool m_supportColorTemperature;
    ThemeType m_themeType;
};

#endif // EYE_COMFORT_MODE_ITEM_H
