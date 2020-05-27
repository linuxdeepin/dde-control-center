/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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
#include "framewidget.h"
#include "framecontentwrapper.h"
#include "widgets/contentwidget.h"

#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <DWindowManagerHelper>

using namespace dcc;

FrameWidget::FrameWidget(FrameContentWrapper *parent)
    : TranslucentFrame(parent),

#ifndef DISABLE_OPACITY_ANIMATION
      m_opacityEffect(new QGraphicsOpacityEffect(this)),
#endif
      m_slidePosAni(new QPropertyAnimation(this, "pos")),
      m_content(nullptr),
      m_wmHelper(DWindowManagerHelper::instance())
{
#ifndef DISABLE_OPACITY_ANIMATION
    m_opacityEffect->setOpacity(1.0);
#endif
    m_slidePosAni->setEasingCurve(QEasingCurve::InOutCubic);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(centralLayout);
    setMinimumSize(FRAME_WIDTH - NAVBAR_WIDTH, parent->height());
    setMaximumSize(FRAME_WIDTH - NAVBAR_WIDTH, parent->height());
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    parent->installEventFilter(this);

#ifndef DISABLE_OPACITY_ANIMATION
    setGraphicsEffect(m_opacityEffect);

    // change widget opacity
    connect(m_slidePosAni, &QPropertyAnimation::valueChanged, this, [=](const QVariant &value) {
        qreal opacity = qBound(qreal(0.0), 1.0 - qAbs(qreal(value.toPoint().x())) / FRAME_WIDTH, qreal(1.0));
        const bool isOpacity = !qFuzzyCompare(opacity, 1.0);

        // junk value
        if (m_slidePosAni->state() != QPropertyAnimation::Running && isOpacity)
            return;

        m_opacityEffect->setEnabled(isOpacity);
        m_opacityEffect->setOpacity(opacity);
    });
#endif
    connect(m_wmHelper, &DWindowManagerHelper::hasCompositeChanged, this, &FrameWidget::onCompositeChanged);

    QTimer::singleShot(1, this, &FrameWidget::onCompositeChanged);
}

ContentWidget *FrameWidget::setContent(ContentWidget * const c)
{
    ContentWidget *lastContent = m_content;
    m_content = c;
    layout()->addWidget(c);

    return lastContent;
}

ContentWidget *FrameWidget::content()
{
    return m_content;
}

void FrameWidget::show()
{
    QWidget::show();

    m_slidePosAni->setStartValue(QPoint(width(), 0));
    m_slidePosAni->setEndValue(QPoint());
    m_slidePosAni->start();
}

void FrameWidget::hide()
{
    m_slidePosAni->setStartValue(QPoint());
    m_slidePosAni->setEndValue(QPoint(-width(), 0));
    m_slidePosAni->start();

    // ignore mouse events
    setAttribute(Qt::WA_TransparentForMouseEvents);
    QTimer::singleShot(m_slidePosAni->duration(), [=] { setAttribute(Qt::WA_TransparentForMouseEvents, false); QWidget::hide(); });
}

void FrameWidget::showBack()
{
    QWidget::show();

    m_slidePosAni->setStartValue(QPoint(-width(), 0));
    m_slidePosAni->setEndValue(QPoint());
    m_slidePosAni->start();
}

void FrameWidget::hideBack()
{
    m_slidePosAni->setStartValue(QPoint());
    m_slidePosAni->setEndValue(QPoint(width(), 0));
    m_slidePosAni->start();

    QTimer::singleShot(m_slidePosAni->duration(), this, &QWidget::hide);
}

void FrameWidget::destroy()
{
    m_slidePosAni->setStartValue(QPoint());
    m_slidePosAni->setEndValue(QPoint(width(), 0));
    m_slidePosAni->start();

    QTimer::singleShot(m_slidePosAni->duration(), this, &FrameWidget::destroySelf);
}

int FrameWidget::animationDuration() const
{
    return m_slidePosAni->duration();
}

bool FrameWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parent() && event->type() == QEvent::Resize)
        setFixedSize(static_cast<QResizeEvent *>(event)->size());

    return false;
}

void FrameWidget::destroySelf()
{
    if (m_content)
    {
        m_content->setParent(nullptr);
        Q_EMIT contentDetached(m_content);
    }

    deleteLater();
}

void FrameWidget::onCompositeChanged()
{
    m_slidePosAni->setDuration(m_wmHelper->hasComposite() ? 300 : 150);
}
