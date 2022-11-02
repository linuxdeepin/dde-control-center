// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dockpopupwindow.h"
#include "imageutil.h"
#include "utils.h"

#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QAccessible>
#include <QAccessibleEvent>

DWIDGET_USE_NAMESPACE

DockPopupWindow::DockPopupWindow(QWidget *parent)
    : DArrowRectangle(ArrowBottom, FloatWidget, parent)
    , m_model(false)
    , m_regionInter(new DRegionMonitor(this))
    , m_enableMouseRelease(true)
{
    setMargin(0);
    m_wmHelper = DWindowManagerHelper::instance();

    compositeChanged();

    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint);

    setShadowBlurRadius(20);
    setRadius(6);
    setShadowYOffset(2);
    setShadowXOffset(0);
    setArrowWidth(18);
    setArrowHeight(10);

    connect(m_wmHelper, &DWindowManagerHelper::hasCompositeChanged, this, &DockPopupWindow::compositeChanged);
    connect(m_regionInter, &DRegionMonitor::buttonRelease, this, &DockPopupWindow::onGlobMouseRelease);
}

DockPopupWindow::~DockPopupWindow()
{
}

bool DockPopupWindow::model() const
{
    return isVisible() && m_model;
}

void DockPopupWindow::setContent(QWidget *content)
{
    QWidget *lastWidget = getContent();
    if (lastWidget)
        lastWidget->removeEventFilter(this);
    content->installEventFilter(this);

    QAccessibleEvent event(this, QAccessible::NameChanged);
    QAccessible::updateAccessibility(&event);

    if (!content->objectName().trimmed().isEmpty())
        setAccessibleName(content->objectName() + "-popup");

    DArrowRectangle::setContent(content);
}

void DockPopupWindow::show(const QPoint &pos, const bool model)
{
    m_model = model;
    m_lastPoint = pos;

    show(pos.x(), pos.y());

    if (m_regionInter->registered()) {
        m_regionInter->unregisterRegion();
    }

    if (m_model) {
        m_regionInter->registerRegion();
    }
    blockButtonRelease();
}

void DockPopupWindow::show(const int x, const int y)
{
    m_lastPoint = QPoint(x, y);
    blockButtonRelease();

    DArrowRectangle::show(x, y);
}

void DockPopupWindow::blockButtonRelease()
{
    // 短暂的不处理鼠标release事件，防止出现刚显示又被隐藏的情况
    m_enableMouseRelease = false;
    QTimer::singleShot(10, this, [this] {
        m_enableMouseRelease = true;
    });
}

void DockPopupWindow::hide()
{
    if (m_regionInter->registered())
        m_regionInter->unregisterRegion();

    DArrowRectangle::hide();
}

void DockPopupWindow::showEvent(QShowEvent *e)
{
    DArrowRectangle::showEvent(e);

    QTimer::singleShot(1, this, &DockPopupWindow::ensureRaised);
}

void DockPopupWindow::enterEvent(QEvent *e)
{
    DArrowRectangle::enterEvent(e);

    QTimer::singleShot(1, this, &DockPopupWindow::ensureRaised);
}

bool DockPopupWindow::eventFilter(QObject *o, QEvent *e)
{
    if (o != getContent() || e->type() != QEvent::Resize)
        return false;

    // FIXME: ensure position move after global mouse release event
    if (isVisible()) {
        QTimer::singleShot(10, this, [=] {
            // NOTE(sbw): double check is necessary, in this time, the popup maybe already hided.
            if (isVisible())
                show(m_lastPoint, m_model);
        });
    }

    return false;
}

void DockPopupWindow::onGlobMouseRelease(const QPoint &mousePos, const int flag)
{
    Q_ASSERT(m_model);

    if (!m_enableMouseRelease)
        return;

    if ((flag != DRegionMonitor::WatchedFlags::Button_Left) && (flag != DRegionMonitor::WatchedFlags::Button_Right)) {
        return;
    }

    const QRect rect = QRect(pos(), size());
    if (rect.contains(mousePos))
        return;

    emit accept();

    m_regionInter->unregisterRegion();
}

void DockPopupWindow::compositeChanged()
{
    setBackgroundColor(QColor(235, 235, 235, 80));
    setBorderColor(QColor(235, 235, 235, 80));
}

void DockPopupWindow::ensureRaised()
{
    if (isVisible()) {
        QWidget *content = getContent();
        if (!content || !content->isVisible()) {
            this->setVisible(false);
        } else {
            raise();
        }
    }
}
