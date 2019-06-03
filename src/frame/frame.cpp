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

#include "frame.h"
#include "framewidget.h"
#include "framecontentwrapper.h"
#include "settingswidget.h"
#include "mainwidget.h"
#include "navigationbar.h"

#include <QApplication>
#include <QKeyEvent>
#include <QScreen>
#include <QGSettings>
#include <qpa/qplatformwindow.h>
#include <DPlatformWindowHandle>
#include <DForeignWindow>

DWIDGET_USE_NAMESPACE

Frame::Frame(QWidget *parent)
    : DBlurEffectWidget(parent),

      m_frameWrapper(new QWidget(this)),
      m_contentWrapper(new FrameContentWrapper(this)),
      m_allSettingsPage(nullptr),
      m_delayKillerTimer(new QTimer(this)),
      m_mouseAreaInter(new DRegionMonitor(this)),
      m_displayInter(new DBusDisplay("com.deepin.daemon.Display", "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this)),
      m_launcherInter(new LauncherInter("com.deepin.dde.Launcher", "/com/deepin/dde/Launcher", QDBusConnection::sessionBus(), this)),
      m_appearanceInter(new Appearance("com.deepin.daemon.Appearance", "/com/deepin/daemon/Appearance", QDBusConnection::sessionBus(), this)),

      m_primaryRect(m_displayInter->primaryRect()),
      m_appearAnimation(this, "geometry"),

      m_platformWindowHandle(this),
      m_wmHelper(DWindowManagerHelper::instance()),

      m_shown(false),
      m_autoHide(true),
      m_debugAutoHide(true)
{
    // use original coordinate
    m_mouseAreaInter->setCoordinateType(DRegionMonitor::Original);

    m_appearanceInter->setSync(false, false);
    // set async
    m_displayInter->setSync(false);

    // set init data
    m_appearAnimation.setEasingCurve(QEasingCurve::OutCubic);

    m_platformWindowHandle.setEnableBlurWindow(false);
    m_platformWindowHandle.setTranslucentBackground(true);
    m_platformWindowHandle.setWindowRadius(0);
    m_platformWindowHandle.setBorderWidth(0);
    m_platformWindowHandle.setShadowOffset(QPoint(0, 0));
    m_platformWindowHandle.setShadowColor(QColor(0, 0, 0, 255 * 0.5));
    m_platformWindowHandle.setShadowRadius(20);
//    m_platformWindowHandle.setShadowColor(Qt::red);

    m_delayKillerTimer->setSingleShot(true);
    m_delayKillerTimer->setInterval(60 * 1000);

    m_navigationBar = new NavigationBar;
    m_navigationBar->setFixedWidth(NAVBAR_WIDTH);
    m_navigationBar->setVisible(false);

    QHBoxLayout *centralLayout = new QHBoxLayout;
    centralLayout->addWidget(m_navigationBar.data());
    centralLayout->addWidget(m_contentWrapper.data());
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(0, 0, 0, 0);

    m_frameWrapper->setLayout(centralLayout);
    m_frameWrapper->setFixedWidth(FRAME_WIDTH);
    m_frameWrapper->move(0, 0);
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMaximumWidth(FRAME_WIDTH);
    setMaskColor(DBlurEffectWidget::DarkColor);

    resize(0, height());

    auto setOpacity = [=] (double opacity) {
        setMaskAlpha(opacity * 255);
    };

    connect(m_navigationBar, &NavigationBar::requestModule, this, [=](const QString &module) { showSettingsPage(module, QString(), true); });
    connect(m_delayKillerTimer, &QTimer::timeout, this, &Frame::onDelayKillerTimeout);
    connect(m_displayInter, &DBusDisplay::PrimaryRectChanged, this, &Frame::onScreenRectChanged);
    connect(m_launcherInter, &LauncherInter::Shown, this, &Frame::hideImmediately);
    connect(m_wmHelper, &DWindowManagerHelper::hasCompositeChanged, this, &Frame::adjustShadowMask);
    connect(&m_appearAnimation, &QPropertyAnimation::stateChanged, this, &Frame::adjustShadowMask, Qt::QueuedConnection);
    connect(&m_appearAnimation, &QPropertyAnimation::finished, this, [=] { Q_EMIT rectChanged(geometry()); });
    connect(m_appearanceInter, &Appearance::OpacityChanged, this, setOpacity);

    setOpacity(m_appearanceInter->opacity());

    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void Frame::startup()
{
    show();
}

void Frame::pushWidget(ContentWidget *const w)
{
    Q_ASSERT(!m_frameWidgetStack.empty());

    FrameWidget *fw = new FrameWidget(m_contentWrapper.data());
    fw->setContent(w);
    fw->show();
    w->setVisible(true);

    m_frameWidgetStack.last()->hide();
    m_frameWidgetStack.push(fw);
    m_navigationBar->raise();

    const bool navBarVisible = m_frameWidgetStack.size() > 1;
    QTimer::singleShot(100, m_navigationBar, [=] { m_navigationBar->setVisible(navBarVisible); });

    connect(w, &ContentWidget::back, this, &Frame::popWidget, Qt::UniqueConnection);
    connect(fw, &FrameWidget::contentDetached, this, &Frame::contentDetached, Qt::UniqueConnection);
}

void Frame::popWidget()
{
    Q_ASSERT(m_frameWidgetStack.size() > 1);

    // ensure top widget is sender()
    ContentWidget *w = qobject_cast<ContentWidget *>(sender());

    // w is nullptr if call this function directly or call by other object
    Q_ASSERT(!w || w == m_frameWidgetStack.last()->content());

    // destroy the container
    m_frameWidgetStack.pop()->destroy();
    m_frameWidgetStack.last()->showBack();

    const bool navBarVisible = m_frameWidgetStack.size() > 1;
    QTimer::singleShot(100, m_navigationBar, [=] { m_navigationBar->setVisible(navBarVisible); });
}

void Frame::init()
{
    // main page
    m_mainWidget = new MainWidget(m_contentWrapper.data());
    connect(m_mainWidget.data(), &MainWidget::showAllSettings, this, &Frame::showAllSettings);
    connect(m_mainWidget.data(), &MainWidget::showSettingPage, this, &Frame::showSettingsPage);
    m_frameWidgetStack.push(m_mainWidget.data());

    // frame position adjust
    onScreenRectChanged(m_primaryRect);

    QTimer::singleShot(1, this, &Frame::adjustShadowMask);

#ifdef QT_DEBUG
//    showSettingsPage("network", QString());
#endif

#ifdef DCC_PREPARE_INIT
    prepareAllSettingsPage();
#endif
}

void Frame::adjustShadowMask()
{
    if (!m_wmHelper->hasComposite() ||
        !m_shown ||
        m_appearAnimation.state() == QPropertyAnimation::Running)
    {
        return;
    }

    const int radius = m_platformWindowHandle.shadowRadius();
    const QSize s = size();
    const qreal ratio = devicePixelRatioF();
    const QRect r(0, radius / ratio, s.width() + radius / ratio, s.height());

    m_platformWindowHandle.setFrameMask(r);
}

void Frame::setAutoHide(const bool autoHide)
{
    m_autoHide = autoHide;
}

void Frame::prepareAllSettingsPage()
{
    if (!m_allSettingsPage.isNull())
        return;

    m_allSettingsPage = new SettingsWidget(this);
    m_allSettingsPage->setVisible(false);

    connect(m_allSettingsPage, &SettingsWidget::moduleVisibleChanged, m_navigationBar, &NavigationBar::setModuleVisible);
    connect(m_allSettingsPage, &SettingsWidget::moduleVisibleChanged, m_mainWidget.data(), &MainWidget::requestModuleVisible);
    connect(m_allSettingsPage, &SettingsWidget::currentModuleChanged, m_navigationBar,
            static_cast<void (NavigationBar::*)(const QString &)>(&NavigationBar::setModuleChecked));
}

void Frame::onDelayKillerTimeout()
{
    const QGSettings gsettings("com.deepin.dde.control-center", "/com/deepin/dde/control-center/");
    if (gsettings.keys().contains("autoExit") && gsettings.get("auto-exit").toBool())
    {
        if (isVisible())
            return m_delayKillerTimer->start();
        qWarning() << "Killer Timeout, now quiiting...";
        qApp->quit();
    } else {
#ifndef DCC_KEEP_SETTINGS_LIVE
    Q_ASSERT(!m_allSettingsPage.isNull());

    // pass if not in main page
    if (m_frameWidgetStack.size() > 1)
        return;

    m_allSettingsPage->deleteLater();
#endif // DCC_KEEP_SETTINGS_LIVE
    }
}

void Frame::showAllSettings()
{
    Q_ASSERT(m_frameWidgetStack.size() == 1);

    prepareAllSettingsPage();

    pushWidget(m_allSettingsPage);
}

void Frame::showSettingsPage(const QString &moduleName, const QString &pageName, bool animation)
{
    // ensure current is main page or all settings page
    while (m_frameWidgetStack.size() > 1 && m_frameWidgetStack.last()->content() != m_allSettingsPage)
        popWidget();

    // current is main page
    if (m_frameWidgetStack.size() == 1)
        prepareAllSettingsPage();

    if (pageName.isEmpty() && m_frameWidgetStack.size() == 1)
        showAllSettings();

    // show specified page
//    m_allSettingsPage->showModulePage(moduleName, pageName);
    QMetaObject::invokeMethod(m_allSettingsPage, "showModulePage", Qt::QueuedConnection, Q_ARG(QString, moduleName), Q_ARG(QString, pageName), Q_ARG(bool, animation));

    if (!m_shown)
        show();
}

void Frame::backToHome()
{
    while (m_frameWidgetStack.size() > 1)
        popWidget();
}

void Frame::setDebugAutoHide(const bool autoHide)
{
    m_debugAutoHide = autoHide;
}

QString Frame::currentModuleName() const
{
    return m_navigationBar->currentModuleName();
}

void Frame::contentDetached(QWidget *const c)
{
    ContentWidget *cw = qobject_cast<ContentWidget *>(c);
    Q_ASSERT(cw);

    if (cw != m_allSettingsPage) {
        return m_allSettingsPage->contentPopuped(cw);
    }
}

void Frame::onScreenRectChanged(const QRect &primaryRect)
{
    // pass invalid data
    if (primaryRect.isNull() || primaryRect.isEmpty()) {
        return;
    }

    // 控制中心一直在主屏显示
    if (windowHandle())
        windowHandle()->setScreen(qApp->primaryScreen());

    m_primaryRect = primaryRect;

    // 假定控制中心一直在主屏
    const qreal ratio = qApp->primaryScreen()->devicePixelRatio();
    const auto h = m_primaryRect.height() / ratio;

    setFixedHeight(h);
    m_frameWrapper->setFixedHeight(h);

    QTimer::singleShot(500, this, [=] {
        // FIXME: The signal is too fast and the screen is not adjusted
        // ###(zccrs): 窗口改变所在屏幕之后，新屏幕的缩放比可能和之前的屏幕不一致。
        // 假定以下情况，屏幕A： 100x100，缩放=1.0，屏幕B：200x200，缩放=2.0
        // 窗口从屏幕A移动屏幕B之后，计算出的新geometry和旧geometry一致，使用QWidget
        // 或QWindow的接口更改窗口大小时会判断参数是否和旧的一致，导致更新位置和大小
        // 失效，因此此处直接使用 QPlatformWindow 调整窗口位置和大小。
        if (windowHandle() && windowHandle()->handle()) {
            QPlatformWindow *native_window = windowHandle()->handle();
            QRect rect = native_window->geometry();

            rect.setWidth(static_cast<int>(FRAME_WIDTH * ratio));
            rect.setHeight(m_primaryRect.height());
            //the frame might be invisible here as rotating screen in control center hides the frame
            rect.moveLeft(m_primaryRect.right() - (m_shown ? (rect.width() - 1) : 0));
            rect.moveTop(m_primaryRect.top());

            native_window->setGeometry(rect);
            Q_EMIT rectChanged(rect);
        } else {
            DBlurEffectWidget::move(m_primaryRect.x() + m_primaryRect.width() / ratio - (m_shown ? (width() - 1) : 0), m_primaryRect.y());
        }
    });
}

void Frame::onMouseButtonReleased(const QPoint &p, const int flag)
{
    if (flag != BUTTON_LEFT && flag != BUTTON_RIGHT) {
        return;
    }

    if (!m_autoHide || !m_debugAutoHide) {
        qDebug() << "forbid hide by m_autoHide" << m_autoHide << m_debugAutoHide;

        return;
    }

    if (isVisible() && (windowHandle()->handle()->geometry().contains(p) || checkOnBoard(p)))
        return;

    // ready to hide frame
    hide();
}

void Frame::keyPressEvent(QKeyEvent *e)
{
    DBlurEffectWidget::keyPressEvent(e);

    switch (e->key()) {
#ifdef QT_DEBUG
    case Qt::Key_Escape:        qApp->quit();                               break;
#endif
    case Qt::Key_F1:            QProcess::startDetached("dman", QStringList("dde"));  break;
    default:;
    }
}

void Frame::resizeEvent(QResizeEvent *e)
{
    DBlurEffectWidget::resizeEvent(e);

    Q_EMIT rectChanged(geometry());
}

void Frame::moveEvent(QMoveEvent *e)
{
    DBlurEffectWidget::moveEvent(e);

    Q_EMIT rectChanged(geometry());
}

void Frame::show()
{
    if (m_appearAnimation.state() == QPropertyAnimation::Running)
        return;

    // show
    m_shown = true;

    QRect r = qApp->primaryScreen()->geometry();

    const QScreen *screen = screenForGeometry(r);
    if (screen) {
        const qreal dpr = screen->devicePixelRatio();
        const QRect screenGeo = screen->geometry();
        r.moveTopLeft(screenGeo.topLeft() + (r.topLeft() - screenGeo.topLeft()) / dpr);
    }

    // animation
    r.setLeft(r.x() + r.width());
    m_appearAnimation.setStartValue(r);
    r.setLeft(r.x() + r.width() - FRAME_WIDTH);
    Q_EMIT destRectChanged(r);
    m_appearAnimation.setEndValue(r);
    m_appearAnimation.start();

    // show frame
    DBlurEffectWidget::show();
    DBlurEffectWidget::activateWindow();

    // notify top widget appear
    if (m_frameWidgetStack.last())
    {
        QTimer::singleShot(m_frameWidgetStack.last()->animationDuration(), [=] {
            if (m_frameWidgetStack.last()->content())
                Q_EMIT m_frameWidgetStack.last()->content()->appear();
            else
                Q_EMIT static_cast<MainWidget*>(m_frameWidgetStack.first())->appear();
        });
    }

    // register global mouse area
    m_mouseAreaInter->registerRegion();

    // connect signal
    connect(m_mouseAreaInter, &DRegionMonitor::buttonRelease, this, &Frame::onMouseButtonReleased, Qt::UniqueConnection);

    // prepare all settings page
    m_delayKillerTimer->stop();
    QTimer::singleShot(m_frameWidgetStack.last()->animationDuration(), this, &Frame::prepareAllSettingsPage);
}

void Frame::hide()
{
    if (m_appearAnimation.state() == QPropertyAnimation::Running)
        return;

    // set shown to false
    m_shown = false;

    // reset auto-hide
    m_autoHide = true;

    QRect r = qApp->primaryScreen()->geometry();

    const QScreen *screen = screenForGeometry(r);
    if (screen) {
        const qreal dpr = screen->devicePixelRatio();
        const QRect screenGeo = screen->geometry();
        r.moveTopLeft(screenGeo.topLeft() + (r.topLeft() - screenGeo.topLeft()) / dpr);
    }

    // animation
    r.setLeft(r.x() + r.width() - FRAME_WIDTH);
    m_appearAnimation.setStartValue(r);
    r.setLeft(r.x() + r.width());
    Q_EMIT destRectChanged(r);
    m_appearAnimation.setEndValue(r);
    m_appearAnimation.start();

    QTimer::singleShot(m_appearAnimation.duration(), this, &QFrame::hide);

    // notify top widget disappear
    if (m_frameWidgetStack.last()) {
        if ( m_frameWidgetStack.last()->content())
            Q_EMIT m_frameWidgetStack.last()->content()->disappear();
        else
            Q_EMIT static_cast<MainWidget*>(m_frameWidgetStack.first())->disappear();
    }

    m_mouseAreaInter->unregisterRegion();

    // disconnect signal
    disconnect(m_mouseAreaInter, &DRegionMonitor::buttonRelease, this, &Frame::onMouseButtonReleased);

    // free all settings page
    m_delayKillerTimer->start();
}

void Frame::toggle()
{
    if (m_appearAnimation.state() == QPropertyAnimation::Running)
        return;

    if (!m_shown)
    {
        show();
#ifdef DISABLE_MAIN_PAGE
        showAllSettings();
#endif
    } else {
        hide();
    }
}

void Frame::hideImmediately()
{
    // reset shown
    m_shown = false;

    // reset auto-hide
    m_autoHide = true;

    // reset position
    QRect r = qApp->primaryScreen()->geometry();

    const QScreen *screen = screenForGeometry(r);
    if (screen) {
        const qreal dpr = screen->devicePixelRatio();
        const QRect screenGeo = screen->geometry();
        r.moveTopLeft(screenGeo.topLeft() + (r.topLeft() - screenGeo.topLeft()) / dpr);
    }
    r.setLeft(r.x() + r.width());
    Q_EMIT destRectChanged(r);
    move(r.topLeft());

    DBlurEffectWidget::hide();

    // notify top widget disappear
    if (m_frameWidgetStack.last() && m_frameWidgetStack.last()->content())
        Q_EMIT m_frameWidgetStack.last()->content()->disappear();

    m_mouseAreaInter->unregisterRegion();

    // disconnect signal
    disconnect(m_mouseAreaInter, &DRegionMonitor::buttonRelease, this, &Frame::onMouseButtonReleased);

    // free all settings page
    m_delayKillerTimer->start();
}

const QScreen *Frame::screenForGeometry(const QRect &rect) const
{
    const qreal ratio = qApp->devicePixelRatio();

    for (const auto *s : qApp->screens())
    {
        const QRect &g(s->geometry());
        const QRect realRect(g.topLeft() / ratio, g.size());

        if (realRect.contains(rect.center()))
            return s;
    }

    return nullptr;
}

bool Frame::checkOnBoard(const QPoint &point)
{
    QList<DForeignWindow*> windowList = DWindowManagerHelper::instance()->currentWorkspaceWindows();

    for (auto it = windowList.crbegin(); it != windowList.crend(); ++it) {
        if ((*it)->handle()->geometry().contains(point)) {
            return (*it)->wmClass() == "onboard";
        }
    }

    return false;
}

bool Frame::event(QEvent *event)
{
    if (event->type() == QEvent::ApplicationFontChange) {
        Q_EMIT fontSizeChanged();
    }

    return DBlurEffectWidget::event(event);
}
