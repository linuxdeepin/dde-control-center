#include "contentwidget.h"

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

#include "labels/tipslabel.h"

#include "separator.h"
#include "backbutton.h"

DWIDGET_USE_NAMESPACE

namespace {

const int DEFAULT_SPEED_TIME = 1;
const double MAX_SPEED_TIME = 14;
const int ANIMATION_DUARTION = 1400;

}  // namespace

namespace dcc {

ContentWidget::ContentWidget(QWidget *parent)
    : QWidget(parent),

      m_content(nullptr),
      m_speedTime(DEFAULT_SPEED_TIME)
{
    dcc::widgets::BackButton *backBtn = new dcc::widgets::BackButton;
    backBtn->setAccessibleName("Back");

    m_navgationBtn = new DImageButton;
    m_navgationBtn->setNormalPic(":/frame/themes/dark/icons/nav_icon_normal.png");
    m_navgationBtn->setHoverPic(":/frame/themes/dark/icons/nav_icon_hover.png");
    m_navgationBtn->setPressPic(":/frame/themes/dark/icons/nav_icon_pressed.png");
    m_navgationBtn->setVisible(false);

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

    // Supporting flick gestures and make wheel scrolling more smooth.
    m_contentArea->viewport()->installEventFilter(this);
    QScroller::grabGesture(m_contentArea, QScroller::LeftMouseButtonGesture);

    QHBoxLayout *navLayout = new QHBoxLayout;
    navLayout->addWidget(m_navgationBtn);
    navLayout->setMargin(0);

    QWidget *navWidget = new QWidget;
    navWidget->setLayout(navLayout);
    navWidget->setFixedWidth(backBtn->width());

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(backBtn);
    titleLayout->addWidget(m_title);
    titleLayout->addWidget(navWidget);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addLayout(titleLayout);
    centralLayout->addSpacing(15);
    centralLayout->addWidget(new dcc::widgets::HSeparator);
    centralLayout->addLayout(m_contentTopLayout);
    centralLayout->addWidget(m_contentArea);
    centralLayout->setContentsMargins(8, 8, 8, 0);
    centralLayout->setSpacing(0);

    connect(backBtn, &dcc::widgets::BackButton::clicked, this, &ContentWidget::back);

    setLayout(centralLayout);
    setObjectName("ContentWidget");

    m_animation = new QPropertyAnimation(m_contentArea->verticalScrollBar(), "value");
    m_animation->setEasingCurve(QEasingCurve::OutQuint);
    m_animation->setDuration(ANIMATION_DUARTION);
    connect(m_animation, &QPropertyAnimation::finished, this, [=] {
        m_animation->setEasingCurve(QEasingCurve::OutQuint);
        m_animation->setDuration(ANIMATION_DUARTION);
    });

}

void ContentWidget::setTitle(const QString &title)
{
    m_title->setText(title);

    setAccessibleName(title);
}

void ContentWidget::sendBackSignal()
{
    emit back();
}

QWidget *ContentWidget::setContent(QWidget * const w)
{
    QWidget *lastWidget = m_content;

    if (lastWidget)
        lastWidget->removeEventFilter(this);

    m_content = w;
    m_content->installEventFilter(this);
    m_content->setFixedWidth(m_contentArea->width());
    m_contentArea->setWidget(m_content);

    return lastWidget;
}

void ContentWidget::scrollToWidget(QWidget * const w)
{
    int y(0);
    QWidget *widget = w;
    while (widget != m_content)
    {
        y += widget->y();
        widget = qobject_cast<QWidget *>(widget->parent());
    }

    m_contentArea->verticalScrollBar()->setValue(std::min(m_contentArea->verticalScrollBar()->maximum(), y));

//    m_contentArea->verticalScrollBar()->setValue(1900);
//    m_contentArea->ensureWidgetVisible(w, 0, 0);
    //    m_contentArea->verticalScrollBar()->setValue();
}

void ContentWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);

    if (e->button() == Qt::BackButton)
        emit back();
}

bool ContentWidget::eventFilter(QObject *watched, QEvent *event)
{
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
    m_animation->stop();
}

void ContentWidget::wheelEvent(QWheelEvent *e)
{
    // Active by touchpad
    if (e->pixelDelta().y() != 0) {
        QWheelEvent ve(e->pos(), e->globalPos(), e->pixelDelta()
                       , e->angleDelta(), e->delta() * 4/*speed up*/
                       , Qt::Vertical, e->buttons(), e->modifiers());
        QWidget::wheelEvent(&ve);
    }
    // Active by mouse
    else {
        int offset = - e->delta();
        if (m_animation->state() == QPropertyAnimation::Running) {
            m_speedTime += 0.2;
            // if the roll is kept constant, it will be faster and faster
            if (m_speed != offset) {
                m_speed = offset;
                m_speedTime = DEFAULT_SPEED_TIME;
            }
        }
        else {
            m_speedTime = DEFAULT_SPEED_TIME;
        }
        m_animation->stop();
        m_animation->setStartValue(m_contentArea->verticalScrollBar()->value());
        m_animation->setEndValue(m_contentArea->verticalScrollBar()->value() + offset * qMin(m_speedTime, MAX_SPEED_TIME));

        m_animation->start();
    }
}

void ContentWidget::mousePressEvent(QMouseEvent *e)
{
    stopScroll();

    QWidget::mousePressEvent(e);
}

}
