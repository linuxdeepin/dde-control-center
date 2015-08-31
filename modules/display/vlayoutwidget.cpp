#include "vlayoutwidget.h"

VLayoutWidget::VLayoutWidget(QWidget *parent) :
    QWidget(parent),
    m_layout(new QVBoxLayout(this))
{
    m_layout->setMargin(0);
    m_layout->setContentsMargins(10, 0, 10, 0);
    m_layout->setSpacing(0);

    setMinimumHeight(0);
    setFixedWidth(310);
}

void VLayoutWidget::addWidget(QWidget *w, int height, Qt::Alignment alignment)
{
    m_layout->addWidget(w, 0, alignment);

    w->setFixedHeight(height);
    setMinimumHeight(height + minimumHeight() + m_layout->spacing());
}

