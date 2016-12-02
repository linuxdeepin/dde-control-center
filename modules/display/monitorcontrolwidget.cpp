#include "monitorcontrolwidget.h"
#include "monitorsground.h"
#include "displaymodel.h"
#include "monitorproxywidget.h"

#include <QVBoxLayout>
#include <QFrame>

MonitorControlWidget::MonitorControlWidget(QWidget *parent)
    : QWidget(parent),

      m_screensGround(new MonitorsGround),
      m_recognize(new QPushButton),
      m_split(new QPushButton),
      m_join(new QPushButton)
{
    m_recognize->setText(tr("Recognize"));
    m_split->setText(tr("Split"));
    m_join->setText(tr("Merge"));

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addWidget(m_recognize);
    btnsLayout->addWidget(m_split);
    btnsLayout->addWidget(m_join);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_screensGround);
    mainLayout->addLayout(btnsLayout);

    setLayout(mainLayout);

    connect(m_recognize, &QPushButton::clicked, this, &MonitorControlWidget::requestRecognize);
    connect(m_join, &QPushButton::clicked, this, &MonitorControlWidget::requestMerge);
}

void MonitorControlWidget::setDisplayModel(DisplayModel *model)
{
    m_screensGround->setDisplayModel(model);
}

void MonitorControlWidget::setScreensMerged(const bool merged)
{
    m_split->setVisible(merged);
    m_join->setVisible(!merged);
}
