#include <QVBoxLayout>
#include <QResizeEvent>
#include <QDebug>

#include "scrollframe.h"

ScrollFrame::ScrollFrame(QWidget *parent) :
    QFrame(parent)
{
    init(new QVBoxLayout);
}

ScrollFrame::ScrollFrame(QBoxLayout *layout, QWidget *parent):
    QFrame(parent)
{
    init(layout);
}

void ScrollFrame::setHeaderWidget(QWidget *widget)
{
    m_headerWidget = widget;
    m_headerWidget->setParent(this);
    m_headerWidget->setFixedWidth(width());

    m_scrollArea->setFixedHeight(height() - widget->height());
    m_scrollArea->move(0, widget->height());
}

QBoxLayout *ScrollFrame::mainLayout() const
{
    return m_mainLayout;
}

void ScrollFrame::setMainLayout(QBoxLayout *layout)
{
    if(m_mainLayout)
        m_mainLayout->deleteLater();

    m_mainLayout = layout;
    m_mainWidget->setLayout(layout);
}

void ScrollFrame::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    m_mainWidget->setFixedWidth(e->size().width());
    m_scrollArea->setFixedWidth(e->size().width());

    if(m_headerWidget) {
        m_headerWidget->setFixedWidth(width());

        m_scrollArea->setFixedHeight(e->size().height() - m_headerWidget->height());
    } else {
        m_scrollArea->setFixedHeight(e->size().height());
    }

    emit sizeChanged(e->size());
}

void ScrollFrame::showEvent(QShowEvent *e)
{
    QFrame::showEvent(e);

    m_mainWidget->setFixedHeight(m_mainLayout->sizeHint().height());
    m_mainWidget->installEventFilter(this);
}

void ScrollFrame::hideEvent(QHideEvent *e)
{
    QFrame::hideEvent(e);

    m_mainWidget->removeEventFilter(this);
}

bool ScrollFrame::eventFilter(QObject *obj, QEvent *ee)
{
    if(ee->type() == QEvent::LayoutRequest && obj == m_mainWidget){
        m_mainWidget->setFixedHeight(m_mainLayout->sizeHint().height());
    }

    return QFrame::eventFilter(obj, ee);
}

void ScrollFrame::init(QBoxLayout *layout)
{
    layout->setMargin(0);
    layout->setSpacing(0);

    m_scrollArea = new DScrollArea(this);
    m_mainWidget = new QWidget;

    m_mainLayout = layout;

    m_mainWidget->setLayout(layout);
    m_mainWidget->setFixedWidth(width());
    m_scrollArea->setWidget(m_mainWidget);
    m_scrollArea->setFixedSize(size());
}

