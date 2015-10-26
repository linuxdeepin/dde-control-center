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

void ScrollFrame::resizeEvent(QResizeEvent *e)
{
    QFrame::resizeEvent(e);

    m_mainWidget->setFixedWidth(e->size().width());
    m_scrollArea->setFixedWidth(e->size().width());
    m_headerWidget->setFixedWidth(e->size().width());
    m_scrollArea->setFixedHeight(e->size().height() - m_headerWidget->height());

    emit sizeChanged(e->size());
}

void ScrollFrame::init()
{
    m_scrollArea = new DScrollArea(this);
    m_headerWidget = new DVBoxWidget(this);
    m_mainWidget = new DVBoxWidget;

    m_headerWidget->setFixedWidth(width());
    m_mainWidget->setFixedWidth(width());

    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidget(m_mainWidget);
    m_scrollArea->setFixedSize(size());

    connect(m_headerWidget, &DVBoxWidget::sizeChanged, this, [this](QSize size){
        m_scrollArea->setFixedHeight(height() - size.height());
        m_scrollArea->move(0, size.height());
    });
}

