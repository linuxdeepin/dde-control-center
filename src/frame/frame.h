/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             listenerri <listenerri@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             listenerri <listenerri@gmail.com>
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

#ifndef FRAME_H
#define FRAME_H

#include <com_deepin_daemon_display.h>
#include <com_deepin_dde_launcher.h>
#include <com_deepin_daemon_appearance.h>

#include <QStack>
#include <QPropertyAnimation>

#include <DBlurEffectWidget>
#include <DPlatformWindowHandle>
#include <DWindowManagerHelper>
#include <dregionmonitor.h>

#define BUTTON_LEFT     1
#define BUTTON_RIGHT    3
#define FRAME_WIDTH     408
#define NAVBAR_WIDTH    56

using DBusDisplay = com::deepin::daemon::Display;
using LauncherInter = com::deepin::dde::Launcher;
using Appearance = com::deepin::daemon::Appearance;

DWIDGET_USE_NAMESPACE

class FrameWidget;
class FrameContentWrapper;
class SettingsWidget;
class NavigationBar;
class MainWidget;

namespace dcc
{

class ContentWidget;

}

using dcc::ContentWidget;

class Frame : public DBlurEffectWidget
{
    Q_OBJECT

    friend class DBusControlCenterService;

public:
    explicit Frame(QWidget *parent = 0);

    void backToHome();
    void setDebugAutoHide(const bool autoHide);
    QString currentModuleName() const;

public Q_SLOTS:
    void startup();

    void pushWidget(ContentWidget * const w);
    void popWidget();
    void showSettingsPage(const QString &moduleName, const QString &pageName, bool animation = false);
    void setAutoHide(const bool autoHide);

Q_SIGNALS:
    void rectChanged(const QRect &rect);
    void fontSizeChanged();
    void destRectChanged(const QRect &rect);

private Q_SLOTS:
    void init();
    void adjustShadowMask();

    void prepareAllSettingsPage();
    void onDelayKillerTimeout();
    void showAllSettings();
    void contentDetached(QWidget * const c);

    void onScreenRectChanged(const QRect &primaryRect);
    void onMouseButtonReleased(const QPoint &p, const int flag);

private:
    void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e) Q_DECL_OVERRIDE;
    void moveEvent(QMoveEvent *e) Q_DECL_OVERRIDE;
    bool event(QEvent *event) Q_DECL_OVERRIDE;
    void show();
    void hide();
    void toggle();
    void hideImmediately();
    const QScreen *screenForGeometry(const QRect &rect) const;
    bool checkOnBoard(const QPoint &point);

private:
    QPointer<QWidget> m_frameWrapper;
    QPointer<FrameContentWrapper> m_contentWrapper;
    QPointer<NavigationBar> m_navigationBar;
    QPointer<SettingsWidget> m_allSettingsPage;
    QPointer<MainWidget> m_mainWidget;
    QTimer *m_delayKillerTimer;

    DRegionMonitor *m_mouseAreaInter;
    DBusDisplay *m_displayInter;
    LauncherInter *m_launcherInter;
    Appearance *m_appearanceInter;
    QStack<FrameWidget *> m_frameWidgetStack;

    QRect m_primaryRect;
    QPropertyAnimation m_appearAnimation;

    DPlatformWindowHandle m_platformWindowHandle;
    DWindowManagerHelper *m_wmHelper;

    bool m_shown;
    bool m_autoHide;
    bool m_debugAutoHide;
};

#endif // FRAME_H
