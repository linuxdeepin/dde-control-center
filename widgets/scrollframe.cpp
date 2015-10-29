#include <QVBoxLayout>
#include <QResizeEvent>
#include <QDebug>

#include "scrollframe.h"

ScrollFrame::ScrollFrame(QWidget *parent) :
    QFrame(parent)
{
    init();
}

QBoxLayout *ScrollFrame::headerLayout() const
{
    return m_headerWidget->layout();
}

QBoxLayout *ScrollFrame::mainLayout() const
{
    return m_mainWidget->layout();
}

void ScrollFrame::pushWidget(QWidget *widget)
{
    m_stackWidget->pushWidget(widget);
}

void ScrollFrame::popCurrentWidget()
{
    if(m_stackWidget->depth() > 1)
        m_stackWidget->popWidget();
}

void ScrollFrame::popAllWidget()
{
    if(m_stackWidget->depth() > 1)
        m_stackWidget->popWidget(m_stackWidget->getWidgetByIndex(1), true, m_stackWidget->depth() - 1);
}

void ScrollFrame::onCurrentWidgetSizeChanged(const QSize &size)
{
    m_stackWidget->setFixedHeight(size.height());
}

void ScrollFrame::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    m_stackWidget->setFixedWidth(e->size().width());
    m_scrollArea->setFixedWidth(e->size().width());
    m_scrollArea->setFixedHeight(e->size().height() - m_headerWidget->height());

    emit sizeChanged(e->size());
}

void ScrollFrame::init()
{
    m_headerWidget = new DVBoxWidget(this);
    m_scrollArea = new DScrollArea(this);
    m_stackWidget = new DStackWidget;
    m_mainWidget = new DVBoxWidget;

    m_headerWidget->show();
    m_stackWidget->setFixedWidth(width());
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_stackWidget);

    connect(m_headerWidget, &DVBoxWidget::sizeChanged, this, [this](QSize size){
        m_scrollArea->setFixedHeight(height() - size.height());
        m_scrollArea->move(0, size.height());
    });

    connect(m_stackWidget, &DStackWidget::currentWidgetChanged, this, [this](const QWidget *w){
        if(m_currentWidget)
            disconnect(m_currentWidget, &DBoxWidget::sizeChanged, this, &ScrollFrame::onCurrentWidgetSizeChanged);

        m_stackWidget->setFixedHeight(w->height());
        const DBoxWidget *widget = qobject_cast<const DBoxWidget*>(w);

        if(widget) {
            m_currentWidget = widget;
            connect(widget, &DBoxWidget::sizeChanged, this, &ScrollFrame::onCurrentWidgetSizeChanged);
        } else {
            m_currentWidget = nullptr;
        }

        emit currentMainWidgetChanged(w);
    });

    m_stackWidget->pushWidget(m_mainWidget);
}
