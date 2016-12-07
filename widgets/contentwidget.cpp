#include "contentwidget.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>
#include <QScroller>
#include <QScrollBar>

#include <dseparatorhorizontal.h>

#include "backbutton.h"

DWIDGET_USE_NAMESPACE

ContentWidget::ContentWidget(QWidget *parent)
    : QWidget(parent),

      m_content(nullptr)
{
    dcc::widgets::BackButton *backBtn = new dcc::widgets::BackButton;

    m_title = new QLabel;
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setStyleSheet("color: white;");

    m_contentArea = new QScrollArea;
    m_contentArea->setWidgetResizable(true);
    m_contentArea->installEventFilter(this);
    m_contentArea->setFrameStyle(QFrame::NoFrame);
    m_contentArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_contentArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Supporting flick gestures and make wheel scrolling more smooth.
    m_contentArea->viewport()->installEventFilter(this);
    QScroller::grabGesture(m_contentArea->viewport(), QScroller::LeftMouseButtonGesture);

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(backBtn);
    titleLayout->addWidget(m_title);
    titleLayout->addSpacing(backBtn->width());

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addLayout(titleLayout);
    centeralLayout->addSpacing(15);
    centeralLayout->addWidget(new DSeparatorHorizontal);
    centeralLayout->addSpacing(15);
    centeralLayout->addWidget(m_contentArea);
    centeralLayout->setContentsMargins(8, 8, 8, 0);
    centeralLayout->setSpacing(0);

    connect(backBtn, &dcc::widgets::BackButton::clicked, this, &ContentWidget::back);

    setLayout(centeralLayout);
    setObjectName("ContentWidget");
}

void ContentWidget::setTitle(const QString &title)
{
    m_title->setText(title);
}

void ContentWidget::sendBackSignal()
{
    emit back();
}

QWidget *ContentWidget::setContent(QWidget * const w)
{
    QWidget *lastWidget = m_content;

    m_content = w;
    m_content->installEventFilter(this);
    m_content->setFixedWidth(m_contentArea->width());
    m_contentArea->setWidget(m_content);

    if (lastWidget)
        lastWidget->removeEventFilter(this);

    return lastWidget;
}

void ContentWidget::scrollToWidget(QWidget * const w)
{
    int y(0);
    QWidget *widget = w;
    while (widget != m_contentArea)
    {
        y += widget->y();
        widget = qobject_cast<QWidget *>(widget->parent());
    }

    qDebug() << Q_FUNC_INFO << y;

    m_contentArea->verticalScrollBar()->setValue(std::min(m_contentArea->verticalScrollBar()->maximum(), y));

//    m_contentArea->verticalScrollBar()->setValue(1900);
//    m_contentArea->ensureWidgetVisible(w, 0, 0);
//    m_contentArea->verticalScrollBar()->setValue();
}

bool ContentWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (m_content && watched == m_contentArea && event->type() == QEvent::Resize)
        m_content->setFixedWidth(static_cast<QResizeEvent *>(event)->size().width());

    if (m_content && watched == m_contentArea->viewport() && event->type() == QEvent::Wheel) {
        const QWheelEvent *wheel = static_cast<QWheelEvent*>(event);

        QWidget * viewport = qobject_cast<QWidget*>(m_contentArea->viewport());
        QScroller *scroller = QScroller::scroller(viewport);
        QScrollBar *vBar = m_contentArea->verticalScrollBar();

        const float curPos = viewport->height() * (vBar->value() * 1.0 / (vBar->maximum() - vBar->minimum()));
        const float delta = -wheel->delta();

        // FIXME(hualet): overshoots happen while scrolling down, but not scrolling up.
        if (delta > 0) {
            scroller->scrollTo(QPointF(0, curPos + delta));
        } else {
            scroller->scrollTo(QPointF(0, curPos + delta * 1.2));
        }

        return true;
    }

    if (watched == m_content && event->type() == QEvent::LayoutRequest)
        m_content->setFixedHeight(m_content->layout()->sizeHint().height());

    return false;
}
