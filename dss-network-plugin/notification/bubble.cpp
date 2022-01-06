/*
 * Copyright (C) 2014 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 *             listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
 *             fanpengcheng <fanpengcheng_cm@deepin.com>
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

#include "bubble.h"
#include "notificationentity.h"
#include "appicon.h"
#include "appbody.h"
#include "actionbutton.h"
#include "button.h"
#include "bubbletool.h"

#include <DWidget>

#include <QDebug>
#include <QTimer>
#include <QPropertyAnimation>
#include <QApplication>
#include <QGSettings>
#include <QMoveEvent>
#include <QBoxLayout>
#include <QParallelAnimationGroup>
#include <QTextDocument>
#include <QBitmap>

Bubble::Bubble(QWidget *parent, EntityPtr entity, OSD::ShowStyle style)
    : DBlurEffectWidget(parent)
    , m_entity(entity)
    , m_icon(new AppIcon(this))
    , m_body(new AppBody(this))
    , m_actionButton(new ActionButton(this))
    , m_closeButton(new DDialogCloseButton(this))
    , m_outTimer(new QTimer(this))
    , m_quitTimer(new QTimer(this))
{
    Q_ASSERT(entity);// 数据无效不应该继续往下走
    Q_UNUSED(style)

    initUI();
    initTimers();
    initConnections();
    m_beforeLocked = true;
    setEntity(entity);

    installEventFilter(this);
}

EntityPtr Bubble::entity() const
{
    return m_entity;
}

void Bubble::setEntity(EntityPtr entity)
{
    if (!entity) return;

    m_entity = entity;

    updateContent();

    int timeout = entity->timeout().toInt();
    //  0: never times out
    // -1: default 5s

    m_outTimer->stop();
    m_outTimer->setInterval(timeout == -1 ? BubbleTimeout : (timeout == 0 ? -1 : timeout));
    m_outTimer->setSingleShot(true);
    m_outTimer->start();
}

void Bubble::setEnabled(bool enable)
{
    if (!enable) {
        m_actionButton->hide();
        m_icon->hide();
        m_body->hide();
    } else {
        m_actionButton->setVisible(!m_actionButton->isEmpty());
        m_icon->show();
        m_body->show();
    }

    DWidget::setEnabled(enable);
}

void Bubble::updateGeometry()
{
    m_outTimer->stop();
    m_outTimer->setSingleShot(true);
    m_outTimer->start();

    Q_EMIT resetGeometry();
}

void Bubble::mousePressEvent(QMouseEvent *event)
{
    if (!isEnabled()) {
        return;
    }

    if (event->button() == Qt::LeftButton) {
        m_clickPos = event->pos();
        m_pressed = true;
    }
}

void Bubble::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isEnabled() /*|| m_userInter->locked()*/)
        return;

    if (m_pressed && m_clickPos == event->pos()) {
        if (!m_defaultAction.isEmpty()) {
            Q_EMIT actionInvoked(this, m_defaultAction);
            m_defaultAction.clear();
        }
        Q_EMIT dismissed(this);
    } else if (m_pressed && mapToGlobal(event->pos()).y() < 10) {
        //等待屏幕上方气泡消失再将通知插入到通知中心，否则会导致同一个通知出现在两个位置。
        QTimer::singleShot(AnimationTime + 10, this, [ = ] {
            Q_EMIT notProcessedYet(m_entity);
        });
        Q_EMIT dismissed(this);
    }

    m_pressed = false;

    DBlurEffectWidget::mouseReleaseEvent(event);
}

bool Bubble::eventFilter(QObject *obj, QEvent *event)
{
    // FIXME:过滤掉这两个事件，否则窗体上鼠标点击后，在界面之外进行mouseRlease,窗体接收不到mouseReleaseEvent
    if (obj) {
        if (event->type() == QEvent::MouseMove
                || event->type() == QEvent::Move) {
            return true;
        }
    }
    return false;
}

void Bubble::showEvent(QShowEvent *event)
{
    DBlurEffectWidget::showEvent(event);

    m_quitTimer->start();
}

void Bubble::hideEvent(QHideEvent *event)
{
    DBlurEffectWidget::hideEvent(event);

    m_quitTimer->start();
}

void Bubble::enterEvent(QEvent *event)
{
    if (!isEnabled())
        return;

    if (m_canClose) {
        m_closeButton->setVisible(true);
    }

    DBlurEffectWidget::enterEvent(event);
}

void Bubble::leaveEvent(QEvent *event)
{
    if (!isEnabled())
        return;

    if (m_canClose) {
        m_closeButton->setVisible(false);
    }

    DBlurEffectWidget::leaveEvent(event);
}

void Bubble::onOutTimerTimeout()
{
    if (containsMouse() || !isEnabled()) {
        m_outTimer->stop();
        m_outTimer->setSingleShot(true);
        m_outTimer->start();
    } else {
        //等待屏幕上方气泡消失再将通知插入到通知中心，否则会导致同一个通知出现在两个位置。
        QTimer::singleShot(AnimationTime + 10, this, [ = ] {
            Q_EMIT notProcessedYet(m_entity);
        });
        Q_EMIT expired(this);
    }
}

void Bubble::initUI()
{
    m_icon->setAccessibleName("AppIcon");
    m_body->setAccessibleName("AppBody");
    m_actionButton->setAccessibleName("ActionButton");
    m_closeButton->setAccessibleName("CloseButton");

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setMouseTracking(true);

    /* 模糊背景 */
    setMaskColor(DBlurEffectWidget::LightColor);
    setMaskAlpha(200);
    setBlurRectXRadius(15);
    setBlurRectYRadius(15);

    setFixedWidth(OSD::BubbleWidth(OSD::BUBBLEWINDOW));
    resize(OSD::BubbleSize(OSD::BUBBLEWINDOW));
    m_icon->setFixedSize(OSD::IconSize(OSD::BUBBLEWINDOW));
    m_body->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_closeButton->setIconSize(OSD::CloseButtonSize(OSD::BUBBLEWINDOW));
    m_closeButton->setVisible(false);
    m_actionButton->setVisible(false);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(BubbleSpacing);
    layout->setMargin(BubblePadding);
    layout->addWidget(m_icon);
    layout->addWidget(m_body);
    layout->addWidget(m_actionButton);
    layout->addWidget(m_closeButton);

    setLayout(layout);
}

void Bubble::initConnections()
{
    connect(m_actionButton, &ActionButton::buttonClicked, this, [ = ](const QString & action_id) {
        BubbleTool::actionInvoke(action_id, m_entity);
        Q_EMIT actionInvoked(this, action_id);
    });

    connect(m_closeButton, &DDialogCloseButton::clicked, this, [ = ]() {
        Q_EMIT dismissed(this);
    });

    connect(m_outTimer, &QTimer::timeout, this, &Bubble::onOutTimerTimeout);
}

void Bubble::initTimers()
{
    m_quitTimer->setInterval(60 * 1000);
    m_quitTimer->setSingleShot(true);

    m_outTimer->setInterval(BubbleTimeout);
    m_outTimer->setSingleShot(true);
}

void Bubble::updateContent()
{
    m_body->setTitle(m_entity->summary());
    if(m_entity->isShowPreview()) {
        m_body->setText(OSD::removeHTML(m_entity->body()));
        if (m_beforeLocked) {
            m_canClose = m_entity->actions().isEmpty();
            m_defaultAction = BubbleTool::processActions(m_actionButton, m_entity->actions());
        }
    } else {
        m_body->setText(tr("1 new message"));
        m_canClose = !m_entity->actions().isEmpty();
    }

    setFixedHeight(m_body->resizeHintHeight(BubbleWindowHeight));

    BubbleTool::processIconData(m_icon, m_entity);
}

bool Bubble::containsMouse() const
{
    return geometry().contains(QCursor::pos());
}

void Bubble::startMove(const QRect &startRect, const QRect &endRect, bool needDelete)
{
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);

    QPropertyAnimation *geometryAni = new QPropertyAnimation(this, "geometry", group);
    geometryAni->setStartValue(startRect);
    geometryAni->setEndValue(endRect);
    geometryAni->setEasingCurve(QEasingCurve::Linear);

    // 保证动画的速度恒定为 72pix/300ms
    int ySpace = qAbs(endRect.y() - startRect.y());
    int animationTime = int(ySpace * 1.0 / 72 * AnimationTime);
    geometryAni->setDuration(animationTime);

    group->addAnimation(geometryAni);
    // 需要删除时增加透明渐变效果
    if (needDelete) {
        QPropertyAnimation *opacityAni = new QPropertyAnimation(this, "windowOpacity", group);
        opacityAni->setStartValue(1);
        opacityAni->setEndValue(0);
        opacityAni->setDuration(animationTime + int(-BubbleStartPos * 1.0 / 72 * AnimationTime));
        group->addAnimation(opacityAni);
    }

    // 当需要更新位置，停止动画，直接刷新最终位置
    if (!needDelete) {
        connect(this, &Bubble::resetGeometry, this, [&] {
            group->stop();
            setFixedGeometry(endRect);
        });
    }

    group->start(QAbstractAnimation::DeleteWhenStopped);

    setEnabled(QSize(endRect.width(), endRect.height()) == OSD::BubbleSize(OSD::BUBBLEWINDOW));

    if (needDelete) {
        QTimer::singleShot(group->duration(), this, [ = ] {
            hide();
            close();
        });
    }
}

void Bubble::setBubbleIndex(int index)
{
    m_bubbleIndex = index;
}

void Bubble::setFixedGeometry(QRect rect)
{
    //只设置宽度，高度自适应即可，否则大字体的时候显示不全
    setFixedWidth(rect.width());
    rect.setHeight(height());
    setGeometry(rect);
}
