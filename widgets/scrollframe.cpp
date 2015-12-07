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
    setStackWidgetHeight(size.height());
}

void ScrollFrame::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    m_stackWidget->setFixedWidth(e->size().width());
    m_scrollArea->setFixedWidth(e->size().width());
    m_scrollArea->setFixedHeight(e->size().height() - m_headerWidget->height());
    if(m_stackWidget->height() < m_scrollArea->height())
        m_stackWidget->setFixedHeight(m_scrollArea->height());

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

    connect(m_stackWidget, &DStackWidget::switchWidgetFinished, this, [this] {
        const QWidget *w = m_stackWidget->currentWidget();

        if(m_currentWidget)
            disconnect(m_currentWidget.data(), SIGNAL(sizeChanged(QSize)), this, SLOT(onCurrentWidgetSizeChanged(QSize)));


        const DBoxWidget *widget = qobject_cast<const DBoxWidget*>(w);

        if(w)
            setStackWidgetHeight(w->height());

        if(widget) {
            m_currentWidget = widget;
            connect(widget, SIGNAL(sizeChanged(QSize)), this, SLOT(onCurrentWidgetSizeChanged(QSize)));
        } else {
            m_currentWidget = nullptr;
        }
    });

    connect(m_stackWidget, &DStackWidget::currentWidgetChanged, this, [this] (const QWidget *w){
        if(w && w->height() > m_stackWidget->height())
            setStackWidgetHeight(w->height());

        emit currentMainWidgetChanged(w);
    });

    m_stackWidget->pushWidget(m_mainWidget);
}

void ScrollFrame::setStackWidgetHeight(int height)
{
    m_stackWidget->setFixedHeight(qMax(height, m_scrollArea->height()));
}
