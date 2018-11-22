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

#include "widgets/contentwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>
#include <QScroller>
#include <QScrollBar>
#include <QScrollArea>
#include <QApplication>
#include <QWheelEvent>
#include <QPropertyAnimation>

#include "widgets/labels/tipslabel.h"

#include "separator.h"
#include "backbutton.h"

DWIDGET_USE_NAMESPACE

namespace {

const int DEFAULT_SPEED_TIME = 2;
const double MAX_SPEED_TIME = 14;
const int ANIMATION_DUARTION = 1400;

}  // namespace

namespace dcc {

ContentWidget::ContentWidget(QWidget *parent)
    : QWidget(parent),

      m_content(nullptr),
      m_speedTime(DEFAULT_SPEED_TIME)
{
    m_backBtn = new dcc::widgets::BackButton;
    m_backBtn->setAccessibleName("Back");

    m_title = new QLabel;
    m_title->setObjectName("ContentTitle");
    m_title->setAlignment(Qt::AlignCenter);

    m_contentTopLayout = new QVBoxLayout;
    m_contentTopLayout->setMargin(0);
    m_contentTopLayout->setSpacing(0);

    m_contentArea = new QScrollArea;
    m_contentArea->setWidgetResizable(true);
    m_contentArea->installEventFilter(this);
    m_contentArea->setFrameStyle(QFrame::NoFrame);
    m_contentArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
    m_contentArea->setContentsMargins(0, 0, 0, 0);

    // Supporting flick gestures and make wheel scrolling more smooth.
    m_contentArea->viewport()->installEventFilter(this);
    QScroller::grabGesture(m_contentArea, QScroller::LeftMouseButtonGesture);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(m_backBtn);
    titleLayout->addWidget(m_title);
    titleLayout->setContentsMargins(0, 0, m_backBtn->width(), 0);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addLayout(titleLayout);
    centralLayout->addSpacing(15);
    centralLayout->addWidget(new dcc::widgets::HSeparator);
    centralLayout->addLayout(m_contentTopLayout);
    centralLayout->addWidget(m_contentArea);
    centralLayout->setContentsMargins(0, 8, 8, 0);
    centralLayout->setSpacing(0);

    connect(m_backBtn, &dcc::widgets::BackButton::clicked, this, &ContentWidget::back);

    setLayout(centralLayout);
    setObjectName("ContentWidget");

    m_scrollAni = new QPropertyAnimation(m_contentArea->verticalScrollBar(), "value");
    m_scrollAni->setEasingCurve(QEasingCurve::OutQuint);
    m_scrollAni->setDuration(ANIMATION_DUARTION);

    m_wheelAni = new QPropertyAnimation(m_contentArea->verticalScrollBar(), "value");
    m_wheelAni->setEasingCurve(QEasingCurve::OutQuint);
    m_wheelAni->setDuration(ANIMATION_DUARTION);

    connect(m_scrollAni, &QPropertyAnimation::finished, this, [=] {
        m_scrollAni->setEasingCurve(QEasingCurve::OutQuint);
        m_scrollAni->setDuration(ANIMATION_DUARTION);
    });

    connect(m_wheelAni, &QPropertyAnimation::finished, this, [=] {
        m_wheelAni->setEasingCurve(QEasingCurve::OutQuint);
        m_wheelAni->setDuration(ANIMATION_DUARTION);
    });

    connect(m_wheelAni, &QPropertyAnimation::valueChanged, this, &ContentWidget::wheelValueChanged);
}

void ContentWidget::setTitle(const QString &title)
{
    m_title->setText(title);

    setAccessibleName(title);
}

void ContentWidget::sendBackSignal()
{
    Q_EMIT back();
}

QWidget *ContentWidget::setContent(QWidget * const w)
{
    QWidget *lastWidget = m_content;

    if (lastWidget)
        lastWidget->removeEventFilter(this);

    m_content = w;
    m_content->installEventFilter(this);
    m_contentArea->setWidget(m_content);

    return lastWidget;
}

void ContentWidget::scrollToWidget(QWidget * const w, bool animation)
{
    int y(0);
    QWidget *widget = w;
    while (widget != m_content)
    {
        y += widget->y();
        widget = qobject_cast<QWidget *>(widget->parent());
    }

    const int current = m_contentArea->verticalScrollBar()->value();
    const int value = std::min(m_contentArea->verticalScrollBar()->maximum(), y);

    if (animation)
    {
        m_scrollAni->setStartValue(current);
        m_scrollAni->setEndValue(value);

        m_scrollAni->start();
    } else {
        m_contentArea->verticalScrollBar()->setValue(value);
    }
}

void ContentWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    if (e->button() == Qt::BackButton)
        Q_EMIT back();
}

bool ContentWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (!m_content) return false;

    if (m_content && watched == m_contentArea && event->type() == QEvent::Resize)
        m_content->setFixedWidth(static_cast<QResizeEvent *>(event)->size().width());

    if (m_content && watched == m_contentArea->viewport() && event->type() == QEvent::Wheel) {
        const QWheelEvent *wheel = static_cast<QWheelEvent*>(event);

        // redirect all wheel events to this object so it can process the scroll animation, see
        // wheelEvent.
        QWheelEvent *newEvent =  new QWheelEvent(wheel->pos(), wheel->delta(), wheel->buttons(),
                                                 wheel->modifiers(), wheel->orientation());
        qApp->postEvent(this, newEvent);

        return true;
    }

    if (watched == m_content && event->type() == QEvent::LayoutRequest)
    {
        if (m_content->hasHeightForWidth())
            m_content->setMinimumHeight(m_content->heightForWidth(m_content->width()));
        else
            m_content->setFixedHeight(m_content->layout()->sizeHint().height());
    }

    return false;
}

void ContentWidget::stopScroll()
{
    m_speedTime = DEFAULT_SPEED_TIME;
    m_scrollAni->stop();
    m_wheelAni->stop();
}

void ContentWidget::wheelEvent(QWheelEvent *e)
{
    int offset = - e->delta();

    if (m_wheelAni->state() == QPropertyAnimation::Running) {
        m_speedTime += 0.02;
    } else {
        m_speedTime = DEFAULT_SPEED_TIME;
    }

    m_scrollAni->stop();
    m_wheelAni->stop();
    m_wheelAni->setStartValue(m_contentArea->verticalScrollBar()->value());
    m_wheelAni->setEndValue(m_contentArea->verticalScrollBar()->value() + offset * qMin(m_speedTime, MAX_SPEED_TIME));
    m_wheelAni->start();
}

void ContentWidget::mousePressEvent(QMouseEvent *e)
{
    stopScroll();

    QWidget::mousePressEvent(e);
}

}
