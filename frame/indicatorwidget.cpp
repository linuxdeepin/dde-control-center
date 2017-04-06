#include "indicatorwidget.h"

#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

IndicatorWidget::IndicatorWidget(QWidget *parent)
    : QWidget(parent),

      m_pluginsIndicator(new DPageIndicator),
      m_prevPluginBtn(new DImageButton),
      m_nextPluginBtn(new DImageButton)
{
    QHBoxLayout *centralLayout = new QHBoxLayout;

    centralLayout->addStretch();
    centralLayout->addWidget(m_prevPluginBtn);
    centralLayout->addSpacing(15);
    centralLayout->addWidget(m_pluginsIndicator);
    centralLayout->addSpacing(15);
    centralLayout->addWidget(m_nextPluginBtn);
    centralLayout->addStretch();

    m_prevPluginBtn->setObjectName("PrevBtn");
    m_prevPluginBtn->setAccessibleName("PrevBtn");
    m_nextPluginBtn->setObjectName("NextBtn");
    m_nextPluginBtn->setAccessibleName("NextBtn");
    m_pluginsIndicator->setObjectName("DPIndicator");

    m_prevPluginBtn->setVisible(false);
    m_nextPluginBtn->setVisible(false);

    m_pluginsIndicator->setFixedHeight(20);
    m_pluginsIndicator->setPageCount(2);
    m_pluginsIndicator->setCurrentPage(0);
    m_pluginsIndicator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(m_nextPluginBtn, &DImageButton::clicked, m_pluginsIndicator, &DPageIndicator::nextPage);
    connect(m_nextPluginBtn, &DImageButton::clicked, this, &IndicatorWidget::requestNext);
    connect(m_prevPluginBtn, &DImageButton::clicked, m_pluginsIndicator, &DPageIndicator::previousPage);
    connect(m_prevPluginBtn, &DImageButton::clicked, this, &IndicatorWidget::requestPrevious);

    setLayout(centralLayout);
}

void IndicatorWidget::enterEvent(QEvent *e)
{
    QWidget::enterEvent(e);

    m_pluginsIndicator->setVisible(false);
    m_prevPluginBtn->setVisible(true);
    m_nextPluginBtn->setVisible(true);
}

void IndicatorWidget::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);

    m_pluginsIndicator->setVisible(true);
    m_prevPluginBtn->setVisible(false);
    m_nextPluginBtn->setVisible(false);
}
