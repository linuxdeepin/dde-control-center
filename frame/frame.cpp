/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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
#include "settingswidget.h"
#include "framewidget.h"
#include "mainwidget.h"

#include <QApplication>
#include <QKeyEvent>
#include <QScreen>

Frame::Frame(QWidget *parent)
    : DBlurEffectWidget(parent),

      m_allSettingsPage(nullptr),
      m_allSettingsPageKiller(new QTimer(this)),
      m_mouseAreaInter(new DRegionMonitor(this)),
      m_displayInter(new DBusDisplay("com.deepin.daemon.Display", "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this)),
      m_launcherInter(new LauncherInter("com.deepin.dde.Launcher", "/com/deepin/dde/Launcher", QDBusConnection::sessionBus(), this)),

      m_primaryRect(m_displayInter->primaryRect()),
      m_appearAnimation(this, "geometry"),

      m_platformWindowHandle(this),
      m_wmHelper(DWindowManagerHelper::instance()),

      m_shown(false),
      m_autoHide(true),
      m_debugAutoHide(true)
{
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
//    m_platformWindowHandle.setShadowColor(Qt::red);

    m_allSettingsPageKiller->setSingleShot(true);
    m_allSettingsPageKiller->setInterval(60 * 1000);

    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMaximumWidth(FRAME_WIDTH);
    setMaskColor(DBlurEffectWidget::DarkColor);

    resize(0, height());

    connect(m_allSettingsPageKiller, &QTimer::timeout, this, &Frame::freeAllSettingsPage);
    connect(m_displayInter, &DBusDisplay::PrimaryRectChanged, this, &Frame::onScreenRectChanged);
    connect(m_launcherInter, &LauncherInter::Shown, this, &Frame::hideImmediately);
    connect(m_wmHelper, &DWindowManagerHelper::hasCompositeChanged, this, &Frame::adjustShadowMask);
    connect(&m_appearAnimation, &QPropertyAnimation::start, this, &Frame::adjustShadowMask);
    connect(&m_appearAnimation, &QPropertyAnimation::finished, this, &Frame::adjustShadowMask, Qt::QueuedConnection);

    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void Frame::startup()
{
    show();
}

void Frame::pushWidget(ContentWidget *const w)
{
    Q_ASSERT(!m_frameWidgetStack.empty());

    FrameWidget *fw = new FrameWidget(this);
    fw->setContent(w);
    fw->show();
    w->setVisible(true);

    m_frameWidgetStack.last()->hide();
    m_frameWidgetStack.push(fw);

    connect(w, &ContentWidget::back, this, &Frame::popWidget, Qt::UniqueConnection);
    connect(fw, &FrameWidget::contentDetached, this, &Frame::contentDetached, Qt::UniqueConnection);
}

void Frame::popWidget()
{
    Q_ASSERT(m_frameWidgetStack.size() > 1);

    // ensure top widget is sender()
    ContentWidget *w = static_cast<ContentWidget *>(sender());
    // w is nullptr if call this function directly
    Q_ASSERT(!w || w == m_frameWidgetStack.last()->content());

    // destroy the container
    m_frameWidgetStack.pop()->destroy();
    m_frameWidgetStack.last()->showBack();
}

void Frame::init()
{
    // main page
    MainWidget *w = new MainWidget(this);
    connect(w, &MainWidget::showAllSettings, this, &Frame::showAllSettings);
    connect(w, &MainWidget::showSettingPage, this, &Frame::showSettingsPage);
    m_frameWidgetStack.push(w);

    // frame position adjust
    onScreenRectChanged(m_primaryRect);

    QTimer::singleShot(1, this, &Frame::adjustShadowMask);

#ifdef QT_DEBUG
//    showSettingsPage("network", QString());
#endif

#ifdef DCC_KEEP_SETTINGS_LIVE
    prepareAllSettingsPage();
#endif
}

void Frame::adjustShadowMask()
{
    if (!m_wmHelper->hasComposite() ||
        !m_shown ||
        m_appearAnimation.state() == QPropertyAnimation::Running)
    {
        m_platformWindowHandle.setShadowRadius(0);
        m_platformWindowHandle.setFrameMask(QRegion());
        return;
    }

    const int radius = 20;
    const QSize s = size();
    const qreal ratio = devicePixelRatioF();
    const QRect r(0, radius / ratio, s.width() + radius / ratio, s.height());

    m_platformWindowHandle.setShadowRadius(radius);
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
}

void Frame::freeAllSettingsPage()
{
#ifndef DCC_KEEP_SETTINGS_LIVE
    Q_ASSERT(!m_allSettingsPage.isNull());

    // pass if not in main page
    if (m_frameWidgetStack.size() > 1)
        return;

    m_allSettingsPage->deleteLater();
#endif
}

void Frame::showAllSettings()
{
    Q_ASSERT(m_frameWidgetStack.size() == 1);

    prepareAllSettingsPage();

    pushWidget(m_allSettingsPage);
}

void Frame::showSettingsPage(const QString &moduleName, const QString &pageName)
{
    // ensure current is main page or all settings page
    while (m_frameWidgetStack.size() > 1 && m_frameWidgetStack.last()->content() != m_allSettingsPage)
        popWidget();

    // current is main page
    if (m_frameWidgetStack.size() == 1)
        prepareAllSettingsPage();

    if (pageName.isEmpty() && m_frameWidgetStack.size() == 1)
        showAllSettings();

    // show specificed page
//    m_allSettingsPage->showModulePage(moduleName, pageName);
    QMetaObject::invokeMethod(m_allSettingsPage, "showModulePage", Qt::QueuedConnection, Q_ARG(QString, moduleName), Q_ARG(QString, pageName));

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

    m_primaryRect = primaryRect;

    const qreal ratio = devicePixelRatioF();

    setFixedHeight(m_primaryRect.height() / ratio);

    DBlurEffectWidget::move(m_primaryRect.right() - width() * ratio + 1, m_primaryRect.y());
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

    if (isVisible() && geometry().contains(p))
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

    emit rectChanged(geometry());
}

void Frame::moveEvent(QMoveEvent *e)
{
    DBlurEffectWidget::moveEvent(e);

    emit rectChanged(geometry());
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
    m_appearAnimation.setEndValue(r);
    m_appearAnimation.start();

    // show frame
    DBlurEffectWidget::show();
    DBlurEffectWidget::activateWindow();

    // notify top widget appear
    if (m_frameWidgetStack.last() && m_frameWidgetStack.last()->content())
    {
        QTimer::singleShot(m_frameWidgetStack.last()->animationDuration(), [=] {
            emit m_frameWidgetStack.last()->content()->appear();
        });
    }

    // register global mouse area
    m_mouseAreaInter->registerRegion();

    // connect signal
    connect(m_mouseAreaInter, &DRegionMonitor::buttonRelease, this, &Frame::onMouseButtonReleased, Qt::UniqueConnection);

    // prepare all settings page
    m_allSettingsPageKiller->stop();
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
    m_appearAnimation.setEndValue(r);
    m_appearAnimation.start();

//    QTimer::singleShot(m_appearAnimation.duration(), this, &QFrame::hide);
    QTimer::singleShot(m_appearAnimation.duration(), [=] {
        if (m_appearAnimation.state() != QPropertyAnimation::Running)
            DBlurEffectWidget::hide();
    });

    // notify top widget disappear
    if (m_frameWidgetStack.last() && m_frameWidgetStack.last()->content())
        emit m_frameWidgetStack.last()->content()->disappear();

    m_mouseAreaInter->unregisterRegion();

    // disconnect signal
    disconnect(m_mouseAreaInter, &DRegionMonitor::buttonRelease, this, &Frame::onMouseButtonReleased);

    // free all settings page
    m_allSettingsPageKiller->start();
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
    move(r.topLeft());

    DBlurEffectWidget::hide();

    // notify top widget disappear
    if (m_frameWidgetStack.last() && m_frameWidgetStack.last()->content())
        emit m_frameWidgetStack.last()->content()->disappear();

    m_mouseAreaInter->unregisterRegion();

    // disconnect signal
    disconnect(m_mouseAreaInter, &DRegionMonitor::buttonRelease, this, &Frame::onMouseButtonReleased);

    // free all settings page
    m_allSettingsPageKiller->start();
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
